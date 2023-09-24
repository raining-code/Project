#include "./thirdpart/cpphttplib/httplib.h"
#include "Daemon.h"
#include "Search.h"
const std::string root_path = "./wwwroot";
int main() {
    // MyDaemon();
    Index::getInstance()->InitIndex();
    httplib::Server ser;
    ser.set_base_dir(root_path);
    ser.Get("/s", [](const httplib::Request &req, httplib::Response &rsp) {
        if (!req.has_param("word")) { // 表示URL中没有携带关键字(参数)
            rsp.set_content("输入的URL中不携带任何关键字!", "text/plain; charset=UTF-8");
            return;
        }
        std::string word = req.get_param_value("word"); // 提取用户传递的关键字
        cout << "用户搜索的关键字是 " << word << endl;
        // 根据关键字构建响应
        string json_str = Searcher::Search(word);
        rsp.set_content(json_str, "application/json");
    });
    ser.listen("0.0.0.0", 8081);
    return 0;
}
