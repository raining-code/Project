#pragma once
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