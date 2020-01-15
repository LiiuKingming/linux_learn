#include <stdlib.h>
#include "tcpsocket.hpp"

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
    TcpSocket newsock;
    while(1){
        // 4. 获取已完成新连接
        bool ret = lst_sock.Accept(newsock);
        if(ret == false){
            continue;
        }
        // 5. 接收数据
        std:: string buf;
        ret = newsock.Recv(buf);
        if(ret == false){
            newsock.Close();
            continue;
        }
        
        std:: cout << "client say:" << buf << "\n";
        // 6. 发送数据
        buf.clear();
        std::cin >> buf;
        newsock.Send(buf);
    }

    lst_sock.Close();
    return 0;
}
