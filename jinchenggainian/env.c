#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[], char *env[]){
    int i;
    extern char **environ;
    for(i = 0; environ[i] != NULL; i++){
        printf("env[%d]=[%s]\n", i, environ[i]);
        
    }
    return 0;
}
