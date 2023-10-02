#pragma once
#include "../socketModule/tcpServer.h"
#include "./util.h"
#include <algorithm>
#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <sstream>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <unordered_map>
#include <vector>
#define OK 200
#define SEP_HEAD ":" // 请求报头kv之间的分隔符
#define SEP_ARGS "?" // uri中路径与参数之间的分隔符
using std::string;
using std::unordered_map;
using std::vector;
struct HttpRequest {
    string requestline;   // 请求行
    vector<string> heads; // 请求报头
    string blank;         // 空行
    string body;          // 请求正文
    /*解析的结果*/
    string method;                        // 请求方法
    string uri;                           // 网址
    string version;                       // http版本
    unordered_map<string, string> headkv; // 请求报头中的K-V
    int length = 0;                       // 正文长度
    string path;                          // 请求的资源路径
    string uriArgs;                       // uri中携带的参数,使用get方法请求时存在该参数
    bool usecgi = false;                  // 是否使用CGI(common gateway interface,公共网关接口)
};
struct HttpResponse {
    string statusline; // 状态行
    string heads;      // 响应报头
    string blank;      // 空行
    string body;       // 响应正文
    /*解析结果*/
    int statusCode = OK; // 状态码
    int fd = -1;
    int size = 0;  // 响应资源的大小
    string suffix; // 响应文件的后缀
};
class EndPoint {
public:
    EndPoint(int sock, TcpServer& s);
    void ReadHttpRequest(); // 读取请求
    void HandleRequest();
    void ConstructHttpResponse();
    void SendHttpResponse();

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