#include "tcpServer.h"
int main(int argc, char** argv) {
    if (argc != 3) {
        LOG(INFO, "输入要绑定的端口和全连接队列的最大长度");
        exit(0);
    }
    TcpServer server(atoi(argv[1]), atoi(argv[2]));
    while (1) {
        int fd = server.Accept();
        if (fd == -1) {
            LOG(WARNING, "accept失败");
        } else {
            ShowClientMessage(server.SelectClientMsg(fd));
        }
    }

    return 0;
}