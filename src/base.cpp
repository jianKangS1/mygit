#include "base.hpp"
#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>
#include <openssl/evp.h>
#include <openssl/err.h>


bool check_file_exist(const std::string& fileName , bool isDir ) { 
    // 拼接路径，检测目标文件夹是否存在
    std::filesystem::path targetDir=fileName;
    // 检测目标路径是否为目录并且存在
    if (std::filesystem::exists(targetDir)) {
        if (std::filesystem::is_directory(targetDir)) {
            return isDir;
        }else{
            return !isDir;
        }
    }else{
        return false;
    }
}

// 为一个文件创建 SHA1 哈希值
ErrorMessage file_SHA1(const std::string& fileName) {
    std::ifstream file(fileName, std::ifstream::binary);
    if (!file.is_open()) {
        //std::cerr << "Cannot open file: " << fileName << std::endl;
        return ErrorMessage(ERROR_FAILED_TO_READ_FILE, fileName);
    }

    // 创建并初始化摘要上下文
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (!ctx) {
        //std::cerr << "Failed to create EVP_MD_CTX" << std::endl;
        return ErrorMessage(ERROR_SHA1_FAILED);
    }

    if (EVP_DigestInit_ex(ctx, EVP_sha1(), nullptr) != 1) {
        //std::cerr << "Failed to initialize digest" << std::endl;
        EVP_MD_CTX_free(ctx);
        return ErrorMessage(ERROR_SHA1_FAILED);
    }

    char buffer[8192];
    while (file.good()) {
        file.read(buffer, sizeof(buffer));
        if (EVP_DigestUpdate(ctx, buffer, file.gcount()) != 1) {
            //std::cerr << "Failed to update digest" << std::endl;
            EVP_MD_CTX_free(ctx);
            return ErrorMessage(ERROR_SHA1_FAILED);
        }
    }

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int lengthOfHash = 0;

    if (EVP_DigestFinal_ex(ctx, hash, &lengthOfHash) != 1) {
        //std::cerr << "Failed to finalize digest" << std::endl;
        EVP_MD_CTX_free(ctx);
        return ErrorMessage(ERROR_SHA1_FAILED);
    }

    // 清理
    EVP_MD_CTX_free(ctx);

    // 转换哈希值为十六进制字符串
    std::stringstream ss;
    for (unsigned int i = 0; i < lengthOfHash; i++) {
        ss << std::hex  << (int)hash[i];
    }

    return ErrorMessage(ERROR_OK, ss.str());
}

// 为字符串数组创建 SHA1 哈希值
ErrorMessage string_SHA1(std::vector<std::string>& strs){
     // 创建并初始化摘要上下文
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (!ctx) {
        //std::cerr << "Failed to create EVP_MD_CTX" << std::endl;
            return ErrorMessage(ERROR_SHA1_FAILED);
    }

    if (EVP_DigestInit_ex(ctx, EVP_sha1(), nullptr) != 1) {
        //std::cerr << "Failed to initialize digest" << std::endl;
        EVP_MD_CTX_free(ctx);
            return ErrorMessage(ERROR_SHA1_FAILED);
    }

    
    for (auto &s: strs){
        if (EVP_DigestUpdate(ctx, s.c_str(), s.size()) != 1) {
            //std::cerr << "Failed to update digest" << std::endl;
            EVP_MD_CTX_free(ctx);
              return ErrorMessage(ERROR_SHA1_FAILED);
        }
    }

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int lengthOfHash = 0;

    if (EVP_DigestFinal_ex(ctx, hash, &lengthOfHash) != 1) {
        //std::cerr << "Failed to finalize digest" << std::endl;
        EVP_MD_CTX_free(ctx);
            return ErrorMessage(ERROR_SHA1_FAILED);
    }

    // 清理
    EVP_MD_CTX_free(ctx);

    // 转换哈希值为十六进制字符串
    std::stringstream ss;
    for (unsigned int i = 0; i < lengthOfHash; i++) {
        ss << std::hex  << (int)hash[i];
    }

    return ErrorMessage(ERROR_OK, ss.str());
};


ErrorMessage convert_dir_to_tree(std::filesystem::path target_dir){
    // 把当前路径下的所有文件和文件夹转换成树结构，返回他的SHA1值
    std::vector<ObjectFile> file_lists;
    // 遍历当目录下的所有项（包括文件和文件夹）
    for (const auto& entry : std::filesystem::directory_iterator(target_dir)) {
        const std::string filenameStr = entry.path();
        if(filenameStr.find(GIT_DIR) != std::string::npos){
            continue; // 忽略.ugit目录
        }
        ObjectFile obj;
        if (entry.is_directory()) {
            ErrorMessage message=convert_dir_to_tree(entry.path());
            if(message.code!= ERROR_OK){
                return message;
            }
            obj.oid=message.message;
            obj.type=TREE_OBJ;
            obj.file_name=filenameStr;
        } else if (entry.is_regular_file()) {
            
            ErrorMessage message=convert_files_to_blob(filenameStr);
            if(message.code!= ERROR_OK){
                std::cerr<<"failed to hash file "<<filenameStr<<"\n";
                return message;
            }
            obj.oid=message.message;
            obj.type=BLOB_OBJ;
            obj.file_name=filenameStr;
        } else {
            std::cerr<<"wrong file type detected\n";
        }
        file_lists.push_back(obj);
    }
    // 计算树结构的SHA1值
    std::vector<std::string> tree_content;
    for(auto& obj:file_lists){
        tree_content.push_back(obj.to_string());
    }
    // 将数据写入新文件中
    ErrorMessage error_message=string_SHA1(tree_content);
    if(error_message.code!= ERROR_OK){
        return error_message;
    }
    std::string oid = error_message.message;
    std::ofstream ofs(OBJECT_DIR / oid, std::ios::binary);
    if(!ofs.is_open()){
        std::cerr<<"failed to open file for writing\n";
        return ErrorMessage(ERROR_FAILED_TO_READ_FILE, oid);
    }
    // 写入类型
    char number_str[OBEJECT_TYPE_LENGTH];
    number_str[0]=TREE_OBJ+'0';
    ofs<<number_str<<SEPERATOR;
    for(auto& str:tree_content){
        ofs<<str;
    }
    ofs.close();
    return ErrorMessage(ERROR_OK, oid);
}

