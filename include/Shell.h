#include "Interpreter.h"
#include "Environment.h"
#include "Parser.h"
#include "Exec.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <experimental/filesystem>
#include <functional>

class Shell
{

private:
    std::string username;
    Parser parser;
    Interpreter interpreter;
    Environment environment;
    void printInfo();
    void printPrompt();
    void printNotFinishedPrompt();
    bool isInputPreprocessable(std::string);
public:
    Shell() : parser(), interpreter(), environment(){};
    ~Shell(){};
    void run();
};