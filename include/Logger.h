#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <string.h>
#include <ostream>
#include <sstream>
#include <fstream>
#include <unistd.h>

#define MAX_USERNAME_LEN 64

void log(std::string tag, std::string msg);

#endif