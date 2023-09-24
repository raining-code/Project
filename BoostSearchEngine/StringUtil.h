#pragma once
#include <boost/algorithm/string.hpp>
#include <string>
#include <vector>
using std::begin;
using std::end;
using std::string;
using std::vector;
class StringUtil {
public:
    static vector<string> CutString(const string &msg, string sep) {
        vector<string> ans;
        boost::split(ans, msg, boost::is_any_of(sep), boost::token_compress_on);
        return ans;
    }
    static string getDesc(const string &content, const string &keyword) {
        auto it = std::search(begin(content), end(content), begin(keyword), end(keyword), [](char x, char y) -> bool { return std::tolower(x) == std::tolower(y); });
        assert(it != end(content));
        size_t firstpos = std::distance(begin(content), it);
        assert(firstpos != string::npos);
        // 向前取50个字符,向后取100个字符
#define PREV_SIZE 50
#define NEXT_SIZE 100
        size_t start = 0;
        size_t end = size(content) - 1;
        if (start + PREV_SIZE < firstpos) {
            start = firstpos - PREV_SIZE;
        }
        if (firstpos + NEXT_SIZE <= end) {
            end = firstpos + NEXT_SIZE;
        }
        return content.substr(start, end - start + 1) + "......";
    }
};
