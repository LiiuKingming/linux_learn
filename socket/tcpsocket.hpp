// 封装一个tcpsocket类, 向外提供简单接口, 能够实现客户端服务端编程流程
// 1. 创建套接字
// 2. 绑定地址信息
// 3. 开始监听 / 发起连接请求
// 4. 获取已完成连接
// 5. 发送数据
// 6. 接受数据
// 7. 关闭套接字

#include <iostream>
#include <string>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define CHECK_RET(q) if((q) == false){return -1;}

class TcpSocket{
    int m_sockfd;

public:
    TcpSocket():
        m_sockfd(-1){}

    ~TcpSocket(){
        Close();
    }

    bool Socket(){
        m_sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(m_sockfd < 0){
            std::cerr << "socket error\n";
            return false;
        }
        return true;
    }

    bool Bind(std:: string &ip, uint16_t port){
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = inet_addr(&ip[0]);
        socklen_t len = sizeof(struct sockaddr_in);
        int ret = bind(m_sockfd, (struct sockaddr*)&addr, len);

        if(ret < 0){
            std::cerr << "bind error\n";
            return false;
        }
        return true;
    }

    bool Listen(int backlog = 5){
        int ret = listen(m_sockfd, backlog);
        if(ret < 0){
            std::cerr << "listen error\n";
            return false;
        }
        return true;
    }

    bool Connect(std::string &ip, uint16_t port){
        int ret;
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = inet_addr(&ip[0]);
        socklen_t len = sizeof(struct sockaddr_in);
        
        ret = connect(m_sockfd, (struct sockaddr*)&addr, len);
        if(ret < 0){
            std::cerr << "connect error\n";
            return false;
        }
        return true;
    }

    void SetFd(int fd){
        m_sockfd = fd;
    }

    bool Accept(TcpSocket &newsock){
        struct sockaddr_in addr;
        socklen_t len = sizeof(struct sockaddr_in);

        // accept是一个阻塞函数;
        // 当已完成连接队列中没有新连接时就会阻塞
        int fd = accept(m_sockfd, (struct sockaddr*)&addr, &len);
        if(fd < 0){
            std::cerr << "accept error\n";
            return  false;
        }
        newsock.SetFd(fd);
        return true;
    }

    bool Send(std:: string &buf){
        int ret = send(m_sockfd, &buf[0], buf.size(), 0);
        if(ret < 0){
            std::cerr << "send error\n";
            return false;
        }
        return true;
    }

    bool Recv(std::string &buf){
        char tmp[4096] = {0};
        
        //recv返回值为0不是为了表示没有数据(没有数据会阻塞)而是为了表示连接已断开
        int ret = recv(m_sockfd, tmp, 4096, 0);
        if(ret < 0){
            std::cerr << "recv error\n";
            return false;
        } else if(ret == 0){
            std::cerr << "peer shutdown\n";
            return false;
        }
        buf.assign(tmp, ret);
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

