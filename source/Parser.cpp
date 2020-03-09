#include "Parser.h"
#include "Logger.h"

Parser::Parser() {}
Parser::~Parser() {}

void Parser::parse(std::string input) {
  if (input.size() == 0) {
    return;
  }
}

std::vector<std::string> Parser::parseInput(std::string input) {
  std::vector<std::string> parsedInput;
  bool inQuote = false;
  std::string job;
  for (auto character : input) {
    if (character == '\'') {
      inQuote = !inQuote;
    }

    if (character == ';' && !inQuote) {

      log(PARSER, "znaleniono wyrazenie: " + job);
      parsedInput.push_back(job);
      job = "";
      continue;
    }

    job.push_back(character);
  }

  log(PARSER, "znaleniono wyrazenie: " + job);
  parsedInput.push_back(job);

  log(PARSER, "zakonczono pierwsze parsowanie inputu");
  return parsedInput;
}
bool Parser::isSpecialSign(char c) {
  return std::find(specialSigns.begin(), specialSigns.end(), c) !=
         specialSigns.end();
}

std::vector<std::string> Parser::tokeniseInput(std::string job) {

  std::vector<std::string> tokenisedInput;
  bool inQuote = false;
  std::string token;
  for (auto character : job) {

    if (character == '\'') {
      // jesli zamykamy quote to push tokena
      token.push_back(character);
      inQuote = !inQuote;
      continue;
    }

    if (!inQuote) {
      if (character == ' ') {
        if (token.size() != 0) {
          log(PARSER, "znaleziono podstawowe wyrazenie: " + token);
          tokenisedInput.push_back(token);
          token = "";
        }
        continue;
      }

      if (isSpecialSign(character)) {
        if (token.size() != 0) {
          log(PARSER, "znaleziono podstawowe wyrazenie: " + token);
          tokenisedInput.push_back(token);
          token = "";
        }
        tokenisedInput.push_back(std::string(1, character));
        continue;
      }
    }
    token.push_back(character);
  }

  if (token.size() != 0) {
    log(PARSER, "znaleziono podstawowe wyrazenie: " + token);
    tokenisedInput.push_back(token);
  }

  log(PARSER, "zakonczone tokenizowanie inputu");  
  return tokenisedInput;
}
