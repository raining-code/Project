#include "index.h"
#include <jsoncpp/json/json.h>
#include <unordered_set>
class Searcher {
public:
    static string Search(const string &query) {
        // 将用户请求进行分词
        vector<string> words = JiebaUtil::CutString(query);
        std::unordered_set<string> strset(words.begin(), words.end());
        vector<WeightId> allresult; // 所有词对应的所有文档
        std::unordered_map<int, int> id_index;
        for (string word : strset) {
            boost::to_lower(word);
            if (word.find(' ', 0) != string::npos) {
                continue;
            }
            auto documents = Index::getInstance()->getInvertedList(word); // 找到关键字相关的所有文档
            if (!documents) {
                continue;
            }
            for (const auto &elem : *documents) {
                if (id_index.count(elem.id)) {
                    allresult[id_index[elem.id]].weight += elem.weight;
                    continue;
                }
                allresult.push_back(elem);
                id_index[elem.id] = allresult.size() - 1;
            }
        }
        std::sort(allresult.begin(), allresult.end(), [](const WeightId &l, const WeightId &r) { return l.weight > r.weight; }); // 降序排列
        // 找到了用户需要的所有文档,将文档内容进行返回
        Json::Value root;
        for (const WeightId &item : allresult) {
            auto info = Index::getInstance()->getForwardIndex(item.id);
            assert(info);
            Json::Value val;
            val["title"] = info->title;
            val["content"] = StringUtil::getDesc(info->digest, item.word);
            val["url"] = info->url;
            root.append(val);
        }
        return Json::StyledWriter().write(root); // 所有结果序列化后返回
    }
};