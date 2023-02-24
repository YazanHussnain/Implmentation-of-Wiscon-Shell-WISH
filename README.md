# **Introduction**

The Wiscon Shell ( **WISH** ) is a shell that I implemented in C as part of the ostep-projects assignment. WISH provides users with a set of built-in functions and allows them to execute external programs using the **execv()** function in C. It also offers support for parallel commands and output redirection.

By utilizing the **execv()** function, **WISH** allows users to execute external programs from within the shell and manipulate their input and output streams. This functionality provides users with a way to interact with the operating system in a more flexible and customizable way.
 Furthermore, WISH's support for parallel commands allows users to execute multiple tasks simultaneously, improving efficiency and productivity. The output redirection feature enables users to direct the output of a command to a file or another location, providing added flexibility and functionality.

**WISH** provides users with a powerful set of tools to work with, making it a useful addition to any programmer's toolkit.

# **Initialize Shell**

In my implementation of the Wiscon Shell (WISH) in C, I utilized a struct called shell\_info to store various pieces of information relevant to the shell's behavior. The struct contains several fields, including

- notAllowed
- interactive
- commands
- args
- filename
- outputRedirect
- multipleFiles
- cur\_cmd
- stream.

**notAllowed** is an integer that indicates whether or not certain operations are not allowed in the shell

**interactive** indicates whether or not the shell is being run in interactive mode.

The **commands** and **args** fields are arrays of character pointers that represent the commands and arguments to be executed by the shell.

**filename** is a character pointer that represents the name of the file to which output should be redirected

**outputRedirect** is an integer that indicates whether or not output redirection is being used **.**

**multipleFiles** is another integer that indicates whether or not output should be redirected to multiple files.

**cur\_cmd** is an integer that represents the current command that the shell is executing, and **stream** is a file stream that represents the input stream for the shell.

To initialize the shell struct with default values, I created the **initialize\_shell()** function. This function sets notAllowed, outputRedirect, and multipleFiles to 0, and sets interactive to 1. It also sets cur\_cmd to 0 and stream to stdin, which is the standard input stream.

**Code Snippet**

```C
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
```

# **Shell Mode**

**WISH** shell support both interactive and bash mode. To check whether to run shell in interactive or bash mode **check\_bash\_mode** function is implemented. check\_bash\_mode() which takes in two arguments, argc and argv[]. The purpose of this function is to check if the shell is running in interactive or non-interactive mode.

The function first checks if argc is equal to 2, which indicates that the shell is running in non-interactive mode. If this is the case, shell.interactive is set to 0, indicating that the shell is not being run in interactive mode.

Next, the function attempts to open the file specified in argv[1] for reading using the fopen() function. If the file cannot be opened, the function prints an error message to stderr and exits the program with a status code of 1.

Overall, the check\_bash\_mode() function is useful in determining whether the shell is running in interactive or non-interactive mode and allowing the program to handle input accordingly. If the shell is running in non-interactive mode, the function opens a file for input instead of waiting for user input from the command line.

**Code Snippet**
```C
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
```

# **Prompt Print**

**print\_prompt()** which takes in an integer argument interactive. The purpose of this function is to print the shell prompt to the console.

The function first checks if interactive is nonzero, which indicates that the shell is being run in interactive mode. If this is the case, the function prints the prompt string " **wish\>**" to the console using the printf() function.

Overall, the print\_prompt() function is useful in providing a visual indicator to the user that the shell is waiting for input. By printing the prompt string "wish\> " to the console, the user knows that they can enter a command for the shell to execute.

**Code Snippet**
```C
void print_prompt(int interactive){
    if (interactive)
    {
        printf("wish> ");
    }
}
```

# **Separate Commands**

The separate\_cmd() function is an implementation in the WISH shell which allows the user to execute multiple commands in parallel by splitting a command string into separate commands, where each command is delimited by the '&' character.

The function first takes a command string cmd as an argument and uses the strtok() function to split the command string into separate commands using the '&' character as a delimiter. The function stores each command in the shell.commands array, which is a character pointer array in the shell\_info struct. The variable k is used to keep track of the current index in the shell.commands array, and is incremented after each command is added to the array.

After each command is separated and stored in the shell.commands array, the function returns the number of commands that were separated from the original command string.

The separate\_cmd() function is a useful implementation in the WISH shell which provides increased flexibility and functionality to the user by allowing them to execute multiple commands simultaneously. By splitting the command string into individual commands, the user can execute each command separately and simultaneously, improving the efficiency and usability of the shell.

**Code Snippet**

```C
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
```

# **Split Command**

The split\_cmd() function is an implementation in the WISH shell which is used to split a command into separate arguments. The function takes an integer p as its argument, which is the index of the command to be split in the shell.commands array.

The function first uses the strtok() function to split the command string at index p into separate arguments using whitespace characters as delimiters. The first call to strtok() uses shell.commands[p] as the input string and " \t\n" as the delimiter, where " \t\n" matches any space, tab, or newline character. The subsequent calls to strtok() use NULL as the input string to continue splitting the original string.

Each argument is stored in the shell.args array, which is a character pointer array in the shell\_info struct. The variable i is used to keep track of the current index in the shell.args array, and is incremented after each argument is added to the array.

Finally, the function sets the last element of shell.args to NULL, indicating the end of the argument list.

The split\_cmd() function is a useful implementation in the WISH shell which provides a way to parse command-line arguments for the execution of external commands using the execv() function in C. By splitting the command string into separate arguments, the shell can pass these arguments to external commands as command-line arguments, allowing the user to execute more complex commands and programs.

**Code Snippet**

```C
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
```

# **Check Redirection**

The check\_redirection() function in WISH shell is used to determine if output redirection is specified in the current command and if so, it sets the necessary flags and variables in the shell\_info struct to handle the redirection.

