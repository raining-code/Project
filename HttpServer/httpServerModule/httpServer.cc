#include "httpServer.h"
void HttpServer::Run(uint16_t port, int QueueMax) {
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
void HttpServer::HandleRequest(int fd, TcpServer& server) {
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
