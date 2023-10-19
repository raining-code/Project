#ifndef HTTP_SERVER
#define HTTP_SERVER
#include "endPoint.h"
#include <memory>
#include <thread>
#define DEFAULT_PORT 8080
#define DEFAULT_QUEUELEN 50
class HttpServer {
public:
    static void Run(uint16_t port = DEFAULT_PORT, int QueueMax = DEFAULT_QUEUELEN);

private:
    static void HandleRequest(int fd, TcpServer& server);
};
inline void HttpServer::Run(uint16_t port, int QueueMax) {
    TcpServer server(port, QueueMax);
    while (1) {
        int fd = server.Accept();
        if (fd == -1) {
            LOG(WARNING, "accept失败");
        } else {
            ShowClientMessage(server.SelectClientMsg(fd));
            LOG(INFO, "创建新线程处理客户端的请求");
            std::thread th(&HttpServer::HandleRequest, fd, std::ref(server));
            th.detach();
        }
    }
}
inline void HttpServer::HandleRequest(int fd, TcpServer& server) {
    std::unique_ptr<EndPoint> uk(new EndPoint(fd, server));
    uk->ReadHttpRequest();       // 读取HTTP请求
    uk->HandleRequest();         // 处理HTTP请求
    uk->ConstructHttpResponse(); // 构建HTTP响应
    uk->SendHttpResponse();      // 发送HTTP响应
    close(fd);                   // 短链接
    auto clientMsg = server.SelectClientMsg(fd);
    server.DeleteClientMsg(fd);
    string closemsg = "与客户端[";
    closemsg.append(clientMsg.first).append(":").append(std::to_string(clientMsg.second)).append("]").append("断开连接");
    LOG(INFO, closemsg);
}

#endif