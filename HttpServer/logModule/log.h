#pragma once
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
// 日志等级
#define INFO
#define WARNING
#define ERROR
#define FATAL
#define LOG(level, message) ShowLog(#level, time(nullptr), message, __FILE__, __LINE__)
using std::cout;
using std::endl;
using std::string;
void ShowLog(string level, time_t timestamp, string message, string filename, int line);
string TimeStampToStringTime(time_t timestamp);
