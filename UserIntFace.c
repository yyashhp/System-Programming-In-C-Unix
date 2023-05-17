#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

// int file descriptor for read/write 
int pipe_desc;

// ----------------------------------- SIGUSR1 signal handler -----------------------------------
void sigusr1_handler(int sig) {

    // Define input as zero
    int input = 0;

    // Send zero to server
    write(pipe_desc, &input, sizeof(input));
    printf("I: Received a SIGUSR1, stopping loop\n");
    printf("I: Reading from user abandoned\n");
    printf("I: Interface is exiting\n");

    exit(EXIT_SUCCESS);
}
//------------------------------------------ Main --------------------------------------------
int main(int argc, char* argv[]) {

    int input;             // user fib input 
    char* pipe_name;       // server pipe name  

    // Set the handler function for the SIGUSR1 signal
    if (signal(SIGUSR1, sigusr1_handler) == SIG_ERR) {
        perror("SIGUSR1 signal handler");
        exit(EXIT_FAILURE);
    }

    // Check if file is specified
    if (argc < 2) {
        printf("Usage: %s <pipe_name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Open server for writing
    pipe_name = argv[1];
    if ((pipe_desc = open(pipe_name, O_WRONLY)) == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    
    while(1){

        // Get user input request for a fib number
        printf("I: Which Fibonacci number do you want : ");
        scanf("%d", &input);

        // Check for valid input
        if (input < 0) {
            printf("I: Invalid input - please enter a positive integer\n");
            continue;
        }

        // End loop & close interface when 0 is entered
        if (input == 0) {
            printf("I: Interface is exiting\n");
            write(pipe_desc, &input, sizeof(input));
            break;
        }

        // Write user fibonacci request to server through pipe
        write(pipe_desc, &input, sizeof(input));
        fflush(stdout); //ensure pipe content is transferred
    }

    // Close pipe
    close(pipe_desc);

    return(EXIT_SUCCESS);
}
