#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

int alarmsHandled = 0;
//Tracks alarms handled, up to 3
void sigalrmHandler(int signo)
{
alarmsHandled++;
printf("Number of SIGALRMs handled: %d\n", alarmsHandled);
alarm(5);
}
//Resets timer to 5 when receieving SIGALRM
void sigintHandler(int signo)
{
printf("SIGINT signal received.\n");
alarm(5);
}//Sets alarm to 5 when receiving SIGINT

int main()
{
signal(SIGALRM, sigalrmHandler);
signal(SIGINT, sigintHandler);

    pid_t childPid;

    childPid = fork();
    if (childPid == 0) {
        execl("receiver", "receiver", NULL);
    } //forks the child process
    else {
        alarm(5);

        while (alarmsHandled < 3) {
            pause();
        }
//Ends after there are 3  SigInts sent in
        kill(childPid, SIGUSR1);
        kill(childPid, SIGKILL);
        waitpid(childPid, NULL, 0);

        printf("Sender process terminated.\n");
    }

    return(EXIT_SUCCESS);

}

