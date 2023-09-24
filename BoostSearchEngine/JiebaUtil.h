#pragma once
#include "./thirdpart/cppjieba/include/cppjieba/Jieba.hpp"
#include <fstream>
#include <string>
#include <unordered_set>
#include <vector>
using std::ifstream;
using std::string;
using std::unordered_set;
using std::vector;
const char *const DICT_PATH = "./thirdpart/cppjieba/dict/jieba.dict.utf8";
const char *const HMM_PATH = "./thirdpart/cppjieba/dict/hmm_model.utf8";
const char *const USER_DICT_PATH = "./thirdpart/cppjieba/dict/user.dict.utf8";
const char *const IDF_PATH = "./thirdpart/cppjieba/dict/idf.utf8";
const char *const STOP_WORD_PATH = "./thirdpart/cppjieba/dict/stop_words.utf8";
class JiebaUtil {
public:
    static vector<string> CutString(const string &words) {
        ReadStopWords();
        vector<string> ans;
        seg.Cut(words, ans);
        for (auto it = ans.begin(); it != ans.end();) {
            if (stop_words.count(*it)) {
                it = ans.erase(it);
            } else {
                it++;
            }
        }
        return ans;
    }
    static void ReadStopWords() {
        // 读取暂停词
        ifstream in(STOP_WORD_PATH, std::ios::in);
        assert(in.is_open());
        string temp;
        int cnt = 0;
        while (getline(in, temp)) {
            stop_words.insert(temp);
            cnt++;
        }
    }

private:
    inline static cppjieba::MixSegment seg = cppjieba::MixSegment(DICT_PATH, HMM_PATH);
    inline static unordered_set<string> stop_words; // 存放暂停词
};