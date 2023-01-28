#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <signal.h>
#include <sys/types.h>

#define MAXARGS 10
#define MAXPRCS 100

int pid_index;
pid_t removed;
int num_pids;
pid_t pids[MAXPRCS];

void alarm_handler(int signum){
    
    kill(removed, SIGSTOP);
    pids[pid_index] = removed;
    num_pids+=1;
    
   
    
}
void cont_handler(int signum){
    
}

void main(int argc, char *argv[]){
    sigset_t block_mask;
    sigemptyset(&block_mask);
    sigaddset(&block_mask, SIGCHLD);
    fflush(stdout);
    int status;

    struct sigaction alrmhandler;
    sigemptyset(&alrmhandler.sa_mask);
    alrmhandler.sa_handler = alarm_handler;
    alrmhandler.sa_mask = block_mask;
    alrmhandler.sa_flags = SA_RESTART;
    sigaction(SIGALRM, &alrmhandler, NULL);

    struct sigaction conthandler;
    conthandler.sa_handler = cont_handler;
    conthandler.sa_flags = SA_RESTART;
    sigaction(SIGCONT, &conthandler, NULL);

    char *ptr;
    int quantum = strtol(argv[1],&ptr, 10)*1000;
    int secs = quantum/1000000;
    int usecs = quantum%1000000;

    char prog[100];
    strcpy(prog, "./");
    strcpy((prog+2),argv[2]);

    char *args[MAXARGS];
    for (int i = 0;i<MAXARGS;i++)
    {
        args[i] = (char*)malloc(100);
    }
    int argcounter = 0;
    int pidcounter = 0;

    for (int i = 2; i<argc ; i++)
    {
        if (!strcmp(":", argv[i]))
        {
            args[argcounter] = NULL;
            argcounter = 0;
            pid_t childpid = fork();
            pids[pidcounter] = childpid;
            if (childpid == 0){
                int ret = pause();
                if (ret == -1){
                    execv(prog,args);
                }
            }
            pidcounter+=1;

        }
        else{
            strcpy(args[argcounter],argv[i]);
            argcounter +=1;
        }
    }
    args[argcounter] = NULL;
    pid_t childpid = fork();
    pids[pidcounter] = childpid;                                                                                                                                                                                                           
        if (childpid == 0){
            int ret = pause();
                if (ret == -1){
                    execv(prog,args);
                }
            
        }
    
    sleep(1);
    pid_index = 0;
    num_pids = pidcounter +1;
    struct itimerval timer;
    timer.it_value.tv_sec = secs;
    timer.it_interval.tv_sec = 0;
    timer.it_value.tv_usec = usecs;
    timer.it_interval.tv_usec = 0;


    struct itimerval cancel;
    cancel.it_value.tv_sec = 0;
    cancel.it_interval.tv_sec = 0;
    cancel.it_value.tv_usec = 0;
    cancel.it_interval.tv_usec = 0;

    

    
    while (num_pids>0)
    { 
        while(pids[pid_index] == -1){
            pid_index = (pid_index+1)%(pidcounter+1);
        }
        
        removed = pids[pid_index];
        pids[pid_index] = -1; 
        setitimer(ITIMER_REAL, &timer, NULL);
        kill(removed,SIGCONT);
        waitpid(removed, &status, WUNTRACED);
        num_pids-=1;
        pid_index = (pid_index+1)%(pidcounter+1);
        setitimer(ITIMER_REAL,&cancel, NULL);
    }
    for (int i = 0; i<MAXARGS;i++){
        free(args[i]);
    }
    
    return;
    
    
}