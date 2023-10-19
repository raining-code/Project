#pragma once
#include "log.h"
#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <unordered_map>
#define DEFAULT_PORT 8080
#define DEFAULT_QUEUELEN 50
using std::cout;
using std::endl;
using std::pair;
using std::unordered_map;
class TcpServer {
public:
    TcpServer(uint16_t port = DEFAULT_PORT, int QueueMax = DEFAULT_QUEUELEN);
    int Accept();
    pair<string, uint16_t> SelectClientMsg(int fd);
    bool DeleteClientMsg(int fd);

private:
    int Socket();
    bool PortReuse(); // 设置端口复用
    bool Bind(uint16_t port);
    bool Listen(int QueueMax);

private:
    int listenfd;
    unordered_map<int, pair<string, uint16_t>> clientMsgMap; // first:ip second:port
};
inline TcpServer::TcpServer(uint16_t port, int QueueMax) {
    listenfd = Socket();
    if (listenfd == -1) {
        LOG(FATAL, "创建套接字失败");
        exit(0);
    }
    if (!PortReuse()) {
        LOG(FATAL, "端口复用失败");
        exit(0);
    }
    if (!Bind(port)) {
        LOG(FATAL, "绑定失败");
        exit(0);
    }
    if (!Listen(port)) {
        LOG(FATAL, "监听失败");
        exit(0);
    }
}
inline int TcpServer::Socket() {
    return socket(AF_INET, SOCK_STREAM, 0);
}
inline bool TcpServer::PortReuse() {
    int enable = 1;
    return setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) == 0;
}
inline bool TcpServer::Bind(uint16_t port) {
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    return !bind(listenfd, (const struct sockaddr*)&serverAddr, sizeof(serverAddr));
}
inline bool TcpServer::Listen(int QueueMax) {
    return !listen(listenfd, QueueMax);
}
inline int TcpServer::Accept() {
    struct sockaddr_in clientAddr;
    socklen_t len = sizeof(clientAddr);
    memset(&clientAddr, 0, len);
    int fd = accept(listenfd, (struct sockaddr*)&clientAddr, &len);
    if (~fd) {
        uint16_t port = ntohs(clientAddr.sin_port);
        string ip = inet_ntoa(clientAddr.sin_addr);
        clientMsgMap[fd] = std::make_pair(ip, port);
    }
    return fd;
}
inline pair<string, uint16_t> TcpServer::SelectClientMsg(int fd) {
    if (!clientMsgMap.count(fd)) {
        return {"", 0};
    }
    return clientMsgMap[fd];
}
inline bool TcpServer::DeleteClientMsg(int fd) {
    if (!clientMsgMap.count(fd)) {
        return false;
    }
    clientMsgMap.erase(fd);
    return true;
}
