#include "Exec.h"

bool Exec::isInnerCommand(Command cmd) {

  std::string cmdName = cmd.argv[0];
  return std::find(innerCommands.begin(), innerCommands.end(), cmdName) !=
         innerCommands.end();
}

bool Exec::isOuterCommand(Command cmd) {

  std::string cmdName = cmd.argv[0];
  return std::find(outerCommands.begin(), outerCommands.end(), cmdName) !=
         outerCommands.end();
}

void Exec::executeInnerCommand(Command cmd) {

  std::string cmdName = cmd.argv[0];
  // jesli exit nie trzeba nic alokowac
  if (cmdName == "exit") {
    exit(0);
  }

  size_t argc = cmd.argv.size();
  char **pTableArg = new char *[argc + 1];
  pTableArg[argc] = NULL;

  for (size_t i = 0; i < argc; i++) {
    char *pArg = (char *)cmd.argv[i].c_str();
    pTableArg[i] = pArg;
  }

  if (cmdName == "cd") {
    // cd ma argumenty bez imienia
    if (chdir(pTableArg[1]) == -1) {
      delete[] pTableArg;
      std::cout << "bash: cd: " + std::string(pTableArg[1]) + "No such file or directory" << std::endl;
    }
  } else if (cmdName == "hello") {
    char *username = getenv("USER");
    printf("\nHello %s.\n", username);
  } else if (cmdName == "export") {

    if (*(pTableArg + 1) == NULL) {
      for (char **temp = environ; *temp != NULL; temp++) {
        printf("declare -x %s\n", *temp);
      }
      return;
    }

    char **tmp = pTableArg + 1;
    for (; *tmp != NULL; tmp++) {

      char *s = getenv(*tmp);

      if (strcmp(s, "") != 0) {

        setenv(*tmp, s, 1);
      } else {
        char *m = (char* )malloc(strlen(*tmp) + 1);
        strncpy(m, *tmp, sizeof(strlen(*tmp) + 1));
        putenv(m);
      }
    }
  }
  delete[] pTableArg;

  return;
}

void Exec::executeOuterCommand(Command cmd, std::string pathToWriteFifo,
                               std::string pathToReadFifo) {

  size_t argc = cmd.argv.size();
  char **pTableArg = new char *[argc + 1];
  pTableArg[argc] = NULL;

  for (size_t i = 0; i < argc; i++) {
    // zeby omiac cudzyslowa
    // jesli zaczyna sie i konczy na ' '
    // wytnij srodek
    if (cmd.argv[i].size() >= 3 && cmd.argv[i][0] == '\'' &&
        cmd.argv[i][cmd.argv[i].size() - 1] == '\'') {
      cmd.argv[i] = cmd.argv[i].substr(1, cmd.argv[i].size() - 2);
    }

    char *pArg = (char *)cmd.argv[i].c_str();
    pTableArg[i] = pArg;
  }

  log(EXEC, "R: " + pathToReadFifo);
  log(EXEC, "W: " + pathToWriteFifo);

  // Forking a child
  pid_t pid = fork();

  if (pid == -1) {
    log(EXEC, "fork sie nie powiodl");

  } else if (pid == 0) {
    // REDIRECTS VVVV
    if (cmd.isInputRedirect) {
      int fd = open(cmd.inputRedirectName.c_str(), O_RDONLY);
      if (fd < 0) {
        log(EXEC, "redirect do input sie nie powiodl");
      }
      dup2(fd, STDIN_FILENO);
      close(fd); /*  */
    }

    if (cmd.isOutputRedirect) {
      int fd = open(cmd.outputRedirectName.c_str(), O_WRONLY | O_CREAT, 0644);
      if (fd < 0) {
        log(EXEC, "redirect do output sie nie powiodl");
      }
      dup2(fd, STDOUT_FILENO);
      close(fd);
    }

    if (pathToWriteFifo != "") {

      int fdWriteFifo = open(pathToWriteFifo.c_str(), O_WRONLY);
      if (fdWriteFifo < 0) {
        log(EXEC, "| do zapisu sie nie powiodl");
      }

      dup2(fdWriteFifo, STDOUT_FILENO);
      close(fdWriteFifo);
    }

    if (pathToReadFifo != "") {

      int fdReadFifo = open(pathToReadFifo.c_str(), O_RDONLY);
      if (fdReadFifo < 0) {
        log(EXEC, "| do odczytu sie nie powiodl");
      }

      dup2(fdReadFifo, STDIN_FILENO);
      close(fdReadFifo);
    }

    if (execvp(pTableArg[0], pTableArg) < 0) {
      std::cout << pTableArg[0] << ": command not found" << std::endl;
      delete[] pTableArg;
    }
    exit(0);
  } 
  delete[] pTableArg;
  return;
}

