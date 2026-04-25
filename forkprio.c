#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <sys/resource.h>

#define NUMB_OF_ARGUMENTS 4

int busywork(void) {
    struct tms buf;
    for (;;) {
        times(&buf);
    }
}

void sig_handler() {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    printf("Child %d (nice %2d):\t%1li seconds\n", (id_t)getpid(), getpriority(PRIO_PROCESS, (id_t) getpid()), usage.ru_stime.tv_sec);
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
    if(argc != NUMB_OF_ARGUMENTS){
        fprintf(stderr,"Numero incorrecto de argumentos\n");
        exit(-1);
    }
    
    int amountChilds = atoi(argv[1]);
    unsigned time = (unsigned) atoi(argv[2]);
    int priority = atoi(argv[3]);
    
    pid_t children[amountChilds];
    
    for(int i = 0; i < amountChilds; i++){
        pid_t childPid;
        if((childPid = fork()) == 0){
            struct sigaction sigAct;
            sigAct.sa_handler = sig_handler;
            sigemptyset(&sigAct.sa_mask);
            if (sigaction(SIGTERM, &sigAct, NULL) == -1) {
                perror("sigaction");
                return 1;
            }

            busywork();
        }
        if (priority == 1) {
            setpriority(PRIO_PROCESS, (id_t)childPid, i+1);
        }
        children[i] = childPid;
    }
    
    printf("Proceso padre se va a dormir\n");
    if (time == 0) {
        busywork();
    }   else {
        sleep(time);
    }

    for(int i = 0;i < amountChilds; i++){
        kill(children[i],SIGTERM);
    }
    exit(EXIT_SUCCESS);
}
