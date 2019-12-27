#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

int tickets = 10;
pthread_mutex_t mutex;

void *thr_tout(void *arg){
    while(1){
        //加锁一定是在访问临界资源之前
        //pthread_mutex_lock 阻塞加锁
        //pthread_mutex_trylock 非阻塞加锁
        //pthread_mutex_timedlock 限时等待的阻塞加锁
        pthread_mutex_lock(&mutex);

        if(tickets > 0){
            usleep(1000);
            printf("tout:%p - get a ticket:%d\n", pthread_self(), tickets);
            tickets--;
            //解锁须在临界资源访问完毕后
            pthread_mutex_unlock(&mutex);
        } else {
            //加锁之后再任何有可能退出的地方都要解锁
            printf("tout:%p exit\n", pthread_self());
            pthread_mutex_unlock(&mutex);

            pthread_exit(NULL);
        }
    }
    return NULL;
}

int main(){
    int i = 0, ret;
    pthread_t tid[4];
    //互斥锁的初始化一定要在线程创建之前
    //pthread_utx_init()
    //pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_init(&mutex, NULL);

    for(; i < 4; i++ ){
        ret = pthread_create(&tid[i], NULL, thr_tout, NULL);
        if(ret != 0){
            printf("thread creat error\n");
            return -1;
        }
    }
    for(i = 0; i < 4; i++){
        pthread_join(tid[i], NULL);
    }
    //一定要在完全不使用的时候销毁互斥锁
    pthread_mutex_destroy(&mutex);
    return 0;
}
