#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
//the #includes and #define
#define MAX_COMMAND_LEN 100

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Must Follow Command with: %s <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *filename = argv[1];
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "Failed to open file %s\n", filename);
        exit(EXIT_FAILURE);
    }

    char command[MAX_COMMAND_LEN];
    while (fgets(command, MAX_COMMAND_LEN, fp)) {
        // Remove newline character from the end of the command
        size_t len = strlen(command);
        if (command[len - 1] == '\n') {
            command[len - 1] = '\0';
        }

        // Fork a child process to run the command in background mode
        pid_t pid = fork();
        if (pid < 0) {
            fprintf(stderr, "Failed to fork a child process\n");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Child process check
            char *args[MAX_COMMAND_LEN];
            int argc = 0;
            char *token = strtok(command, " ");
            while (token != NULL) {
                args[argc++] = token;
                token = strtok(NULL, " ");
            }
            args[argc] = NULL; //clear out
            execvp(args[0], args); //waiting...
            fprintf(stderr, "Failed to execute command %s\n", command);
            return(EXIT_SUCCESS);
        }
    }

    // Wait for all child processes to finish
    int status;
    pid_t child_pid;
    while ((child_pid = wait(&status)) != -1) {
        if (WIFEXITED(status)) {
            printf("Child process %d exited with status %d\n", child_pid, WEXITSTATUS(status));
        } else {
            printf("Child process %d terminated abnormally\n", child_pid);
        }
    }

    fclose(fp);
    return 0;
}

