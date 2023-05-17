#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>

#define PIPE_NAME "FIBOPIPE"
#define MAX_THREADS 100

// Child (interface) process ID
pid_t child_pid; 

// Set a global counter of the number running threads 0
int running_threads = 0;

// ----------------------------------- SIGXCPU signal handler -----------------------------------
void sigxcpu_handler(int signum){

    // Ignore further SIGXCPU signals to avoid repetition
    signal(SIGXCPU, SIG_IGN);
    printf("\nS: Received a SIGXCPU, ignoring any more\n");
    
    // Send SIGUSR1 signal to child process to close it out 
    kill(child_pid, SIGUSR1);
    printf("S: Received a SIGXCPU, sending SIGUSR1 to interface\n");    
}

// ----------------------------------- CPU time counter function -----------------------------------
void update_time(clock_t start_time, clock_t end_time, unsigned int *microseconds, unsigned int *seconds){
    
    // Local function variables, to add to total time
    double temp_time;
    unsigned int temp_microseconds;
    unsigned int temp_seconds;   

    // Get this CPU time tracked
    temp_time = ((double)(end_time-start_time))/CLOCKS_PER_SEC;
    temp_seconds = (unsigned int) temp_time;
    temp_microseconds = (unsigned int)((temp_time - temp_seconds) * 1e6);

    // Update CPU time to add to total
    *seconds = temp_seconds;
    *microseconds = temp_microseconds;

    // Print CPU time after iteration
    printf("S: Server has used %us %uus", *seconds, *microseconds);
}

// ----------------------------------- Recursive fibonacci function -----------------------------------
long fib(long whichNum) {
    if (whichNum <= 1) {
        return(whichNum);
    } 
    else {
        return(fib(whichNum - 2) + fib(whichNum - 1));
    }
}

// ----------------------------------- Threaded function to handle fib calculations -----------------------------------
void *fib_thread(void *arg) {
    
    // Get fib
    int num = *(int *)arg;
    long fib_n = fib(num);

    // Print fib
    printf("S: Fibonacci %d is %ld\n", num, fib_n);
    fflush(stdout); //clear out operation

    // Decrement running_threads for tracking count
    running_threads--;

    return(NULL);
}
// ----------------------------------- Main -----------------------------------
int main(int argc, char *argv[]) {

    int pipe_read,                   // int for pipe reading input
        num_read,                           // int read from UI from user
        cpu_limit_sec,               // int to read arg[0] for CPU limit track
        child_status;                // int to store exit status of UI for print
    
    struct rlimit cpu_limit;         // CPU limit struct


    struct stat pipe_state;          // stats object to check if pipe is already created
     
    unsigned int microseconds=0, // continually updated microseconds int count
                 seconds=0;      // continually updated seconds int count


    clock_t  start_time,             // start CPU time clock_t
             end_time;               // end CPU time clock_t
    
   

    pthread_t threads[MAX_THREADS];  // threads array store

    pid_t parent_pid = getpid();     // parent pid

    // Get CPU start time using clock()
    
    start_time = clock();

    // Check for valid command line args
    if (argc != 2) {
        printf("Provide CPU Limit, Usage: %s <seconds>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Get CPU limit from input
    cpu_limit_sec = atoi(argv[1]);
    
    // Check for valid CPU limit
    if (cpu_limit_sec <= 0) {
        printf("Invalid CPU limit: %d\n", cpu_limit_sec);
        exit(EXIT_FAILURE);
    }

    // Set CPU limit to specified number of seconds
    printf("S: Setting CPU limit to %ds\n", cpu_limit_sec);
    cpu_limit.rlim_cur = cpu_limit_sec;
    cpu_limit.rlim_max = cpu_limit_sec;
    if (setrlimit(RLIMIT_CPU, &cpu_limit) != 0) {
        perror("setrlimit");
        exit(EXIT_FAILURE);
    }

    // Set the handler function for the SIGUSR1 signal
    if (signal(SIGXCPU, sigxcpu_handler) == SIG_ERR) {
        perror("SIGXCPU signal handler");
        exit(EXIT_FAILURE);
    }

    // Check if the pipe already exists 
    if (stat(PIPE_NAME, &pipe_state) == 0) {
        // Delete the existing pipe and create a new one
        if (unlink(PIPE_NAME) == -1) {
            perror("unlink");
            exit(EXIT_FAILURE);
        }
    }

    // Create the pipe
    if (mkfifo(PIPE_NAME, 0666) == -1) {
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }  

    // Create a child (interface) process
    if((child_pid = fork()) == -1){
        perror("fork");
        exit(EXIT_FAILURE);
    }

    // If child process comes up well
    if (child_pid == 0) {

      // Replace the child process with the receiver program
      
      execlp("./UserIntFace", "UserIntFace", PIPE_NAME, NULL);
        
      // This line will be executed only if execlp fails
        
      perror("execlp");
        
      exit(EXIT_FAILURE); 
    } 
    
    // If parent process, then begin
    
    else {
        
      // Open UI for reading
        
      if ((pipe_read = open(PIPE_NAME, O_RDONLY)) < 0) {
        perror("Open pipe");
        exit(EXIT_FAILURE);
        }

        // Repeatedly:
        while (1) {
            
          // Read integer off the pipe
            
          if (read(pipe_read, &num_read, sizeof(num_read)) > 0) {

          // Repeat until 0 is passed
            if (num_read == 0) {
             break;
           }

          // Get this iteration's CPU time
           
            end_time = clock();
          
          //Update CPU time
           
          update_time(start_time, end_time, &microseconds, &seconds);
                
          // Handle max CPU time infraction
           
          if(seconds >= cpu_limit_sec){

            // Send SIGXCPU signal to current (parent) process
            
            if(kill(parent_pid, SIGXCPU) < 0){
              perror("kill");
              exit(EXIT_FAILURE);
            }
            break;
          }

          // Create new thread & calculate the n'th fib number
          printf("\nS: Received %d from interface\n", num_read);
          if (pthread_create(&threads[running_threads], NULL, fib_thread, &num_read) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
              }
                
          // Increment number of running threads
          running_threads++;
                
          // Detach thread
          if (pthread_detach(threads[running_threads-1]) != 0) {
            perror("pthread_detach");
            exit(EXIT_FAILURE);
          }
          printf("S: Created and detached the thread for %d\n", num_read);          
        }    
      }

        // Close pipe
        close(pipe_read);

        // Handle remaining threads
        while(running_threads > 0) {
            printf("S: Waiting for %d threads\n", running_threads);   
            sleep(5);         
        }

        // Get final CPU time 
        end_time = clock();
        
        // Update CPU time accordingly
        update_time(start_time, end_time, &microseconds, &seconds);
                
        // Clean up the child zombie process (User Interface)
        waitpid(child_pid, &child_status, 0);
        printf("\nS: Child %d completed with status %d\n", child_pid, child_status);
        
        // Delete named pipe for reuse purposes
        if (unlink(PIPE_NAME) == -1) {
            perror("unlink");
            exit(EXIT_FAILURE);
        }
       
    }
    return(EXIT_SUCCESS);
}
