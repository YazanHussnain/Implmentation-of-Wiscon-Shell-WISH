#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#define MAX_CMD_LEN 256

void execute_cd(char **arg);
void execute_exit(char **arg);
char *execute_path(char **arg, char *path);
void check_bash_mode(int argc, char*argv[]);
void print_prompt(int interactive);
int separate_cmd(char cmd[]);
void split_cmd(int p);
void check_redirection(void);

char error_message[30] = "An error has occurred";

typedef struct shell_info
{
	int notAllowed;
	int interactive;
	char *commands[100];
	char *args[100];
	char *filename;
	int outputRedirect;
	int multipleFiles;
	int cur_cmd;
	FILE *stream;
}shell_info;

shell_info shell;

void initialize_shell(void){
	shell.notAllowed = 0;
	shell.interactive = 1;
	shell.outputRedirect = 0;
	shell.multipleFiles = 0;
	shell.cur_cmd = 0;
	shell.stream = stdin;
}

void execute_cd(char **arg) {
    if (chdir(arg[1]) == -1) {
        fprintf(stderr, "%s\n", error_message);
    }
}

void execute_exit(char **arg) {
    if (arg[1] != NULL)
	{
		fprintf(stderr, "%s\n", error_message);
	}
	else
	{
		exit(0);
	}
}

char *execute_path(char **arg, char *path) {

	int num_path = 0;
	char *pathCopy = strdup(path);
	int i = 1;
	
    while (arg[i] != NULL)
	{
				// Add the specified directory to the search path
		pathCopy = realloc(pathCopy, strlen(pathCopy) + strlen(arg[i]) + 2);
		strcat(pathCopy, ":");
		strcat(pathCopy, arg[i]);
		//printf("%s\n", pathCopy);
		setenv("PATH", pathCopy, 1);
		putenv("PATH");
		//printf("Current search path: %s\n", pathCopy);
		i++;
		num_path++;
	}

	shell.notAllowed = 0;
	return pathCopy;
}

void check_bash_mode(int argc, char*argv[]){

	if (argc == 2)
	{
		shell.interactive = 0;
		if ((shell.stream = fopen(argv[1], "r")) == NULL)
		{
			fprintf(stderr, "%s\n", error_message);
			exit(1);
		}
	}
}

void print_prompt(int interactive){
	if (interactive)
	{
		printf("wish> ");
	}
}

int separate_cmd(char cmd[]) {
	char *command = strtok(cmd, "&");
	int k = 0;
	while (command != NULL)
	{
		shell.commands[k] = command;
		command = strtok(NULL, "&");
		k++;
		//fprintf(stdout, "%s\n", commands[k]);
	}
	return k;
}

void split_cmd(int p){
	char *arg = strtok(shell.commands[p], " \t\n");
	int i = 0;
	while (arg != NULL)
	{
		shell.args[i] = arg;
		arg = strtok(NULL, " \t\n");
		i++;
	}
	shell.args[i] = NULL;

}

void check_redirection(void){
	int j;
	shell.multipleFiles = 0;
	shell.outputRedirect = 0;
	for (j = 0; shell.args[j] != NULL; j++)
	{
		if ((strcmp(shell.args[j], ">") == 0) && (strcmp(shell.args[0], ">") != 0))
		{
			if (shell.args[j + 2] != NULL)
			{
				shell.multipleFiles = 1;
			}
			else
			{
				shell.outputRedirect = 1;
				shell.filename = shell.args[j + 1];
				shell.args[j] = NULL;
			}
		}
		else
		{
			char *pos = strchr(shell.args[j], '>');

			if (pos)
			{
				if (shell.args[j + 1] != NULL)
				{
					shell.multipleFiles = 1;
				}
				else
				{
					char *arg = strtok(shell.args[j], ">");
					shell.args[j] = arg;
					shell.outputRedirect = 1;
					arg = strtok(NULL, ">");
					shell.filename = arg;
				}
			}
		}
	}
}


