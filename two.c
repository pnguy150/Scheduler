#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void main(int argc, char *argv[]){
    char *ptr;
    int repeat = strtol(argv[1],&ptr, 10);
    for(int i = 0; i <repeat;i++){
        printf("%d\n", repeat);
        sleep(1);
    }
}