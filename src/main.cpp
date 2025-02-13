#include <iostream>
// #include <filesystem>
#include <cstring>
#include <vector>

// possible commands
enum commands {
  quit, // exit is the name of a function already
  echo,
  type,
  unknown
};

// Verifies what command is called
commands identify_command(std::string str){
  if(str.length() >= 4 && !str.compare(0,4,"exit")) return quit;
  else if(str.length() >= 4 && !str.compare(0,4,"echo")) return echo;
  else if(str.length() >= 4 && !str.compare(0,4,"type")){return type;}
  else return unknown;
}

std::vector<std::string> mystrtok(std::string string_to_split, std::string token = " "){
  int start(0), found(0);
  std::vector<std::string> output;

  while((found = string_to_split.find(token, start)) != std::string::npos){
    output.push_back(string_to_split.substr(start, found - start));
    start = found + token.length();
  }
  output.push_back(string_to_split.substr(start));
  return output;
}

// Search if the command is in folders specified by $PATH
// Returns: folder where command was found
//          nullptr if not in $PATH
bool search_path(std::string command, std::string** correct_return_path){

  std::string PATH(getenv("PATH")); // maybe change to a global variable if needed elsewhere
  std::vector folders = mystrtok(PATH, ":"); // custom funtion to split the $PATH into a vector of folders

  // iterate through all the folders in path and verifies if the command exists in any of them
  for(auto it = folders.begin(); it != folders.end(); it++){
    std::string filepath((*it) + "/" + command);
    FILE* pfile;
    if((pfile = fopen(filepath.c_str(), "r")) != NULL){
      fclose(pfile);
      (*correct_return_path) = new (std::nothrow) std::string((*it)); // saves path in the correct_return_path variable
      return true;
    }
  }
  return false;
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
    std::string* path(nullptr); //auxiliary pointer to use on "case type:"
    switch(identify_command(input))
    {
    case quit:
      exit_shell(input);
      break;
    
    case echo:
      std::cout << input.substr(5) << std::endl;
      break;
    
    case type:
      // Identifies if what is in the substring starting after "type " is a known command
      if(identify_command(input.substr(5)) != unknown){
        std::cout << input.substr(5) << " is a shell builtin" << std::endl;
      }
      
      // Identifies if the command is somewhere in $PATH
      else if((search_path(input.substr(5), &path))){
        std::cout << input.substr(5) << " is " << (*path) << "/" << input.substr(5) << std::endl;
        delete path;
      }

      // command was not found
      else{
        std::cout << input.substr(5) << ": not found" << std::endl;
      }
      break;
    
    case unknown:
      std::cout << input << ": command not found" << std::endl;
      break;
    }
  }
}