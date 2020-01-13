// 封装实现一个socket类;向外提供更加容易使用的udp接口来实现udp通信流程
#include <iostream>
#include <string>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

using namespace std;

class UdpSocket{
    int m_sockfd;
public:
    UdpSocket():m_sockfd(-1){}

    ~UdpSocket(){
        Close();
    }

    bool Socket(){
        m_sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if(m_sockfd < 0){
            std::cerr << "socket error\n";
            return false;
        }
        return true;
    }

    bool Bind(const string &ip, uint16_t port){
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = inet_addr(ip.c_str());
        socklen_t len = sizeof(struct sockaddr_in);
        int ret = bind(m_sockfd, (struct sockaddr*)&addr, len);

        if(ret < 0){
            std::cerr << "bind error\n";
            return false;
        }
        return true;
    }

    bool Recv(string &buf, string &ip, uint16_t &port){
        char tmp[4096];
        struct sockaddr_in peeraddr;
        socklen_t len = sizeof(peeraddr);

        int ret = recvfrom(m_sockfd, tmp, 4096, 0, (struct sockaddr*)&peeraddr, &len);

        if(ret < 0){
            std::cerr << "recvfrom error\n";
            return false;
        }

        buf.assign(tmp, ret);
        port = ntohs(peeraddr.sin_port);

        // inet_ntoa 将网络字节序的整数IP地址转换为字符串IP地址
        // 返回缓冲区首地址, 内部实现使用静态成员变量
        // 非线程安全
        ip = inet_ntoa(peeraddr.sin_addr);
        return true;
    }
    
    bool Send(string &data, string &ip, uint16_t port){
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = inet_addr(ip.c_str());
        socklen_t len = sizeof(struct sockaddr_in);

        int ret = sendto(m_sockfd, &data[0], data.size(), 0, (struct sockaddr*)&addr, len);

        if(ret < 0){
            std::cerr <<"sento error\n";
            return -1;
        }
        return true;
    }

    bool Close(){
        if(m_sockfd >= 0){
            close(m_sockfd);
            m_sockfd = -1;
        }
        return true;
    }
};

#define CHECK_RET(q) if((q) == false){return -1;}

int main(int argc, char *argv[]){
    if(argc != 3){
        std::cout << "./udp_cli serverip serverport\n";
        return -1;
    }
    string srv_ip = argv[1];
    uint16_t srv_port = atoi(argv[2]);
    UdpSocket sock;

    CHECK_RET(sock.Socket());
    // CHECK_RET(sock.Bind("127.0.0.1", 8000));

    while(1){
        string buf;
        cin >> buf;
        CHECK_RET(sock.Send(buf, srv_ip, srv_port));
        buf.clear();
        CHECK_RET(sock.Recv(buf, srv_ip, srv_port));
        std::cout << "servet say : " << buf << "\n";
    }
    CHECK_RET(sock.Close());

    return 0;
}
