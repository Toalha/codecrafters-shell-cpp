#include <iostream>
#include <vector>
#include <memory>
#include <filesystem> 

// maximum size allowed for the input
constexpr int MAX_INPUT_SIZE = 256;

// possible commands
enum commands {
  quit, // exit is the name of a function already
  echo,
  type,
  pwd,
  external_command,
  unknown
};


bool search_path(std::string command, std::unique_ptr<std::string> &path);


// Verifies what command is called
commands identify_command(std::string str, std::unique_ptr<std::string> &path){
  if(str.length() >= 4 && !str.compare(0,4,"exit")) return quit;
  else if(str.length() >= 4 && !str.compare(0,4,"echo")) return echo;
  else if(str.length() >= 4 && !str.compare(0,4,"type")){return type;}
  else if(str.length() >=3 && !str.compare(0,4,"pwd")){return pwd;}

  //verification of external commands
  else{
    // checks if it is a command without arguments
    if(str.find_first_of(" ") == str.npos && search_path(str, path)){
      return external_command;
    }
    //checks if it is a command with arguments
    else if(str.find_first_of(" ") != str.npos && search_path(str.substr(0,str.find_first_of(" ")), path)){
      return external_command;
    }
    else{
      return unknown;
    }
  }
  return unknown;
}

// Custom implementation of C function strtok() that works with strings
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
bool search_path(std::string command, std::unique_ptr<std::string> &path){

  std::string PATH(getenv("PATH")); // maybe change to a global variable if needed elsewhere
  std::vector folders = mystrtok(PATH, ":"); // custom funtion to split the $PATH into a vector of folders

  // iterate through all the folders in path and verifies if the command exists in any of them
  for(auto it = folders.begin(); it != folders.end(); it++){
    std::string filepath((*it) + "/" + command);
    FILE* pfile;
    if((pfile = fopen(filepath.c_str(), "r")) != NULL){
      fclose(pfile);
      // saves path in the path variable
      path = std::make_unique<std::string>((*it));//std::unique_ptr<std::string>(new (std::nothrow) std::string((*it))); 
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

// funtion to limit the input string of the shell to prevent crashes
std::string limit_input_string(){
  char buffer[MAX_INPUT_SIZE + 1];
  std::cin.getline(buffer, MAX_INPUT_SIZE);
  return std::string(buffer);
}

int main() {
  while(true){
    std::unique_ptr<std::string> path = std::make_unique<std::string>(); //variable storing useful paths to files/commands 
    //std::make_unique<std::string>("First Path");

    //std::string* path(nullptr); //auxiliary pointer to use on "case type:"

    // Flush after every std::cout / std:cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    // Uncomment this block to pass the first stage
    std::cout << "$ ";

    std::string input = limit_input_string();
    
    // Commands ---------------------------------------------
    switch(identify_command(input, path))
    {
    case quit:
      exit_shell(input);
      break;
    
    case echo:
      std::cout << input.substr(5) << std::endl;
      break;
    
    case type:{
      // Command not found
      commands command = identify_command(input.substr(5), path);
      if(command == unknown){
        std::cout << input.substr(5) << ": not found" << std::endl;
      }
      else if(command == external_command){
        std::cout << input.substr(5) << " is " << (*path) << "/" << input.substr(5) << std::endl;
      }
      else{
        std::cout << input.substr(5) << " is a shell builtin" << std::endl;
      }
      break;
    }
    case pwd:
      path = std::make_unique<std::string>(std::filesystem::current_path());
      std::cout << (*path) << std::endl;
      break;

    case external_command:
      system(input.c_str());  
      break;
    
    case unknown:
      std::cout << input << ": command not found" << std::endl;
      break;
    }
  }
}