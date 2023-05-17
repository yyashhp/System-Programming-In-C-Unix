#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void sigusr1Handler(int signo)
 {
  printf("SIGUSR1 signal received.\n"); //Prints upon reception of a SIGUSR 
 }

void sigkillHandler(int signo)
{
printf("SIGKILL signal received.\n");
exit(EXIT_FAILURE); //Exits upon reception of SIGKILL
}

int main()
{
 signal(SIGUSR1, sigusr1Handler);
 signal(SIGKILL, sigkillHandler);
    
 signal(SIGINT, SIG_IGN);

while (1) {
    pause(); //Pauses while running
}

return (EXIT_SUCCESS);

}

