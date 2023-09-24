#pragma once
#include "JiebaUtil.h"
#include "StringUtil.h"
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::unordered_map;
using std::vector;
// 正排索引:id->内容
struct IdDigest {
    string title;  // 标题
    string digest; // 摘要
    string url;
    size_t id = 0; // 文档id
};
struct WeightId {
    size_t id = 0;
    string word;
    size_t weight = 0; // 在特定关键字下文档的权值是多少
};
struct word_cnt {
    size_t title_cnt = 0;
    size_t digest_cnt = 0; // 统计关键字在标题和内容中出现的次数
};
const static string info_path = "./data/output/information.txt";
class Index {
public:
    IdDigest *getForwardIndex(const size_t id);
    vector<WeightId> *getInvertedList(string word);
    static Index *getInstance();
    void InitIndex();

private:
    bool BuildIndex(string infofile_path = info_path);
    IdDigest *BuildForwardIndex(const string &htmlmsg);
    void BuildInvertedIndex(IdDigest *finfo);
    Index() = default;
    Index(const Index &) = delete;
    Index &operator=(const Index &) = delete;

private:
    static Index instance;
    vector<IdDigest> forwardIndex;                         // forwardIndex[i]表示表示文档id为i的文档的内容
    unordered_map<string, vector<WeightId>> invertedIndex; // 一个关键字可能在多个文档中出现. 关键字-文档s-权值
};