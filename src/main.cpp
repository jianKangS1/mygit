
#include <iostream>
#include <memory>
#include "cxxopts.hpp"
#include "cli.hpp"


int start_program(int argc, const char* argv[])
{
  CommandExecutor executor;
  executor.set_cmmand(parse_command(argc, argv));
  auto result = executor.execute_command();
  show_error_message(result);
  return 0;
}
