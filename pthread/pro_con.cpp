#include <iostream>
#include <pthread.h>
#include <queue>

#define MAX_QUEUE 5
using namespace std;

class BlockQueue{
    queue<int> m_q;
    int m_capacity;//限制队列中节点的最大数量
    pthread_mutex_t m_mutex;//实现互斥
    pthread_cond_t m_condPro;//生产者队列
    pthread_cond_t m_condCon;//消费者队列
public:
    BlockQueue(int qmax = MAX_QUEUE)
        :m_capacity(qmax){
        /*初始化资源*/
        pthread_mutex_init(&m_mutex, NULL);
        pthread_cond_init(&m_condPro, NULL);
        pthread_cond_init(&m_condCon, NULL);
    }
    
    ~BlockQueue(){
        /*释放资源*/
        pthread_mutex_destroy(&m_mutex);
        pthread_cond_destroy(&m_condPro);
        pthread_cond_destroy(&m_condCon);
    }

    bool Push(int &data){
        /*入队*/
        pthread_mutex_lock(&m_mutex);
        while(m_q.size() == m_capacity){
            pthread_cond_wait(&m_condPro, &m_mutex);
        }
        m_q.push(data);
        pthread_mutex_unlock(&m_mutex);

        pthread_cond_signal(&m_condCon);
        return true;
    }

    bool Pop(int *data){
        /*出队*/
        pthread_mutex_lock(&m_mutex);
        while(m_q.empty()){
            pthread_cond_wait(&m_condCon, &m_mutex);
        }
        *data = m_q.front();
        m_q.pop();
        pthread_mutex_unlock(&m_mutex);
        pthread_cond_signal(&m_condPro);
        return true;
    }
};

void *productor(void *arg){
    BlockQueue *q = (BlockQueue*)arg;
    int i = 0;
    while(1){
        //生产者不断获取数据
        q->Push(i);
        cout << "productor:" << pthread_self() << "---put data:" << i++ << endl;
    }
    return NULL;
}

void *consumer(void *arg){
    BlockQueue *q = (BlockQueue*)arg;
    while(1){
        //消费者不断获取数据
        int data;
        q->Pop(&data);
        cout << "consumer:" << pthread_self() << "---get data:" << data << endl;
    }
    return NULL;
}

#define MAX_THR 4
int main(){
    pthread_t ptid[MAX_THR], ctid[MAX_THR];
    int ret = 0;
    BlockQueue q;

    for(int i = 0; i < MAX_THR; i++){
        ret = pthread_create(&ptid[i], NULL, productor, (void*)&q);
        if(ret != 0){
            cout << "pthread creat error" << endl;
            return -1;
        }
    }

    for(int i = 0; i < MAX_THR; i++){
        ret = pthread_create(&ctid[i], NULL, consumer, (void*)&q);
        if(ret != 0){
            cout << "pthread creat error" << endl;
            return -1;
        }
    }

    for(int i = 0; i < MAX_THR; i++){
        pthread_join(ctid[i], NULL);
        pthread_join(ptid[i], NULL);
    }
    return 0;
}




