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
using std::cout;
using std::endl;
using std::pair;
using std::unordered_map;
class TcpServer {
public:
    TcpServer(uint16_t port, int QueueMax);
    int Accept();
    pair<string, uint16_t> SelectClientMsg(int fd);
    bool DeleteClientMsg(int fd);

private:
    int Socket();
    bool Bind(uint16_t port);
    bool Listen(int QueueMax);

private:
    int listenfd;
    unordered_map<int, pair<string, uint16_t>> clientMsgMap; // first:ip second:port
};
void ShowClientMessage(const pair<string, uint16_t>& msg);