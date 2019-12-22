#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(){
    int pipefd[2] = {0};
    int ret = pipe(pipefd);
    if(ret < 0){
        perror("pipe error");
        return -1;
    }

    int pid = fork();
    if (pid < 0) {
        perror("fork error");
        return -1;                            
    }else if (pid == 0) {
        //子进程 读数据
        //暂停
        close(pipefd[0]);
        pause(); 
        char buf[1024] = {0};
        int ret = read(pipefd[0], buf, 1023);
        printf("buf:[%s]-[%d]\n",buf ,ret);
    } else {
        //父进程 写数据
        close(pipefd[0]);
        

        char *ptr = "L_KingMing！";
        int total = 0;
        while(1){
            int ret =write(pipefd[1], ptr, strlen(ptr));
            total += ret;
            printf("total:%d\n",total);
            
        }
    }
    return 0;
}
