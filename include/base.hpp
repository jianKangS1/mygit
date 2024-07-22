#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include "cli.hpp"
// 数据结构相关的定义
extern const std::filesystem::path GIT_DIR ;
extern const std::filesystem::path OBJECT_DIR;
extern const std::filesystem::path current_dir ;
// 文件的类型 添加到blob中
enum ObjectType {
    BLOB_OBJ=1,
    TREE_OBJ,
    COMMIT_OBJ,
    TAG_OBJ
};

// 文件的类型数组
struct ObjectFile {
    ObjectType type;
    std::string oid;
    std::string file_name; // 用绝对路径表示文件名
    std::string to_string(){
        return std::string(1,type+'0')+" "+oid+" "+file_name+"\n";
    };
        // 重载输入操作符 >>
    friend std::istream& operator>>(std::istream& input, ObjectFile& obj) {
        // 读取整行数据
        std::string line;
        if (std::getline(input, line)) {
            std::istringstream iss(line);

            // 解析类型、oid、文件名
            char type_char;
            std::string oid_str;
            std::string file_name_str;

            // 从字符串流中解析数据
            char c;
            if (iss >> type_char >> oid_str >> file_name_str>>c) {
                // 设置ObjectFile对象的成员变量
                obj.type = static_cast<ObjectType>(type_char - '0');
                obj.oid = oid_str;
                obj.file_name = file_name_str;
            } else {
                // 如果解析失败，标记输入流的错误状态
                input.setstate(std::ios::failbit);
            }
        }
        return input;
    }
};

#define OBEJECT_TYPE_LENGTH 1
extern const char *SEPERATOR;

bool check_file_exist(const std::string& fileName , bool isDir = false);

ErrorMessage file_SHA1(const std::string& fileName) ;
ErrorMessage string_SHA1(std::vector<std::string>& str);
// 把文件夹转换成树对象，返回oid
ErrorMessage convert_dir_to_tree(std::filesystem::path target_dir=current_dir);
// 把文件转换成blob对象，返回oid
ErrorMessage convert_files_to_blob(const std::string& file_path);

// 把blob对象内容写入到输出流中
ErrorMessage convert_blob_to_stream(std::string& oid, std::ostream& out);
// 把树对象转换成文件夹，并且将原来文件夹里的文件还原
ErrorMessage convert_tree_to_dir(std::string & oid);