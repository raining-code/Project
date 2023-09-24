#include "index.h"
Index Index::instance;
bool Index::BuildIndex(string infofile_path) {
    std::ifstream ifs(infofile_path, std::ios::in | std::ios::binary);
    if (ifs.is_open() == false) {
        cerr << "打开文件失败" << endl;
        return false;
    }
    string s;
    int cnt = 0;
    while (std::getline(ifs, s)) {
        cout << "已经完成" << ++cnt << "个文件索引的建立" << endl;
        // s中保存的是一个HTML文件的信息
        IdDigest *finfo = BuildForwardIndex(s);
        if (!finfo) {
            cerr << "建立正排索引失败" << endl;
            continue;
        }
        BuildInvertedIndex(finfo);
    }
    return true;
}
IdDigest *Index::BuildForwardIndex(const string &htmlmsg) {
    // id->内容
    IdDigest finfo;
    vector<string> msg = StringUtil::CutString(htmlmsg, "\3");
    if (msg.size() != 3) {
        return nullptr;
    }
    finfo.title = std::move(msg[0]);
    finfo.digest = std::move(msg[1]);
    finfo.url = std::move(msg[2]);
    finfo.id = forwardIndex.size();
    forwardIndex.push_back(std::move(finfo));
    return &forwardIndex.back();
}
void Index::BuildInvertedIndex(IdDigest *finfo) {
    vector<string> title_words = JiebaUtil::CutString(finfo->title);   // 标题中出现的关键字
    vector<string> digest_words = JiebaUtil::CutString(finfo->digest); // 摘要中出现的关键字
    unordered_map<string, word_cnt> wordcnt;
    for (string &word : title_words) {
        boost::to_lower(word);
        wordcnt[word].title_cnt++;
    }
    for (string &word : digest_words) {
        boost::to_lower(word);
        wordcnt[word].digest_cnt++;
    }
    for (auto &[word, cnt] : wordcnt) {
        WeightId wid;
        wid.id = finfo->id;
        wid.word = word;
        // 出现在标题中,权值计算大一点
        wid.weight = cnt.title_cnt * 10 + cnt.digest_cnt;
        invertedIndex[word].push_back(std::move(wid));
    }
}
IdDigest *Index::getForwardIndex(const size_t id) {
    if (id < forwardIndex.size()) {
        return &forwardIndex[id];
    }
    return nullptr;
}
vector<WeightId> *Index::getInvertedList(string word) {
    if (invertedIndex.find(word) != invertedIndex.end()) {
        return &invertedIndex[word];
    }
    return nullptr;
}
Index *Index::getInstance() {
    return &instance;
}
void Index::InitIndex() {
    BuildIndex();
}