ErrorMessage convert_files_to_blob(const std::string& filename){
    // 把文件转换成blob结构，返回他的SHA1值
    if(!check_file_exist(filename)){
        return ErrorMessage(ERROR_FILE_NOT_FOUND, filename);
    }

    std::string oid=file_SHA1(filename).message;
    // 如果objects目录不存在，则创建
    if(!check_file_exist(OBJECT_DIR, true)){
        std::filesystem::create_directory(OBJECT_DIR);
    }
    //在 .ugit/objects/ 目录下创建文件
    std::filesystem::path object_path= OBJECT_DIR /  oid;
    std::ofstream ofs(object_path, std::ios::binary);
    if(!ofs.is_open()){
        return ErrorMessage(ERROR_FAILED_TO_READ_FILE, object_path.string());
    }
    //将文件内容写入到对象文件中
    std::ifstream ifs(filename, std::ios::binary);
    if(!ifs.is_open()){
        return ErrorMessage(ERROR_FAILED_TO_READ_FILE, filename);
    }
    char buffer[8192];
    // 写入类型
    char number_str[OBEJECT_TYPE_LENGTH];
    number_str[0]=BLOB_OBJ+'0';
    ofs.write(number_str, OBEJECT_TYPE_LENGTH);
    // 写入空格
    ofs.write(SEPERATOR,1);
    while(ifs.good()){
        ifs.read(buffer, sizeof(buffer));
        ofs.write(buffer, ifs.gcount());
    }
    ifs.close();
    ofs.close();
    //std::cout<<oid<<std::endl;
    return ErrorMessage(ERROR_OK, oid);
};

// 把blob对象内容写入到输出流中
ErrorMessage convert_blob_to_stream(std::string& oid, std::ostream& out){

    std::filesystem::path object_path= OBJECT_DIR / oid;
    if(!check_file_exist(object_path)){
        return ErrorMessage(ERROR_INVALID_OBJECT_ID, oid);
    }
    std::ifstream ifs(object_path, std::ios::binary);
    if(!ifs.is_open()){
        return ErrorMessage(ERROR_FAILED_TO_READ_FILE, object_path.string());
    }
    char buffer[8192];
    char file_type[OBEJECT_TYPE_LENGTH];
    //去除类型和空格
    ObjectType type=(ObjectType)(ifs.get()-'0');
    if(type!= BLOB_OBJ){
        std::cerr<<"wrong object type detected\n";
        return ErrorMessage(ERROR_INVALID_OBJECT_ID, oid);
    }
    if(ifs.get() != SEPERATOR[0]){
        return ErrorMessage(ERROR_INVALID_OBJECT_ID, oid);
    }

    //std::cout<<file_type<<std::endl;
    while(ifs.good()){
        ifs.read(buffer, sizeof(buffer));
        out.write(buffer, ifs.gcount());
    }
    ifs.close();
    return ErrorMessage();
};

ErrorMessage convert_tree_to_dir(std::string & oid){
    // 把tree对象内容写入到目录中
    if(!check_file_exist(oid)){
        return ErrorMessage(ERROR_INVALID_OBJECT_ID, oid);
    }
    std::ifstream ifs(oid ,std::ios::binary);
    if(!ifs.is_open()){
        return ErrorMessage(ERROR_FAILED_TO_READ_FILE, oid);
    }

    ObjectFile file;
    while(ifs>>file){
        if(file.type == TREE_OBJ){
            std::filesystem::create_directory(file.file_name);
            ErrorMessage error_message= convert_tree_to_dir(file.oid);
            if(error_message.code!= ERROR_OK){
                return error_message;
            }
        }else if(file.type == BLOB_OBJ){
            std::ofstream blob_ifs(file.file_name, std::ios::binary);
            if(!blob_ifs.is_open()){
                return ErrorMessage(ERROR_FAILED_TO_READ_FILE, file.file_name);
            }
            ErrorMessage error_message= convert_blob_to_stream(file.oid, blob_ifs);
            if(error_message.code!= ERROR_OK){
                return error_message;
            }
            blob_ifs.close();
        }
    }
    return ErrorMessage();
};