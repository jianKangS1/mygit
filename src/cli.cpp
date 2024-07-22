#include <iostream>
#include <memory>
#include <filesystem>
#include "cxxopts.hpp"
#include "cli.hpp"
#include "data.hpp"

const std::filesystem::path current_dir = std::filesystem::current_path();
const std::filesystem::path GIT_DIR = current_dir / ".mgit";
const std::filesystem::path OBJECT_DIR= GIT_DIR / "objects";

std::unique_ptr<Command> parse_command(int argc, const char* argv[]){
  std::unique_ptr<Command> command_ptr;
  std::unique_ptr<cxxopts::Options> allocated(new cxxopts::Options(argv[0], " - example command line options"));
  auto& options = *allocated;
  options
    .positional_help("[optional args]")
    .show_positional_help();

  options
    .set_width(70)
    .set_tab_expansion()
    .allow_unrecognised_options()
    .add_options()
    ("init", "initialize a new repository")
    ("hash-object", "compute object ID and optionally creates a blob from a file",cxxopts::value<std::string>())
    ("cat-file", "output the contents of a repository object", cxxopts::value<std::string>())
    ("write-tree", "create a tree object from the current index")
    ("read-tree", "read a tree object into the current index",cxxopts::value<std::string>());

  auto result = options.parse(argc, argv);
  if(result.count("init")){
    command_ptr = std::make_unique<InitCommand>();
  }
  else if (result.count("hash-object"))
  {
    command_ptr= std::make_unique<HashObjectCommand>(current_dir / result["hash-object"].as<std::string>());
  }else if (result.count("cat-file"))
  {
    command_ptr= std::make_unique<CatFileCommand>(OBJECT_DIR / result["cat-file"].as<std::string>());
  }else if(result.count("write-tree")){
    command_ptr= std::make_unique<WriteTreeCommand>(OBJECT_DIR / result["write-tree"].as<std::string>());

  }
  return command_ptr;
  
};

void show_error_message(const ErrorMessage& error_message){

    switch (error_message.code)
    {
    case ERROR_FILE_NOT_FOUND:
      std::cout<<"File not found "<<error_message.message<<std::endl;
      break;
    case ERROR_FAILED_TO_GET_SHA1:
      std::cout<<"Failed to get SHA1 of file "<<error_message.message<<std::endl;
      break;
    case ERROR_FAILED_TO_READ_FILE:
      std::cout<<"Failed to read file "<<error_message.message<<std::endl;
      break;
    case ERROR_FILE_EXISTS:
      std::cout<<"File already exists "<<error_message.message<<std::endl;
      break;
    case ERROR_INVALID_OBJECT_ID:
      std::cout<<"Invalid object ID "<<error_message.message<<std::endl;
      break;
    default:
      break;
    }
  
};
