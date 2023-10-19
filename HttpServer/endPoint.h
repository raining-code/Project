#pragma once
#include "RequestAndResponse.h"
#include "tcpServer.h"
#include "util.h"
#include <algorithm>
#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <sstream>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#define SEP_HEAD ":" // 请求报头kv之间的分隔符
#define SEP_ARGS "?" // uri中路径与参数之间的分隔符
class EndPoint {
public:
    EndPoint(int sock, TcpServer& s);
    void ReadHttpRequest();       // 读取请求
    void HandleRequest();         // 处理HTPP请求
    void ConstructHttpResponse(); // 构建HTPP响应
    void SendHttpResponse();      // 发送HTTP响应

private:
    void ReadRequestLine();    // 读取请求行
    void ReadRequestHead();    // 读取请求报头和空行
    void ParseRequestLine();   // 解析请求行
    void ParseRequestHeader(); // 解析请求报头
    void ReadRequestBody();    // 读取请求正文

public: // 测试
    HttpRequest request;
    HttpResponse response;
    int fd;
    TcpServer& server;
};