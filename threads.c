#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>


#define MAX_LEN 128

typedef char String[MAX_LEN];

// Global variable for communication

String global_string;

// Producer thread function
void *producer_thread(void *arg) {
    
	while (1) {
        // Yield the CPU until global string is empty
        while (strlen(global_string) > 0) {
            sched_yield();
        }
        // Prompt the user for a string entry
        printf("Producer Thread. Enter a string: ");
        fgets(global_string, MAX_LEN, stdin);
        
		// End the program when user enters EOF as response (^D)
        if (feof(stdin)) {
			printf("\n");
            return(EXIT_SUCCESS);
        }

		//printf("In producer\n");
    }
}

// Consumer thread function
void *consumer_thread(void *arg) {
    while (1) {
        // Yield the CPU until global string is non-empty
        while (strlen(global_string) == 0) {
            sched_yield();
        }
        // Print the global string to the screen
        if (strlen(global_string) !=0){

			printf("Consumer Thread: %s", global_string);

			// Set the global string to empty
        	memset(global_string, 0, sizeof(global_string));
		};

		//printf("In consumer\n");
    }
}

// Main program
int main() {
    // Set the common global string to empty
    memset(global_string, 0, sizeof(global_string));

    // Start the communicating threads
    pthread_t p_thread, c_thread;
    
	if (pthread_create(&p_thread, NULL, producer_thread, NULL) != 0){
		perror("pthread_create was successful for the producer thread");
		exit(EXIT_FAILURE);
	}

    if (pthread_create(&c_thread, NULL, consumer_thread, NULL) != 0){
		perror("pthread_create was successful for the consumer thread");
		exit(EXIT_FAILURE);
	}

    // Detach the threads
    if (pthread_detach(p_thread) != 0){
		perror("pthread_detach was successful for the producer");
		exit(EXIT_FAILURE);
	}

    if (pthread_detach(c_thread) != 0){
		perror("pthread_detach was successful for the consumer");
		exit(EXIT_FAILURE);
	}

    // Exit the main program
    pthread_exit(NULL);
}

