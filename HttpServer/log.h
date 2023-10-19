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
inline void ShowLog(string level, time_t timestamp, string message, string filename, int line) {
    string strTime = TimeStampToStringTime(timestamp);
    printf("[级别:%s][时间:%s][信息:%s][文件:%s][行数:%d]\n", level.c_str(), strTime.c_str(), message.c_str(), filename.c_str(), line);
}
inline string TimeStampToStringTime(time_t timestamp) {
    struct tm* timeinfo = localtime(&timestamp);
    std::ostringstream oss;
    oss << std::setfill('0');
    oss << (timeinfo->tm_year + 1900) << '-';
    oss << std::setw(2) << (timeinfo->tm_mon + 1) << '-';
    oss << std::setw(2) << timeinfo->tm_mday << '-';
    oss << std::setw(2) << timeinfo->tm_hour << ':';
    oss << std::setw(2) << timeinfo->tm_min << ':';
    oss << std::setw(2) << timeinfo->tm_sec;
    return oss.str();
}