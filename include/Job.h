#ifndef JOB_H
#define JOB_H


#include "Command.h"
#include <vector>

struct Job {
  std::vector<Command> commands;
  bool isBackground;

  Job() { isBackground = false; }
};

#endif // JOB_H