#include "Interpreter.h"

Job Interpreter::interprete(std::vector<std::string> jobTokens) {
  Job job;

  Command command;

  for (size_t i = 0; i < jobTokens.size(); i++) {
    if (jobTokens[i] == "<") {
      log(INTER, "wykryto redirect <");
      command.isInputRedirect = true;
      command.inputRedirectName = jobTokens[++i];
    }

    else if (jobTokens[i] == ">") {
      log(INTER, "wykryto redirect >");
      command.isOutputRedirect = true;
      command.outputRedirectName = jobTokens[++i];
    }

    else if (jobTokens[i] == "|") {
      log(INTER, "wykryto pipe |");
      job.commands.push_back(command);
      command = Command();
    } else if (jobTokens[i] == "&") {
      continue;
    } else {
      command.argv.push_back(jobTokens[i]);
    }
  }

  if (command.argv.size() != 0) {
    job.commands.push_back(command);
  }

  if (jobTokens[jobTokens.size() - 1] == "&") {
    log(INTER, "zadanie ma byc uruchomione w tle");
    job.isBackground = true;
  }

  return job;
}
