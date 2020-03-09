#ifndef COMMAND_H
#define COMMAND_H

#include <string>
#include <vector>

struct Command {
  std::vector<std::string> argv;
  bool isInputRedirect = false;
  bool isOutputRedirect = false;
  std::string inputRedirectName;
  std::string outputRedirectName;
};
#endif // !COMMAND_H