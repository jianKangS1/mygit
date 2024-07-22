#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <assert.h>
#include <vector>
#include "base.hpp"
#include"data.hpp"


const char *SEPERATOR=" "; // 这里跟git的实现有些许不同，这里的分隔符是空格，而git的分隔符是\0 便于调试

// 这里的所有的路径都是绝对路径了


ErrorMessage InitCommand::handle_command(){

    std::filesystem::path targetDir = options.begin()->second;
    if(check_file_exist(GIT_DIR, true)){
        return ErrorMessage(ERROR_FILE_EXISTS, GIT_DIR);
    }

    // 创建目标文件夹
    std::filesystem::create_directory(GIT_DIR);
    return ErrorMessage();

    
}

ErrorMessage HashObjectCommand::handle_command(){
    assert(options.size() >= 1);
    return convert_files_to_blob(options.begin()->second);
}

ErrorMessage CatFileCommand::handle_command(){
    assert(options.size() >= 1);
    return convert_blob_to_stream(options.begin()->second,std::cout);
}


ErrorMessage WriteTreeCommand::handle_command(){
    return convert_dir_to_tree();
};


ErrorMessage ReadTreeCommand::handle_command(){
    assert(options.size() >= 1);
    return convert_tree_to_dir(options.begin()->second);
};