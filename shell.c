/*
```Pseudocode
function parent_main()
    register_child_signal(on_child_exit())
    setup_environment()
    shell()


function on_child_exit()
    reap_child_zombie()
    write_to_log_file("Child terminated")


function setup_environment()
    cd(Current_Working_Directory)


function shell()
    do
        parse_input(read_input())
        evaluate_expression():
        switch(input_type):
            case shell_builtin:
                execute_shell_bultin();
            case executable_or_error:
                execute_command():

    while command_is_not_exit


function execute_shell_bultin()
    swirch(command_type):
        case cd:
        case echo:
        case export:


function execute_command()
    child_id = fork()
    if child:
        execvp(command parsed)
        print("Error)
        exit()
    else if parent and foreground:
        waitpid(child)
```

*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>

#define MAX_INPUT_SIZE 1024
#define MAX_ARGS 64
#define HASHMAP_SIZE 128

// Hashmap structure
typedef struct {
    char *key;
    char *value;
} HashmapEntry;

typedef struct {
    HashmapEntry entries[HASHMAP_SIZE];
} Hashmap;

// Function prototypes
void parse_input(char *input, char **args);
void execute_command(char **args, int background);
void execute_shell_builtin(char **args, Hashmap *hashmap);
void setup_environment();
void on_child_exit(int sig);
unsigned int hash(const char *key);
void hashmap_insert(Hashmap *hashmap, const char *key, const char *value);
char *hashmap_get(Hashmap *hashmap, const char *key);
void hashmap_remove(Hashmap *hashmap, const char *key);

int main() {
    char input[MAX_INPUT_SIZE];
    char *args[MAX_ARGS];
    int background;

    // Initialize hashmap
    Hashmap hashmap = {0};

    // Set up signal handler for SIGCHLD
    signal(SIGCHLD, on_child_exit);

    // Set up environment
    setup_environment();

    // Main shell loop
    while (1) {
        printf("myshell> ");

        // Read input using fgets
        if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL) {
            break; // Exit on EOF (Ctrl+D)
        }

        // Remove newline character
        input[strcspn(input, "\n")] = 0;

        // Parse input into arguments
        parse_input(input, args);

        // Check for background execution
        background = 0;
        if (args[0] != NULL) {
            int i = 0;
            while (args[i] != NULL) i++;
            if (i > 0 && strcmp(args[i-1], "&") == 0) {
                background = 1;
                args[i-1] = NULL; // Remove the '&' from arguments
            }
        }

        // Execute command
        if (args[0] != NULL) {
            if (strcmp(args[0], "exit") == 0) {
                break; // Exit shell
            } else if (strcmp(args[0], "cd") == 0 || strcmp(args[0], "echo") == 0 || strcmp(args[0], "export") == 0) {
                execute_shell_builtin(args, &hashmap);
            } else {
                execute_command(args, background);
            }
        }
    }

    return 0;
}

// Parse input into arguments
void parse_input(char *input, char **args) {
    int i = 0;
    args[i] = strtok(input, " ");
    while (args[i] != NULL) {
        i++;
        args[i] = strtok(NULL, " ");
    }
}

// Execute external commands
void execute_command(char **args, int background) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        return;
    }

    if (pid == 0) { // Child process
        if (execvp(args[0], args) == -1) {
            perror("Error executing command");
        }
        exit(EXIT_FAILURE);
    } else { // Parent process
        if (!background) {
            waitpid(pid, NULL, 0); // Wait for child to complete if not in background
        }
    }
}

// Execute shell built-in commands
void execute_shell_builtin(char **args, Hashmap *hashmap) {
    if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL || strcmp(args[1], "~") == 0) {
            chdir(getenv("HOME"));
        } else if (strcmp(args[1], "..") == 0) {
            chdir("..");
        } else {
            if (chdir(args[1]) != 0) {
                perror("cd failed");
            }
        }
    } else if (strcmp(args[0], "echo") == 0) {
        int i = 1;
        while (args[i] != NULL) {
            // Check if the argument is a variable (starts with '$')
            if (args[i][0] == '$') {
                char *value = hashmap_get(hashmap, args[i] + 1); // Skip '$'
                if (value != NULL) {
                    printf("%s ", value);
                } else {
                    printf("%s ", args[i]); // Print the original argument if not found
                }
            } else {
                printf("%s ", args[i]);
            }
            i++;
        }
        printf("\n");
    } else if (strcmp(args[0], "export") == 0) {
        if (args[1] != NULL) {
            char *key = strtok(args[1], "=");
            char *value = strtok(NULL, "=");
            if (key != NULL && value != NULL) {
                hashmap_insert(hashmap, key, value);
            } else {
                fprintf(stderr, "Invalid export syntax. Use: export KEY=VALUE\n");
            }
        }
    }
}

// Set up environment
void setup_environment() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        setenv("PWD", cwd, 1);
    }
}

// Handle SIGCHLD signal to reap zombie processes and log termination
void on_child_exit(int sig) {
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        if (WIFEXITED(status)) {
            FILE *log_file = fopen("shell.log", "a");
            if (log_file != NULL) {
                fprintf(log_file, "Child process %d terminated\n", pid);
                fclose(log_file);
            }
        }
    }
}

// Hash function for the hashmap
unsigned int hash(const char *key) {
    unsigned int hash = 0;
    while (*key) {
        hash = (hash << 5) + *key++;
    }
    return hash % HASHMAP_SIZE;
}

// Insert a key-value pair into the hashmap
void hashmap_insert(Hashmap *hashmap, const char *key, const char *value) {
    unsigned int index = hash(key);
    hashmap->entries[index].key = strdup(key);
    hashmap->entries[index].value = strdup(value);
}

// Get the value associated with a key from the hashmap
char *hashmap_get(Hashmap *hashmap, const char *key) {
    unsigned int index = hash(key);
    if (hashmap->entries[index].key != NULL && strcmp(hashmap->entries[index].key, key) == 0) {
        return hashmap->entries[index].value;
    }
    return NULL;
}

// Remove a key-value pair from the hashmap
void hashmap_remove(Hashmap *hashmap, const char *key) {
    unsigned int index = hash(key);
    if (hashmap->entries[index].key != NULL) {
        free(hashmap->entries[index].key);
        free(hashmap->entries[index].value);
        hashmap->entries[index].key = NULL;
        hashmap->entries[index].value = NULL;
    }
}