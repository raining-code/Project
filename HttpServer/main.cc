#include "endPoint.h"
#include "tcpServer.h"
#include "util.h"
#include <algorithm>
#include <fcntl.h>
#include <fstream>
int main() {
    TcpServer server;
    int fd = server.Accept();
    EndPoint e(fd, server);
    e.ReadHttpRequest();
    // cout << e.request.requestline << endl;
    // std::for_each(e.request.heads.begin(), e.request.heads.end(), [](const string& s) {
    //     cout << s << endl;
    // });
    // cout << e.request.body << endl;
    // cout << e.request.length << endl;
    cout << e.request.method << endl;
    cout << e.request.version << endl;
    cout << e.request.uri << endl;
    return 0;
}