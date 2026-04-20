#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>      // times()
#include <unistd.h> // getpid()
#include <sys/wait.h> // wait()
#include <signal.h> // struct rusage

#define NUMB_OF_ARGUMENTS 4
int busywork(void)
{
    struct tms buf;
    for (;;) {
        times(&buf);
    }
}

void handle_sigint(int sig) {
    struct rusage *usage;
    pid_t pid = getpid();
    getrusage(RUSAGE_SELF,usage);
    
    printf("\nChild %d (nice %2d):\t%3li\n", pid, getpriority(PRIO_PROCESS,pid), usage->ru_stime);
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    if(argc != NUMB_OF_ARGUMENTS){
        fprintf(stderr,"incorrect number of arguments");
        exit(-1);
    }
    
    int childs = atoi(argv[1]);
    int time = atoi(argv[2]);
    int priority = atoi(argv[3]);
    
    pid_t hijos[childs];
    pid_t parentPid = getpid();
    
    
    for(int i = 0; i < childs; i++){
        pid_t h;
        if((h = fork()) == 0){
            struct sigaction sa;
            sa.sa_handler = &handle_sigint;
            sigemptyset(&sa.sa_mask);
            sa.sa_flags = SA_RESTART;
            if (sigaction(SIGINT, &sa, NULL) == -1) {
                perror("sigaction");
                return 1;
            }
            busywork();
        }
        hijos[i] = h;
    }
    
    
    sleep(time);

    //mandar señales
    for(int i=0;i<childs;i++){
        printf("kill proceso %d\n",hijos[i]);
        kill(hijos[i],SIGKILL);
    }
    exit(EXIT_SUCCESS);
}
