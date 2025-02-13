#include <iostream>
// possible commands
enum commands {
  quit, // exit is the name of a function already
  echo,
  type,
  unknown
};

commands identify_command(std::string str){
  if(str.length() >= 4 && !str.compare(0,4,"exit")) return quit;
  else if(str.length() >= 4 && !str.compare(0,4,"echo")) return echo;
  else if(str.length() >= 4 && !str.compare(0,4,"type")){return type;}
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
    
    case type:
      // Identifies what is the input after "type"
      switch (identify_command(input.substr(5)))
      { 
      case unknown:
        std::cout << input.substr(5) << ": not found" << std::endl;
        break;

      // If the command isn't unknown, then it is a shell builtin command
      default:
        std::cout << input.substr(5) << " is a shell builtin" << std::endl;
        break;
      }
      break;
    
    case unknown:
      std::cout << input << ": command not found" << std::endl;
      break;
    }
  }
}