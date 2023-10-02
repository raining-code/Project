#include "tcpServer.h"
TcpServer::TcpServer(uint16_t port, int QueueMax) {
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
int TcpServer::Socket() {
    return socket(AF_INET, SOCK_STREAM, 0);
}
bool TcpServer::PortReuse() {
    int enable = 1;
    return setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) == 0;
}
bool TcpServer::Bind(uint16_t port) {
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    return !bind(listenfd, (const struct sockaddr*)&serverAddr, sizeof(serverAddr));
}
bool TcpServer::Listen(int QueueMax) {
    return !listen(listenfd, QueueMax);
}
int TcpServer::Accept() {
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
pair<string, uint16_t> TcpServer::SelectClientMsg(int fd) {
    if (!clientMsgMap.count(fd)) {
        return {"", 0};
    }
    return clientMsgMap[fd];
}
bool TcpServer::DeleteClientMsg(int fd) {
    if (!clientMsgMap.count(fd)) {
        return false;
    }
    clientMsgMap.erase(fd);
    return true;
}
void ShowClientMessage(const pair<string, uint16_t>& clientmsg) {
    string msg = "accept成功,客户端ip和端口:";
    msg.append("[").append(clientmsg.first).append(":").append(std::to_string(clientmsg.second)).append("]");
    LOG(INFO, msg);
}