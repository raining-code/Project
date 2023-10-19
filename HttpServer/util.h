#pragma once
#include "log.h"
#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <unordered_map>
using std::cout;
using std::endl;
using std::pair;
using std::string;
string ReadOneLine(int fd);
pair<string, string> SplitStr(const string& str, string sep);
void ShowClientMessage(const pair<string, uint16_t>& msg);

inline string ReadOneLine(int fd) {
    // 读取一行,报文可能以/r作为分隔,也可能以/n作为分隔,也可能以/r/n作为分隔
    string line;
    char ch;
    while (1) {
        ssize_t size = recv(fd, &ch, 1, 0);
        if (!size || !~size) { // 表示读取结束或者读取出错
            break;
        }
        if (ch == '\r') {
            recv(fd, &ch, 1, MSG_PEEK); // 数据窥探
            if (ch == '\n') {           // 表示分隔符是\r\n
                recv(fd, &ch, 1, 0);
            }
            line += '\n';
            break;
        }
        if (ch == '\n') {
            line += '\n';
            break;
        }
        line += ch;
    }
    return line;
}
inline pair<string, string> SplitStr(const string& str, string sep) {
    size_t pos = str.find(sep);
    if (pos == string::npos) {
        return {str, ""};
    }
    return {str.substr(0, pos), str.substr(pos + 1)};
}
inline void ShowClientMessage(const pair<string, uint16_t>& clientmsg) {
    string msg = "accept成功,客户端ip和端口:";
    msg.append("[").append(clientmsg.first).append(":").append(std::to_string(clientmsg.second)).append("]");
    LOG(INFO, msg);
}