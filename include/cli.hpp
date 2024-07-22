#pragma once 
#include <map>
#include <string>
#include <memory>
#include <utility>
enum CommnadType{
    CMD_UNKNOWN,
    CMD_INIT,
    CMD_HASH_OBJECT,
    CMD_CAT_FILE,
    CMD_WRITE_TREE,
    CMD_READ_TREE,
};

enum ERROR_CODE{
    ERROR_OK,
    // 前端的错误
    ERROR_FAILED_TO_PARSE_COMMAND,
    // 后端的错误
    ERROR_FILE_NOT_FOUND,
    ERROR_FILE_EXISTS,
    ERROR_FAILED_TO_READ_FILE,
    ERROR_FAILED_TO_GET_SHA1,
    ERROR_INVALID_OBJECT_ID,

    ERROR_SHA1_FAILED, //sha1计算失败
};

struct ErrorMessage{
    ERROR_CODE code;
    std::string message;

    ErrorMessage(ERROR_CODE code=ERROR_OK, std::string message=""): code(code), message(message){}
};

void show_error_message(const ErrorMessage& error_message);

class Command{
public:
    CommnadType type;
    std::map<std::string, std::string> options=std::map<std::string, std::string>();
    Command(CommnadType type): type(type){
    };
    virtual ErrorMessage handle_command() = 0;
};


std::unique_ptr<Command> parse_command(int argc, const char* argv[]);

class CommandExecutor {
private:
    std::unique_ptr<Command> command;
public:
    void set_cmmand(std::unique_ptr<Command> cmd) {
        command = std::move(cmd);
    }

    ErrorMessage execute_command() {
        if (command) {
            return command->handle_command();
        }else{
            return ErrorMessage(ERROR_FAILED_TO_PARSE_COMMAND,"Command is not set");
        }
    }
};

int start_program(int argc, const char* argv[]);


