#include <iostream>

int exit_shell(std::string str){
  int err_code = 0;
  sscanf(str.c_str(), "%*s %d", &err_code);
  return err_code;
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
    
    // Checks if there's an exit command
    if(!input.compare(0,5,"exit ")){
      exit(exit_shell(input));
      //std::cout << "exit " << exit_shell(input);
      break;
    }

    std::cout << input << ": command not found" << std::endl;
  }
}
