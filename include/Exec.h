#include "Job.h"
#include <Command.h>
#include <algorithm>
#include <errno.h>
#include <exception>
#include <fcntl.h>
#include <iostream>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <algorithm>
#include "Logger.h"

#define EXEC "EXEC"
class Exec {
private:
  std::vector<std::string> outerCommands{"ls", "pwd", "echo", "which"};
  std::vector<std::string> innerCommands{"cd", "export", "exit"};
  size_t fifoCounter = 0;
  bool isInnerCommand(Command);
  bool isOuterCommand(Command);

  std::string genNextFifoName();
  void unlinkPipes(size_t firstToRemove, size_t numToRemove);
  std::string fifoFileNamePrefix = "/tmp/shell_fifo";
  std::pair<size_t, size_t> makeFifos(size_t num);
  bool isEnvCommand(Command);

public:
  Exec() = default;
  void executeInnerCommand(Command cmd);
  void executeOuterCommand(Command cmd, std::string pathToWriteFifo,
                           std::string pathToReadFifo);
  int execute(Job job);
};