The function first initializes the shell.multipleFiles and shell.outputRedirect variables to 0. Then it loops through all the arguments in the current command, stopping when it encounters a NULL argument. For each argument, it checks if the argument is equal to the "\>" character, which indicates output redirection.

If the "\>" character is found, the function checks if it is the first argument in the command, in which case it is considered an error. Otherwise, the function checks if there are additional arguments (more the one output files) after the "\>" character, which would indicate multiple output files. If there are additional arguments, the function sets the shell.multipleFiles flag to 1.

If there are no additional arguments (only one file given) after the "\>" character, the function sets the shell.outputRedirect flag to 1 and stores the filename to which output should be redirected in the shell.filename variable. The "\>" character is replaced with a NULL terminator to separate the command and the filename.

If the "\>" character is found within an argument (not as a separate argument), the function first splits the argument into two separate strings using the strchr() function to locate the "\>" character. If there are additional arguments (more the one output files) after the "\>" character, the function sets the shell.multipleFiles flag to 1.

If there are no additional arguments (only one file given) after the "\>" character, the function sets the shell.outputRedirect flag to 1 and stores the filename to which output should be redirected in the shell.filename variable. The "\>" character and any leading or trailing whitespace characters are removed from the argument, and the remaining argument is stored in the shell.args array.

The check\_redirection() function is an important implementation in the WISH shell which allows for output redirection and multiple output files to be specified on the command line, allowing the user to control the output of their commands and programs.

**Code Snippet**

```C
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
```

# **Execute cd Command**

The above code defines a function execute\_cd which is used to change the current working directory of the shell.

The function takes a char \*\*arg as an argument which contains the path to the directory to which the shell should change the working directory.

The function uses the chdir function to change the working directory to the specified path. If the chdir function fails, it returns -1 and the function prints an error message using the fprintf function to the standard error stream (stderr).

The error message is retrieved from the error\_message variable which is defined above in the code. This function is called by the shell when the user enters the cd command, followed by the desired directory path, as an input to the shell.

**Code Snippet**

```C
void execute_cd(char **arg) {
    if (chdir(arg[1]) == -1) {
        fprintf(stderr, "%s\n", error_message);
    }
}
```

# **Execute path Command**

The function execute\_path takes two arguments: char \*\*arg, which is an array of strings containing the command and its arguments, and char \*path, which is the current search path for executables. The function first initializes num\_path to zero and creates a copy of the path string using strdup.

Then, the function iterates through the arg array starting from index 1, which skips the command itself, and appends each argument to the pathCopy string. The new string is created by dynamically allocating memory using realloc, and then appending : and the argument to the end of the string using strcat. The PATH environment variable is then updated with the new search path using setenv and putenv. Finally, the function sets shell.notAllowed to zero and returns the updated pathCopy string.

This function is used to modify the search path for executables to include directories specified by the user. This allows the shell to run executables located in directories other than the default search path.

**Code Snippet**

```C
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
```

# **Explain Main**

The program reads user input, processes it and executes the corresponding command. The main function of the program contains a loop that waits for user input, processes it and executes the command. The loop continues until the user enters the "exit" command or there is an error.

The program starts by initializing the shell and getting the system path using the getenv function. It then makes a copy of the path using the strdup function and enters an infinite loop to read user input.

```C
char cmd[MAX_CMD_LEN];
char *path = getenv("PATH");
char *pathCopy = strdup(path);
int num_cmd = 0;
int breakLoop = 0;

initialize_shell();
```

Inside the loop, the program prints a prompt and reads the user input using fgets. The program then removes the newline character at the end of the input using strcspn function. If the length of the input is zero, the program continues to the next iteration of the loop.

```C
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
```

The program then separates the input into an array of commands using separate\_cmd function. It sets the current command to zero and starts a loop that executes the command. Inside the loop, the program splits the command into an array of arguments using split\_cmd function.

```C
num_cmd = separate_cmd(cmd);

shell.cur_cmd = 0;
while (shell.cur_cmd < num_cmd)
{
// Split the input into an array of arguments
            
    split_cmd(shell.cur_cmd);
```

The program then checks for any redirection using check\_redirection function. If there is redirection for multiple files, the program prints an error message and continues to the next command.

```C
check_redirection();
            
if (shell.multipleFiles)
{
    fprintf(stderr, "%s\n", error_message);
    shell.cur_cmd++;
    continue;
}
```

The program handles the "cd" command separately using execute\_cd function. It also handles the "path" command separately using execute\_path function. If the "path" command has no arguments, it sets the notAllowed flag to 1. Otherwise, it adds the specified directory to the search path.

```C
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
```

If the command is not a built-in command, the code checks if it is allowed or not by checking the "notAllowed" flag. If it is not allowed, an error message is printed, and the program returns. If the command is allowed, a child process is forked to execute the command.

The child process starts by searching for the full path of the command to be executed. It does so by parsing the PATH environment variable and appending the command name to each path entry until it finds an executable file. If it finds an executable file, it checks if the output of the command needs to be redirected to a file. If so, it opens the output file and redirects stdout to the file descriptor of the opened file. Finally, it calls the execv system call to replace the current process image with the executable file image, passing the arguments to the command as the argument vector.

If the child process cannot find an executable file, it prints an error message and exits. If the fork() call returns an error, it also prints an error message. The parent process waits for the child process to finish executing before it returns to the command prompt.

```C
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
```

The parent process waits for the child process to finish using wait function. If there is an error during forking, the program prints an error message.

```C
while (wait(NULL) > 0);
```

If the user enters the "exit" command, the program sets the breakLoop flag to 1 and exits the loop.

```C
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
```

Finally, the program frees the allocated memory and returns 0.

```C
free(pathCopy);
return 0;
```
