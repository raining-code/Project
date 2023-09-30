#include "httpServer.h"
int main(int argc, char** argv) {
    if (argc != 3) {
        LOG(INFO, "输入要绑定的端口和全连接队列的最大长度");
        exit(0);
    }
    HttpServer::Run();
    return 0;
}