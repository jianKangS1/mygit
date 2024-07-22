#pragma once

#include <map>
#include "cli.hpp"
#include "base.hpp"
// 命令相关的





class InitCommand : public Command {
    public:
    InitCommand() :Command(CMD_INIT) {

    }
    ErrorMessage handle_command() override;
};

class HashObjectCommand : public Command {
    public:
    HashObjectCommand(std::string filename) :Command(CMD_HASH_OBJECT) {
        this->options["file-name"]=filename;
    }
    ErrorMessage handle_command() override;
};

class CatFileCommand : public Command {
    public:
    CatFileCommand(std::string object_id) :Command(CMD_CAT_FILE) {
        this->options["object-id"]=object_id;
    }
    ErrorMessage handle_command() override;
};


class WriteTreeCommand : public Command {
    public:
    WriteTreeCommand() :Command(CMD_WRITE_TREE) {
        // 默认的运行的就是当前目录
    }
    ErrorMessage handle_command() override;
};

class ReadTreeCommand : public Command {
    public:
    ReadTreeCommand(std::string oid) :Command(CMD_READ_TREE) {
        this->options["object-id"]=oid;
    }

    ErrorMessage handle_command() override;
};