bool Exec::isEnvCommand(Command command) {
  return std::find(command.argv[0].begin(), command.argv[0].end(), '=') != command.argv[0].end();
}
int Exec::execute(Job job) {

  if (job.isBackground == true) {
    log(EXEC, "orzymano zadanie do wykonania w tle");
  }
  auto fifoRange = makeFifos(job.commands.size() - 1);
  size_t outerCounter = 0;
  
  for (size_t i = 0; i < job.commands.size(); i++) {
    if(isEnvCommand(job.commands[i])){
      continue;
    }
    if (isInnerCommand(job.commands[i])) {
      log(EXEC, "polecenie wewnetrzne");
      executeInnerCommand(job.commands[i]);
    } else {
      outerCounter++;
      log(EXEC, "polecenie zewnetrzne");
      std::string pathToWriteFifo;
      std::string pathToReadFifo;
      if (job.commands.size() > 1) {
        if (i == 0) {
          pathToWriteFifo =
              fifoFileNamePrefix + std::to_string(fifoRange.first);
        } else if (i == job.commands.size() - 1) {
          pathToReadFifo =
              fifoFileNamePrefix +
              std::to_string(fifoRange.first + fifoRange.second - 1);
        } else {
          pathToReadFifo =
              fifoFileNamePrefix + std::to_string(fifoRange.first + i - 1);
          pathToWriteFifo =
              fifoFileNamePrefix + std::to_string(fifoRange.first + i);
        }
      }
      executeOuterCommand(job.commands[i], pathToWriteFifo, pathToReadFifo);
    }
  }

  int status, returned = 0;
  for (size_t i = 0; i < outerCounter; i++) {
    wait(&status);
  }
  if(WIFEXITED(status) && outerCounter>0)
  {
    returned = WEXITSTATUS(status);
    //std::cout << "process returned with code: " << returned << std::endl;
  }

  unlinkPipes(fifoRange.first, fifoRange.second);
  return returned;
}
std::pair<size_t, size_t> Exec::makeFifos(size_t n) {

  size_t first = fifoCounter;
  size_t offset = n;

  while (n--) {
    std::string pathToFifo = fifoFileNamePrefix + std::to_string(fifoCounter);

    if (mkfifo(pathToFifo.c_str(), 0666)) {
      log(EXEC, "nie mozna utworzyc fifo: errno " + std::to_string(errno));
    }

    fifoCounter++;
  }

  return {first, offset};
}
void Exec::unlinkPipes(size_t firstToRemove, size_t numToRemove) {

  while (numToRemove--) {
    size_t currentId = firstToRemove + numToRemove;
    std::string pathToRemove = fifoFileNamePrefix + std::to_string(currentId);
    auto ret = unlink(pathToRemove.c_str());
    log(EXEC, std::to_string(currentId) + " unlinkuje " + std::to_string(ret) + std::to_string(errno));
  }
}

std::string Exec::genNextFifoName() {
  return fifoFileNamePrefix + std::to_string(fifoCounter++);
}
