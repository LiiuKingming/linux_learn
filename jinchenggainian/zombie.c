#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(){
    printf("******%d\n", getpid());
    pid_t pid = fork();
    if (pid < 0){
      printf("fork error\n");
      return -1;
    } else if(pid == 0){
      //子进程中的pid返回值为0
      printf("child!\n");
      sleep(5);
      exit(0);
    } else if(pid > 0){
      printf("parent !\n");  
    }
    while (1){
      printf("------%d\n", getpid());
      sleep(1);
    }
    return 0;
}
