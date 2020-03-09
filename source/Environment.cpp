#include <Environment.h>

void Environment::setEnvVariable(std::string key, std::string value)
{
    hashmap[key] = value;
}

std::string Environment::getEnvVariable(std::string key)
{
    char *ret = getenv(key.c_str());
    if (ret != NULL)
        return std::string(ret);

    if (hashmap[key] != "")
    {
        return hashmap[key];
    }

    return "";
}

bool Environment::iterateEnvVariables(std::vector<std::string> &tokens)
{
    for (auto &token : tokens)
    {
        replaceEnvVariables(token);
        createEnvVariables(token);
        removeQuotes(token);
    }
    tokens = checkExport(tokens);

    if(tokens.size() == 1 && tokens[0] == "") {
        return false;
    }

    return true;
}

std::vector<std::string> Environment::checkExport(std::vector<std::string> tokens)
{
    if (tokens[0] == "export")
    {
        if (tokens.size() == 1)
            return {"export"};

        if (tokens.size() > 1)
        {
            std::string varName = "";
            for (auto character : tokens[1])
            {
                if(character == '=')
                break;
                varName.push_back(character);
            }
            unsetenv(varName.c_str());
            setenv(varName.c_str(), getEnvVariable(varName).c_str(), 1);
            return {""};
        }
    }
    else
    {
        return {tokens};
    }
}

void Environment::removeQuotes(std::string &token)
{
    std::string new_ = "";
    for (auto character : token)
    {
        if (character != '\'')
            new_.push_back(character);
    }
    token = new_;
}

void Environment::createEnvVariables(std::string &token)
{
    //removing quotas:
    std::string varName = "";
    std::string varValue = "";
    bool lookingForName = 1;
    for (auto character : token)
    {
        if (lookingForName)
        {
            if (character == '\'')
            {
                return;
            }
            if (character == '=')
            {
                lookingForName = 0;
                continue;
            }
            varName.push_back(character);
        }
        else
        {
            if (character == '\'')
            {
                continue;
            }
            varValue.push_back(character);
        }
        if (!lookingForName)
            setEnvVariable(varName, varValue);
    }
}

void Environment::replaceEnvVariables(std::string &token)
{
    std::string old_ = token;
    std::string new_ = "";

    bool readingVarName = 0;
    std::string varName = "";
    for (auto character : old_)
    {
        if (character == '$')
        {   
            if(readingVarName == 1)
            {
                new_ += getEnvVariable(varName);
                varName = "";
            }
            else{
            readingVarName = 1;
            continue;
            }
        }

        if (!readingVarName)
        {
            new_.push_back(character);
        }
        else
        {
            if (character == '\'')
            {
                readingVarName = 0;
                if (varName.size() == 0)
                    new_ += '$';
                else
                {
                    new_ += getEnvVariable(varName);
                    new_ += character;
                }

                varName = "";
            }
            else
            {
                varName.push_back(character);
            }
        }
    }
    if (readingVarName)
    {
        if (varName.size() == 0)
            new_ += '$';
        else
        {
            new_ += getEnvVariable(varName);
        }
    }

    token = new_;
}