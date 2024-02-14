#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

void executeCommand(char *args[]) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("Error forking process.");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process
        execvp(args[0], args);

        // If execvp fails
        perror("Error executing command");
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
    }
}

int main() {
    char input[256];

    while (1) {
        // Display prompt
        printf("SimpleShell> ");

        // Get user input
        if (fgets(input, sizeof(input), stdin) == NULL) {
            perror("Error reading input");
            exit(EXIT_FAILURE);
        }

        // Remove newline character from input
        size_t input_length = strlen(input);
        if (input_length > 0 && input[input_length - 1] == '\n') {
            input[input_length - 1] = '\0';
        }

        // Exit if the user enters 'quit'
        if (strcmp(input, "quit") == 0) {
            break;
        }

        // Tokenize the input
        char *token;
        char *args[64];
        int i = 0;

        token = strtok(input, " ");
        while (token != NULL) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL; // Null-terminate the array

        // Execute the command
        if (i > 0) {
            executeCommand(args);
        }
    }

    return 0;
}
