#include <vector>
#include <pthread.h>
#include <semaphore.h>
#include <iostream>
#define MAX_QUEUE 5
using namespace std;

class RingQueue{
    vector<int> m_array;//数组
    int m_capacity;//数组的节点数量
    int m_posWrite;//写指针当前位置，写入数据后指针向后移动
    int m_posRead;//读指针当前位置，读取数据后指针向后移动
    sem_t m_semSpace;//空闲空间
    sem_t m_semData;//数据
    sem_t m_semLock;//实现安全的锁

public:
    RingQueue(int maxq = MAX_QUEUE)
        :m_capacity(maxq),
        m_array(maxq),
        m_posRead(0),
        m_posWrite(0){
            sem_init(&m_semSpace, 0, maxq);//空间
            sem_init(&m_semData, 0, 0);//数据
            sem_init(&m_semLock, 0, 1);//锁初始化
    }

    ~RingQueue(){
        sem_destroy(&m_semData);
        sem_destroy(&m_semSpace);
        sem_destroy(&m_semLock);
    }

    bool Push(int &data){
        //没有空间则直接阻塞,空闲空间计数-1
        sem_wait(&m_semSpace);
        
        sem_wait(&m_semLock);//直接通过自身计数判断是否阻塞
        
        m_array[m_posWrite] = data;//加锁保护入队操作
        m_posWrite = (m_posWrite + 1) % m_capacity;
        sem_post(&m_semLock);//解锁
       
        sem_post(&m_semData);//数据资源计数+1, 唤醒消费者
        
        return true;
    }

    bool Pop(int *data){
        sem_wait(&m_semData);//通过资源计数判断是否能获取资源

        sem_wait(&m_semLock);
        *data = m_array[m_posRead];
        m_posRead = (m_posRead + 1) % m_capacity;
        sem_post(&m_semLock);

        sem_post(&m_semSpace);
        return  true;
    }
};

#define MAX_THR 4

void *productor(void *arg){
    RingQueue *q = (RingQueue*)arg;
    int i = 0;
    while(1){
        //生产者入队数据
        q->Push(i);
        cout << "productor" << pthread_self() << "---put data:" << i++ << endl;
    }
    return NULL;
}

void *consumer(void *arg){
    RingQueue *q = (RingQueue*)arg;
    while(1){
        //消费者出队数据
        int data;
        q->Pop(&data);
        cout << "consumer" << pthread_self() << "---get data:" << data << endl;
    }
    return NULL;
}
int main() {
    int ret;
    pthread_t ptid[MAX_THR], ctid[MAX_THR];
    RingQueue q;

    for (int i = 0; i < MAX_THR; i++) {
        ret = pthread_create(&ptid[i], NULL, productor, (void*)&q);
        if (ret != 0) {
            std::cout << "thread creat errno" << std::endl;
            return -1;
        }
    }
    for (int i = 0; i < MAX_THR; i++) {
        ret = pthread_create(&ctid[i], NULL, consumer, (void*)&q);
        if (ret != 0) {
            std::cout << "thread creat errno" << std::endl;
            return -1;
        }
    }
    for (int i = 0; i < MAX_THR; i++) {
        pthread_join(ptid[i], NULL);
        pthread_join(ctid[i], NULL);

    }
    return 0;
}
