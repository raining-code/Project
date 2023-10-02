#pragma once
#include "../logModule/log.h"
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
void ShowClientMessage(const pair<string, uint16_t>& msg);