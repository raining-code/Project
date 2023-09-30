#ifndef HTTP_SERVER
#define HTTP_SERVER
#include "../socketModule/tcpServer.h"
#include <memory>
#include <thread>
#include <unordered_map>
#include <vector>
#define DEFAULT_PORT 8080
#define DEFAULT_QUEUELEN 50
#define OK 200
using std::unordered_map;
using std::vector;
class HttpServer {
public:
    static void Run(uint16_t port = DEFAULT_PORT, int QueueMax = DEFAULT_QUEUELEN);

private:
    static void HandleRequest(int fd, TcpServer& server);
};
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
    int size = 0;
    string suffix; // 响应文件的后缀
};
class EndPoint {
public:
    EndPoint(int sock);
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

private:
    HttpRequest request;
    HttpResponse response;
    int fd;
};
string ReadOneLine(int fd);
#endif