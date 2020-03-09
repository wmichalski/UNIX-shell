#include <algorithm>
#include <string>
#include <vector>

#define PARSER "PARSER"
class Parser {

private:
  std::vector<char> specialSigns{'>', '<', '|', '&'};
  bool isSpecialSign(char c);

public:
  Parser();
  ~Parser();
  void parse(std::string input);
  std::vector<std::string> parseInput(std::string input);
  std::vector<std::string> tokeniseInput(std::string);
};