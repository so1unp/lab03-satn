#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>      // times()
#include <unistd.h> // getpid()
#include <sys/wait.h> // wait()
#include <signal.h> // struct rusage
#include <string.h>


#define NUMB_OF_ARGUMENTS 3



int busywork(void)
{
    struct tms buf;
    for (;;) {
        times(&buf);
    }
}

void sig_handler(int p) {
    //printf("Soy el hijo con PID %d y me acaban de matar. rip\n", getpid());
    //printf("%d: %s\n", p, strsignal(p));
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    //printf("\nChild %d (nice %2d):\t%3li\n", getpid(), getpriority(PRIO_PROCESS,getpid()), usage->ru_stime);
    printf("\nChild %d \t%3li\n", getpid(), usage->ru_stime);
    exit(EXIT_SUCCESS);
}




int main(int argc, char *argv[])
{
    if(argc != NUMB_OF_ARGUMENTS){
        fprintf(stderr,"incorrect number of arguments");
        exit(-1);
    }
    
    int childs = atoi(argv[1]);
    unsigned time = atoi(argv[2]);
    //int priority = atoi(argv[3]);
    
    pid_t hijos[childs];
    
    
    for(int i = 0; i < childs; i++){
        pid_t h;
        if((h = fork()) == 0){
            struct sigaction sigAct;
            sigAct.sa_handler = sig_handler;
            sigemptyset(&sigAct.sa_mask);
            //sigAct.sa_flags = SA_RESTART;
            if (sigaction(SIGTERM, &sigAct, NULL) == -1) {
                perror("sigaction");
                return 1;
            }
            printf("Hijo con PID: %d se va a perder tiempo\n", getpid());
            busywork();
        }
        hijos[i] = h;
    }
    
    printf("Proceso padre se va a dormir\n");
    sleep(time);

    for (int j = 0; j < childs; j++) {
        printf("Hijo numero %d, PID: %d\n", j+1, hijos[j]);
    }
    //mandar señales
    for(int i=0;i<childs;i++){

        //printf("kill proceso %d\n",hijos[i]);
        kill(hijos[i],SIGTERM);
    }
    exit(EXIT_SUCCESS);
}
