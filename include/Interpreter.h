#include <Job.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>
#include "Logger.h"

#define INTER "INTERPRETER"
class Interpreter {
public:
  Interpreter() = default;
  Job interprete(std::vector<std::string> jobTokens);
};