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
    std::unique_ptr<EndPoint> uk(new EndPoint(fd));
    uk->ReadHttpRequest();
    uk->HandleRequest();
    uk->ConstructHttpResponse();
    uk->SendHttpResponse();
    close(fd); // 短链接
    auto clientMsg = server.SelectClientMsg(fd);
    server.DeleteClientMsg(fd);
    string closemsg = "与客户端[";
    closemsg.append(clientMsg.first).append(":").append(std::to_string(clientMsg.second)).append("]").append("断开连接");
    LOG(INFO, closemsg);
}
string ReadOneLine(int fd) {
    // 读取一行,报文可能以/r作为分隔,也可能以/n作为分隔,也可能以/r/n作为分隔
}
EndPoint::EndPoint(int sock) {
    fd = sock;
}
void EndPoint::ReadHttpRequest() {
    ReadRequestLine();
    ReadRequestHead();
    ParseRequestLine();
    ParseRequestHeader();
    ReadRequestBody();
}
void EndPoint::HandleRequest() {
}
void EndPoint::ConstructHttpResponse() {
}
void EndPoint::SendHttpResponse() {
}

void EndPoint::ReadRequestLine() {
    request.requestline = ReadOneLine(fd);
}
void EndPoint::ReadRequestHead() {
}
void EndPoint::ParseRequestLine() {
}
void EndPoint::ParseRequestHeader() {
}
void EndPoint::ReadRequestBody() {
}