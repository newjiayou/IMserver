#ifndef EPOLL_CHAT_SERVER_H
#define EPOLL_CHAT_SERVER_H

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include "ThreadPool.h"

// 客户端连接状态上下文
struct ClientContext {
    int fd;
    std::string ip;
    std::vector<uint8_t> buffer; // 处理粘包的缓冲区
    std::string accountID;
    std::mutex clientMutex;  
    std::mutex sendMutex ; 
};
class EpollChatServer{
public:
    explicit EpollChatServer(uint16_t port);
    ~EpollChatServer();

    bool start();

private:
    uint16_t m_port ;
    int m_listenFd;
    int m_epollFd;
    std::unordered_map<int,  std::shared_ptr<ClientContext>> m_clients; // fd -> ClientContext
    std::unordered_map<std::string, int> m_userMap;   // accountID -> fd
    //多线程
    std::mutex m_mapMutex;
    std::mutex m_sendMutex; // 保护发送操作的原子性
    ThreadPool m_threadPool;
    //内部辅助函数
   void log(const std::string& msg);
    void setNonBlocking(int fd);
    std::string extractJsonValue(const std::string& json, const std::string& key);

    // Epoll 事件驱动
    void run();
    void handleAccept();
    void handleRead(int fd);
    void handleDisconnect(int fd);

    // 业务逻辑与发包机制
    void processPacket(std::shared_ptr<ClientContext> ctx, uint16_t msgType, const std::string& body);
    void sendPacket(int fd, uint16_t type, const std::string& data);

};
#endif // EPOLL_CHAT_SERVER_H