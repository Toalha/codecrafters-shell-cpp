#include <iostream>

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
    // Checks if there's an exit command
    if(!input.compare(0,5,"exit ")){
      exit_shell(input);
    }

    // Checks if there's an echo command
    else if(!input.compare(0,5,"echo ")){
      std::cout << input.substr(5) << std::endl;
    }

    // Unknown commands
    else{
      std::cout << input << ": command not found" << std::endl;
    }
  }
}
