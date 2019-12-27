#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#define TID_MAX 4

pthread_cond_t condCustomer;
pthread_cond_t condCook;
pthread_mutex_t mutex;
int food = 0;

void *Customer(void *arg){
    while(1){
        pthread_mutex_lock(&mutex);
        while(food == 0){
            //没有食物则需要等待,但是在等待前要先解锁
            //并且这个解锁操作和陷入休眠的操作必须是原子操作
            //解锁->休眠->被唤醒之后需要重新加锁
            pthread_cond_wait(&condCustomer, &mutex);
        }
        printf("Really delicious\n");
        food--;
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&condCook);
    }
    return NULL;
}

void *Cook(void *arg){
    while(1){
        pthread_mutex_lock(&mutex);
        while(food == 1){
            //有食物但是没人吃,陷入等待
            pthread_cond_wait(&condCook, &mutex);
        }
        printf("Is dinner ready!\n");
        food++;
        pthread_mutex_unlock(&mutex);
        //食物做好了唤醒等待的人
        pthread_cond_signal(&condCustomer);
    }
    return NULL;
}


int main(){
    pthread_t customer_tid, cook_tid;
    int ret;

    //初始化条件变量
    pthread_cond_init(&condCook, NULL);
    pthread_cond_init(&condCustomer, NULL);
    pthread_mutex_init(&mutex, NULL);
    int i;
    for(i = 0; i < TID_MAX; i++){
        ret = pthread_create(&cook_tid, NULL, Cook, NULL);
        if(ret != 0){
            printf("pthread creat error\n");
            return -1;
        }
    }

    for(i = 0; i < TID_MAX; i++){
        ret = pthread_create(&customer_tid, NULL, Customer, NULL);
        if(ret != 0){
            printf("pthread creat error\n");
            return -1;
        }
    }
       
    pthread_join(cook_tid, NULL);
    pthread_join(customer_tid, NULL);
    pthread_cond_destroy(&condCook);
    pthread_cond_destroy(&condCustomer);
    pthread_mutex_destroy(&mutex);
    return 0;
}
