#include "util.h"
string ReadOneLine(int fd) {
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
pair<string, string> SplitStr(const string& str, string sep) {
    size_t pos = str.find(sep);
    if (pos == string::npos) {
        return {str, ""};
    }
    return {str.substr(0, pos), str.substr(pos + 1)};
}