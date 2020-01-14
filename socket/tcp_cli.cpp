#include <stdlib.h>
#include "tcpsocket.hpp"

int main(int argc, char *argv[]){
    if(argc != 3){
        std::cout << "./tcp_cli srvip srvport\n";
        return -1;
    }

    std::string ip = argv[1];
    u_int16_t port = atoi(argv[2]);
    TcpSocket sock;
    
    // 1. 创建套接字
    CHECK_RET(sock.Socket());
    // 2. 绑定地址信息(客户端不需要主动绑定)
    // 3. 向服务端发起连接请求
    CHECK_RET(sock.Connect(ip, port));
    while(1){
        // 4. 发送数据
        std::string buf;
        std::cin >> buf;
        bool ret = sock.Send(buf);
        if(ret == false){
            sock.Close();
            return -1;
        }

        //5. 接收数据
        buf.clear();
        ret = sock.Recv(buf);
        if(ret == false){
            sock.Close();
            return -1;
        }
        std::cout << "srver say :" << buf << "\n";
    }

    // 6. 关闭套接字
    sock.Close();
    return 0;
}

