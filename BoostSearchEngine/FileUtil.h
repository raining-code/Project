#pragma once
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
using std::cerr;
using std::cout;
using std::endl;
using std::string;
class FileUtil {
public:
    static std::optional<string> ReadFile(const string &filepath) {
        string result;
        // 将filepath中的内容读取到result中
        std::ifstream ifs(filepath);
        if (ifs.is_open() == false) {
            cerr << "打开文件失败" << endl;
            return std::nullopt;
        }
        string s;
        while (std::getline(ifs, s)) {
            result += s;
        }
        ifs.close();
        return result;
    }
};
