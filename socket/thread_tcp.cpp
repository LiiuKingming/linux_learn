#include <stdlib.h>
#include "tcpsocket.hpp"

void* thr_start(void *arg){
    TcpSocket *newsock = (TcpSocket*)arg;
    while(1){
        std::string buf;
        bool ret = newsock->Recv(buf);
        if(ret == false){
            newsock->Close();
            return NULL;
        }
        std::cout << "client say:" << buf << "\n";
        buf.clear();
        std::cin >> buf;
        newsock->Send(buf);
    }
    newsock->Close();
    delete newsock;
    return NULL;
}

int main(int argc, char *argv[]){
    if(argc != 3){
        std::cout << "./tcp_srv 127.0.0.1\n";
        return -1;
    }
    
    std::string ip = argv[1];
    uint16_t port = atoi(argv[2]);
    TcpSocket lst_sock;
    // 1. 创建套接字
    CHECK_RET(lst_sock.Socket());
    // 2. 绑定地址信息
    CHECK_RET(lst_sock.Bind(ip, port));
    // 3. 开始监听
    CHECK_RET(lst_sock.Listen());
    while(1){
        TcpSocket *newsock = new TcpSocket();
        // 4. 获取已完成新连接
        bool ret = lst_sock.Accept(*newsock);
        if(ret == false){
            delete newsock;
            continue;
        }
        // 创建新线程处理与客户端的通信
        pthread_t tid;
        pthread_create(&tid, NULL, thr_start, (void*)newsock);
        pthread_detach(tid);
    }
    // 7. 关闭套接字
    lst_sock.Close();
    return 0;
}
