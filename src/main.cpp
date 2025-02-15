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
  cd,
  external_command,
  unknown
};


bool search_path(std::string command, std::unique_ptr<std::string> &path);


// Verifies what command is called
commands identify_command(std::string str, std::unique_ptr<std::string> &path){
  if(str.length() >= 4 && !str.compare(0,4,"exit")) return quit;
  else if(str.length() >= 4 && !str.compare(0,4,"echo")) return echo;
  else if(str.length() >= 4 && !str.compare(0,4,"type")){return type;}
  else if(str.length() >=3 && !str.compare(0,3,"pwd")){return pwd;}
  else if(str.length() >=2 && !str.compare(0,2,"cd")){return cd;}

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

// Custom implementation of C function strtok() that works with strings and returns a vector of strings with each split string in order
std::vector<std::string> mystrtok(std::string string_to_split, std::string token = " "){
  int start(0), found(0);
  std::vector<std::string> output;

  while((found = string_to_split.find(token, start)) != std::string::npos){
    output.push_back(string_to_split.substr(start, found - start));
    start = found + token.length();
  }
  if(string_to_split.substr(start).length() != 0){
    output.push_back(string_to_split.substr(start));
  }
  return output;
}

// Custom funtion to skip spaces between the command and the argument
inline std::string skip_spaces(std::string str){
  for(int pos = 0; pos<str.length(); pos++){
    if(str[pos] != ' '){
      return str.substr(pos);
    }
  }
  return " ";
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

// custom function to update the current directory path based on the intructions in the cd command
// inputs: current filesystem::path, cd comman
// output: updated filesystem::path if changes to a valid directory
//         old path if its an invalid cd command
std::filesystem::path update_current_directory_path(std::filesystem::path curr_path, std::string cd_commands){
  // verifies if the cd command is a valid and absolute path, if so updates curr_path to the cd command
  if(std::filesystem::is_directory(cd_commands) && std::filesystem::path(cd_commands).is_absolute()){
    return cd_commands;
  }

  std::filesystem::path aux = curr_path;
  std::vector<std::string> _commands = mystrtok(cd_commands, "/");

  for(auto it = _commands.begin(); it != _commands.end(); it++){
    if(it == _commands.begin() && !(*it).compare("~")){
      aux = getenv("HOME");
    }

    else if(!(*it).compare("..")){
      aux = aux.parent_path();
    }
    else if(!(*it).compare(".")){
      //do nothing
    }
    else{
      aux.append((*it));
    }
  }

  //checks if directory is valid, if not returns the original directory
  if(std::filesystem::is_directory(aux)){
    return aux;
  }
  else{
    std::cout << "cd: /" << aux.stem().c_str() << ": No such file or directory" << std::endl;
    return curr_path;
  }
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
  std::filesystem::path current_directory_path = std::filesystem::current_path();
  while(true){
    std::unique_ptr<std::string> path = std::make_unique<std::string>(); //variable storing useful paths to files/commands 
    

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
      std::cout << current_directory_path.c_str() << std::endl;
      break;

    case cd:{
      std::string aux(skip_spaces(input.substr(2))); // saves the input path without spaces 
      current_directory_path = update_current_directory_path(current_directory_path, aux);
      break;
    }
    case external_command:
      system(input.c_str());  
      break;
    
    case unknown:
      std::cout << input << ": command not found" << std::endl;
      break;
    }
  }
}