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
#endif