#include "endPoint.h"
EndPoint::EndPoint(int sock, TcpServer& s)
: server(s) {
    fd = sock;
}
void EndPoint::ReadHttpRequest() {
    ReadRequestLine();    // 读取请求行
    ReadRequestHead();    // 读取请求报头
    ParseRequestLine();   // 解析请求行
    ParseRequestHeader(); // 解析请求报头
    ReadRequestBody();    // 读取请求正文
}
void EndPoint::HandleRequest() { // 对请求进行处理
    if (request.method != "GET" && request.method != "POST") {
        return;
    }
    auto [path, args] = SplitStr(request.uri, SEP_ARGS);
    if (!(request.method == "GET" && args.empty())) { // GET方法并且uri没有参数,直接把请求的资源返回即可
        request.usecgi = true;
        request.path = path;
        request.uriArgs = args;
    }
}
void EndPoint::ConstructHttpResponse() {
}
void EndPoint::SendHttpResponse() {
}

void EndPoint::ReadRequestLine() {
    request.requestline = ReadOneLine(fd);
    request.requestline.pop_back(); // 去掉末尾的\n
}
void EndPoint::ReadRequestHead() {
    string kvstr;
    while (1) {
        kvstr = ReadOneLine(fd);
        kvstr.pop_back();
        if (kvstr.empty()) {
            break;
        }
        request.heads.push_back(move(kvstr));
    }
}
void EndPoint::ParseRequestLine() {
    // 方法 URI 版本
    std::stringstream ss;
    ss << request.requestline; // 使用stringstream对请求行进行解析
    ss >> request.method >> request.uri >> request.version;
    std::transform(request.method.begin(), request.method.end(), request.method.begin(), toupper); // 将方法转化为大写
}
void EndPoint::ParseRequestHeader() {
    auto& heads = request.heads;
    for (const auto& kvstr : heads) {
        auto [k, v] = SplitStr(kvstr, SEP_HEAD);
        request.headkv[k] = v;
    }
}
void EndPoint::ReadRequestBody() {
    if (!request.headkv.count("Content-Length")) {
        return;
    }
    size_t len = std::stoll(request.headkv["Content-Length"]); // 请求正文的长度
    request.length = len;
    while (len--) {
        char c;
        recv(fd, &c, 1, 0);
        request.body += c;
    }
}