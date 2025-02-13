#include <iostream>
// possible commands
enum commands {
  quit, // exit is the name of a function already
  echo,
  unknown
};

commands identify_command(std::string str){
  if(str.length() > 5 && !str.compare(0,5,"exit ")) return quit;
  else if(str.length() > 5 && !str.compare(0,5,"echo ")) return echo;
  else return unknown;
}

// exits the shell with err_codes 
void exit_shell(std::string str){
  int err_code = 0;
  sscanf(str.c_str(), "%*s %d", &err_code);
  exit(err_code);
}

int main() {
  while(true){
    // Flush after every std::cout / std:cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    // Uncomment this block to pass the first stage
    std::cout << "$ ";

    std::string input;
    std::getline(std::cin, input);
    
    // Commands ---------------------------------------------
    switch(identify_command(input))
    {
    case quit:
      exit_shell(input);
      break;
    
    case echo:
      std::cout << input.substr(5) << std::endl;
      break;
    
    case unknown:
      std::cout << input << ": command not found" << std::endl;
      break;
    }
  }
}