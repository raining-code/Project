#include "FileUtil.h"
#include <boost/filesystem.hpp>
#include <iostream>
#include <string>
#include <vector>
using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::vector;
struct FileKeyInfo { // HTML文件的标题、摘要、URL
    string title;
    string digest;   // 摘要
    string url;
};
const static string src_path = "./data/input/";
static vector<string> file_list; // 保存源HTML文件的路径
static vector<FileKeyInfo> keyinfos;
bool EnumFiles() {
    boost::filesystem::path root_path(src_path); // 递归起点
    if (boost::filesystem::exists(root_path) == false) {
        cerr << "路径不存在" << endl;
        return false;
    }
    boost::filesystem::recursive_directory_iterator end;              // 结束迭代器
    boost::filesystem::recursive_directory_iterator start(root_path); // 起始迭代器
    while (start != end) {
        if (!boost::filesystem::is_regular(*start)) {                 // 如果当前遍历到目录
            start++;
            continue;
        }
        if (start->path().extension() == ".html") { // 后缀为.html的普通文件
            // cout << start->path().string() << endl;
            // ./data/input/boost/process/extend/on_error.html
            // ./data/input/boost/process/limit_handles.html
            // ./data/input/boost/process/throw_on_error.html
            file_list.push_back(start->path().string());
        }
        start++;
    }
    return true;
}
bool ParseTitle(const string &content, string &title) {
    //<title>Acknowledgements</title>,Acknowledgements为标题
    static const string label1 = "<title>";
    static const string label2 = "</title>";
    size_t start = content.find(label1);
    if (start == string::npos) {
        cerr << "找不到标题的开始位置" << endl;
        return false;
    }
    start += label1.size();
    size_t end = content.find(label2);
    if (end == string::npos) {
        cerr << "找不到标题的结束位置" << endl;
        return false;
    }
    end--;
    title = content.substr(start, end - start + 1);
    return true;
}
enum status {
    LABEL, // 标签
    DIGEST // 摘要
};
bool ParseContent(const string &content, string &digest) {
    /*
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
    <title>User's Guide</title>
    <link rel="stylesheet" href="../../../doc/src/boostbook.css" type="text/css">
    上述文本去标签之后的摘要是"User's Guide"
    */
    enum status s = LABEL;
    for (char c : content) {
        switch (s) {
        case LABEL:
            if (c == '>') { // 表示当前标签处理完毕,往后为有效数据
                s = DIGEST;
            }
            break;
        case DIGEST: // 有效数据
            if (c == '<') {
                s = LABEL;
            } else {
                if (c == '\n') {
                    c = ' '; //'\n'为2个HTML文件KeyInfo之间的分隔符
                }
                digest += c;
            }
            break;
        default:
            break;
        }
    }
    return true;
}
bool ParseUrl(const string &filepath, string &url) {
    //"./data/input/boost/mpi/packed.html"
    //"https://www.boost.org/doc/libs/1_81_0/doc/html/packed.html"
    const static string urlhead = "https://www.boost.org/doc/libs/1_81_0/doc/html/";
    url += urlhead;
    size_t size = src_path.size(); //"./data/input/"
    url += filepath.substr(size);
    return true;
}

bool ParseHtml() {
    // int test = 5;
    for (const string &file : file_list) { // 依次遍历每一个HTML文件
        std::optional<string> content;     // 保存HTML文件的内容
        if (!(content = FileUtil::ReadFile(file))) {
            cerr << "读取文件" << file << "的内容失败" << endl;
            continue;
        }
        FileKeyInfo info; // 保存有关该HTML文件的重要内容
        // 提取标题、提取摘要、构建URL
        if (!ParseTitle(*content, info.title)) {
            cerr << "提取标题失败" << endl;
            continue;
        }
        // 摘要中是包含标题的
        if (!ParseContent(*content, info.digest)) {
            cerr << "提取摘要失败" << endl;
            continue;
        }
        if (!ParseUrl(file, info.url)) {
            cerr << "构建Url失败" << endl;
            continue;
        }
        // 调试
        // cout << "标题: " << info.title << endl;
        // cout << "摘要: " << info.digest << endl;
        // cout << "URL: " << info.url << endl;
        // test--;
        // if (!test) {
        //     break;
        // }
        keyinfos.push_back(std::move(info));
    }
    return true;
}
bool SaveHtml() {
    const static string dst_file = "./data/output/information.txt";
    std::ofstream ofs(dst_file, std::ios::out | std::ios::binary);
    if (!ofs.is_open()) {
        cerr << "打开文件失败" << endl;
        return false;
    }
    for (auto &info : keyinfos) {
        ofs << info.title << '\3' << info.digest << '\3' << info.url << '\n';
    }
    // 在information.txt中,/n作为2个HTML文件之间的分隔,ParseContent函数保证HTML摘要中不包含/n
    ofs.close();
    return true;
}
int main() {
    // 1.递归遍历文件
    if (!EnumFiles()) {
        cerr << "递归遍历文件失败" << endl;
        exit(0);
    }
    cout << "递归遍历文件成功" << endl;
    // 2.解析
    if (!ParseHtml()) {
        cerr << "解析失败" << endl;
        exit(0);
    }
    cout << "解析成功" << endl;
    // 3.回写
    if (!SaveHtml()) { // 将results的结果回写到dst_file
        cerr << "回写到dst_file失败" << endl;
        exit(0);
    }
    cout << "回写到dst_file成功" << endl;
    return 0;
}
