#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>


int main(){
    while(1){
        //得到用户的标准输入
        printf("[username@localhost]$ ");
        fflush(stdout);
        char cmd_buf[1024] = {0};
        fgets(cmd_buf, 1023 ,stdin);//从标准输入读取数据
        cmd_buf[strlen(cmd_buf) - 1] = '\0';//字符串末尾赋0
        printf("cmd:[%s]\n", cmd_buf);
    
        //字符串解析 得到命令名称+运行参数 
        char *argv[32] = {NULL};
        int argc = 0;
        char *ptr = cmd_buf;
        while (*ptr != '\0'){
            if(!isspace(*ptr)){//不为空白输入
                argv[argc] = ptr;
                argc++;
                while(!isspace(*ptr) && *ptr != '\0')
                    ptr++;
                *ptr = '\0';
            }
            ptr++;
        }
        argv[argc] = NULL;
    
        //创建子进程
        pid_t pid = fork();
        if(pid < 0){
            continue;
        }else if(pid == 0){
            //在子进程中完成程序替换功能
            execvp(argv[0], argv);

            //因为子进程和父进程带代码一样, 若程序替换失败就会出现两个shell
            //而一个程序不需要两个shell, 所以替换失败时, 子程序直接退出
            exit(0);
        }
        //进程等待
        wait(NULL);
    }
    return 0;
}



