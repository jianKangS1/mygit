#include <gtest/gtest.h>
#include "cli.hpp"
#include "data.hpp"
namespace commandtest {


TEST(CommandTest, INIT) {
    const char *argv[] = {"./mygit", "--init"};
    start_program(3,argv);
    EXPECT_TRUE(check_file_exist("./.mgit",true));
}

TEST(CommandTest, HashObject){
    const char *argv[] = {"./mygit", "--hash-object", "bsa.txt"};
    start_program(4,argv);
    EXPECT_TRUE(check_file_exist(GIT_DIR,true));
    EXPECT_TRUE(check_file_exist(OBJECT_DIR,true));
    const char *argv2[] = {"./mygit", "--cat-file", "95632eb62b995e19b8367d7af796f2df715e9"};
    start_program(4,argv2);
}

TEST(CommandTest, WriteTree){
    const char *argv[] = {"./mygit", "--write-tree"};
    start_program(3,argv);
    EXPECT_TRUE(check_file_exist(GIT_DIR,true));
}

}