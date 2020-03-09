#include "Logger.h"

void log(std::string tag, std::string msg){

    char linuxName[MAX_USERNAME_LEN];
    memset(linuxName, 0x00, MAX_USERNAME_LEN);

    if (getlogin_r(linuxName, MAX_USERNAME_LEN)) {
        throw std::runtime_error("nie mozna pobrac nazwy uzytkownika unix");
    }

    std::string filePath = "/home/" + std::string(linuxName) + "/shell.log";
    std::ofstream configFile;
    // dodaje nowy rekord
    configFile.open(filePath, std::ios::in | std::ios::app);
    configFile << "(" << tag << "): " << msg << std::endl;
}