int main(int argc, char *argv[])
{
	char cmd[MAX_CMD_LEN];
	char *path = getenv("PATH");
	char *pathCopy = strdup(path);
	int num_cmd = 0;
	int breakLoop = 0;

	initialize_shell();

	if (argc > 2)
	{
		fprintf(stderr, "%s\n", error_message);
		free(pathCopy);
		exit(1);
	}

	check_bash_mode(argc, argv);

	while (1)
	{
		// Get user input
		print_prompt(shell.interactive);

		if(fgets(cmd, MAX_CMD_LEN, shell.stream) == NULL){
			free(pathCopy);
			exit(0);
		}

		// Remove the newline character from the end of the input
		cmd[strcspn(cmd, "\n")] = '\0';
		if (strlen(cmd) == 0)
		{
			continue;
		}

		num_cmd = separate_cmd(cmd);

		shell.cur_cmd = 0;
		while (shell.cur_cmd < num_cmd)
		{
			// Split the input into an array of arguments
			
			split_cmd(shell.cur_cmd);

			if (shell.args[0] == NULL)
			{
				shell.cur_cmd++;
				continue;
			}

			check_redirection();
			
			if (shell.multipleFiles)
			{
				fprintf(stderr, "%s\n", error_message);
				shell.cur_cmd++;
				continue;
			}

			// Handle the 'cd' command separately using chdir()
			if (strcmp(shell.args[0], "exit") == 0)
			{
				if (shell.args[1] != NULL)
				{
					fprintf(stderr, "%s\n", error_message);
				}
				else
				{
					breakLoop = 1;
					break;
				}
			}
			else if (strcmp(shell.args[0], "cd") == 0)
			{
				execute_cd(shell.args);
			}

			// Handle the 'path' command separately
			else if (strcmp(shell.args[0], "path") == 0)
			{
				if (shell.args[1] == NULL)
				{
					shell.notAllowed = 1;
				}
				else
				{
					pathCopy = execute_path(shell.args, path);
				}
			}
			else if (!shell.notAllowed)
			{
			 	// Fork a child process to execute the command
				pid_t pid = fork();

				if (pid == 0)
				{
				 	// Child process: execute the command
					char *fullpath;
					char *token;
					int i = 0;
					int found = 0;
					while (!found && (token = strtok(pathCopy, ":")) != NULL)
					{
						i++;
						pathCopy = NULL;
						fullpath = malloc(strlen(token) + strlen(shell.args[0]) + 2);
						sprintf(fullpath, "%s/%s", token, shell.args[0]);
						if (access(fullpath, X_OK) == 0)
						{
						 	//args[0] = fullpath;
							//printf("Executing command: %s\n", args[0]);

							if (shell.outputRedirect)
							{
								int output_fd = open(shell.filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
								if (output_fd == -1)
								{
									fprintf(stderr, "%s\n", error_message);
									free(pathCopy);
									exit(EXIT_FAILURE);
								}

								if (dup2(output_fd, STDOUT_FILENO) == -1)
								{
									fprintf(stderr, "%s\n", error_message);
									free(pathCopy);
									exit(EXIT_FAILURE);
								}
							}

							execv(fullpath, shell.args);
							perror(shell.args[0]);
							free(fullpath);
							//close(output_fd);
							free(pathCopy);
							exit(EXIT_FAILURE);
						}

						free(fullpath);
					}

					if (i == 0)
					{
						fprintf(stderr, "%s\n", error_message);
					}
					else
					{
						fprintf(stderr, "%s\n", error_message);
					}
					free(pathCopy);
					exit(EXIT_FAILURE);
				}
				else if (pid < 0)
				{
				 	// Fork error
					fprintf(stderr, "%s\n", error_message);
				}
				else
				{
					// Parent process:
				}
			}
			else
			{
				fprintf(stderr, "%s\n", error_message);
			}

			shell.cur_cmd++;
		}
		if(breakLoop){
			break;
		}

		while (wait(NULL) > 0);
	}
	free(pathCopy);
	return 0;
}