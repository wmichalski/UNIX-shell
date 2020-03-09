#include <map>
#include <string>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include "Logger.h"

#define ENVR "ENV"
class Environment
{
private:
    std::map<std::string, std::string> hashmap;

public:
    Environment(){};
    ~Environment(){};
    void setEnvVariable(std::string, std::string);
    std::string getEnvVariable(std::string);
    void replaceEnvVariables(std::string &);
    void createEnvVariables(std::string &);
    bool iterateEnvVariables(std::vector<std::string> &);
    void removeQuotes(std::string &);
    std::vector<std::string> checkExport(std::vector<std::string>);
};