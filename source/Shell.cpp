#include "Shell.h"
#include "Logger.h"

#define SHELL "SHELL.CPP"

void Shell::printInfo()
{
  std::cout << "---------------\n";
  std::cout << "SHELL UXP1A 2019Z\n";
  std::cout << "Wiktor Michalski\n";
  std::cout << "Robert Ostoja-Lniski\n";
  std::cout << "Wojciech Sitek\n";
  std::cout << "Maciej Szulik\n";
  std::cout << "---------------\n";
}

void Shell::printPrompt()
{ 
  std::string cur_dir = std::experimental::filesystem::current_path();
  std::cout << username << "@" << cur_dir << "> ";
}

void Shell::printNotFinishedPrompt(){
  std::cout << "> ";
}

bool Shell::isInputPreprocessable(std::string input) {

  size_t quoteCount = 0;
  for(auto c : input){
    if(c == '\''){
      quoteCount++;
    }
  }

  return quoteCount % 2 == 0;
}
void Shell::run()
{
  log(SHELL, "###################");
  log(SHELL, "Uruchomiono program.");
  std::string input;
  std::string previousInput;
  username = std::getenv("USER");
  size_t maxSize = 256;

  struct sigaction act;
  act.sa_handler = SIG_IGN; 
  act.sa_flags = 0;

  // blokada sygnalow
  sigaction(SIGINT, &act, NULL);
  sigaction(SIGQUIT, &act, NULL);
  sigaction(SIGTSTP, &act, NULL);
  sigaction(SIGTTIN, &act, NULL);
  sigaction(SIGTTOU, &act, NULL);

  log(SHELL, "sygnaly zablokowane");
  int myPid = getpid();
  setpgid(0, myPid);
  
  if(tcsetpgrp(STDIN_FILENO, myPid ) == - 1){
      throw std::runtime_error("Cannot get grp pid");
  }

  printInfo();
  Exec exec;

  printPrompt();

  while(1)
  {
    
    std::getline(std::cin, input);
    log(SHELL, "uzytkownik podal " + input);
    input = previousInput + input;
    log(SHELL, "calkowite polecenie to: " + input);
    // niektore szczegolne przypadki sa 
    // obslugiwane przed preprocesingiem w celu
    // zwiekszenia wydajnosci
    if(input == "") {
      log(SHELL, "pusty znak");
      printPrompt();
      continue;
    }

    if(input == "|"){
      std::cout << "bash: syntax error near unexpected token `|'" << std::endl;
      printPrompt();
      continue;
    }

    if(input.size() > maxSize){
      input = "";
      log(SHELL, "przekroczono rozmiar maksymalny");
      printPrompt();
      continue;
    }

    if(!isInputPreprocessable(input)){
      log(SHELL, "polecenie otwarte cudzyslowem");
      previousInput = input + "\n";
      printNotFinishedPrompt();
      continue;
    }

    if(input[input.size()-1] == '\\'){
      log(SHELL, "wykryto backslash na koncu linii");
      previousInput = input.substr(0, input.size() - 1);
      printNotFinishedPrompt();
      continue;
    }

    previousInput = "";

    auto parsedInput = parser.parseInput(input);
    for (auto inputChunk : parsedInput)
    {
      if (inputChunk.size() == 0)
        continue;
      auto jobTokens = parser.tokeniseInput(inputChunk);

      if(environment.iterateEnvVariables(jobTokens)) {
        Job job = interpreter.interprete(jobTokens);
        log(SHELL, "przekazano polecenie do wykonania");
        environment.setEnvVariable("?", std::to_string(exec.execute(job)));
      };

    }
    printPrompt();
  }
}