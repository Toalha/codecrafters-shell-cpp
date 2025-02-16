#include <iostream>
#include <vector>
#include <memory>
#include <filesystem> 

// maximum size allowed for the input
constexpr int MAX_INPUT_SIZE = 256;

struct tokenized_inputs{
  std::string token;
  enum quote_type{
    no_quote,
    single_quote,
    double_quote
  };
  quote_type quote;

  //constructor for the structure
  tokenized_inputs(std::string _token, quote_type _quote){
    token = _token;
    quote = _quote;
    return;
  }
};

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
  if(!str.compare("exit")) return quit;
  else if(!str.compare("echo")) return echo;
  else if(!str.compare("type")){return type;}
  else if(!str.compare("pwd")){return pwd;}
  else if(!str.compare("cd")){return cd;}

  //verification of external commands
  else{
    // checks if it is an external command present in path
    if(search_path(str, path)){
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

// separates words by " " and "' '"
// example: input:  "aaa 'bbb ccc' ddd"
//          output: [aaa, 'bbb ccc', ddd]
std::vector<tokenized_inputs> process_input_commands(std::string input){
  int start(0), end(0);
  bool single_quote_open = false;
  bool consec_single_quote = false;

  std::vector<tokenized_inputs> output;

  for(int i = 0; i < input.length(); i++){
    if(!single_quote_open){
      if(input[i] == ' '){
        if(start!=-1){
          output.push_back(tokenized_inputs(input.substr(start, i-start), tokenized_inputs::no_quote));
          start=-1;
        }
      }
      else if(input[i] == '\''){
        single_quote_open = true;
        start = i;
      }
      //skips spaces if theres more than one between arguments
      else{
        if(start == -1){
          start = i;
        }
      }
    }
    else{
      if(input[i] == '\''){
        // if conseq single quotes then concatenate words together
        if(consec_single_quote){
          output.back().token.append(input.substr(start+1, i-start-1));
          consec_single_quote = false;
          single_quote_open = false;
          start = -1;
        }
        else{
          //+1/-1 because we dont want to include the ' in the string
          output.push_back(tokenized_inputs(input.substr(start+1, i-start-1), tokenized_inputs::single_quote)); 
          single_quote_open = false;
          start = -1;
        }

        //checks if consecutive single quotes
        if(i < input.length()-1 && input[i+1] == '\''){
          consec_single_quote = true;
        }
      }
    }
  }
  // saves the rest of the input if there's any left
  if(start != -1 && input.substr(start).length() != 0){
    //output.push_back(input.substr(start));
    output.push_back(tokenized_inputs(input.substr(start), tokenized_inputs::no_quote));
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


// funtion to limit the input string of the shell to prevent crashes
std::string limit_input_string(){
  char buffer[MAX_INPUT_SIZE + 1];
  std::cin.getline(buffer, MAX_INPUT_SIZE);
  return std::string(buffer);
}

// prints the inputs with the format of the process_input_commands() function
inline void print_inputs(std::vector<tokenized_inputs> input, int start = 0){
  for(int i = start; i < input.size(); i++){
    if(i != start){
      std::cout << " ";
    }
    std::cout << input[i].token;
  }
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

    std::vector<tokenized_inputs> input = process_input_commands(limit_input_string());


    // Commands ---------------------------------------------
    switch(identify_command(input[0].token, path))
    {
    case quit:
      if(input.size() == 2){
        exit(atoi(input[1].token.c_str()));
      }
      else{
        exit(0);
      }
      break;
    
    case echo:
      print_inputs(input, 1);  std::cout << std::endl;
      break;
    
    case type:{
      commands command;
      if(input.size() < 2){
        command = unknown;
      }
      else{
        command = identify_command(input[1].token, path);
      }

      if(command == unknown){
        std::cout << input[1].token << ": not found" << std::endl;
      }
      else if(command == external_command){
        std::cout << input[1].token << " is " << (*path) << "/" << input[1].token << std::endl;
      }
      else{
        std::cout << input[1].token << " is a shell builtin" << std::endl;
      }
      break;
    }
    case pwd:
      std::cout << current_directory_path.c_str() << std::endl;
      break;

    case cd:
      if(input.size() == 1){
        current_directory_path = update_current_directory_path(current_directory_path, "~");
      }
      if(input.size() > 2){
        std::cout << "bash: cd: too many arguments" << std::endl;
      }
      current_directory_path = update_current_directory_path(current_directory_path, input[1].token);
      break;
    
    case external_command:{
      std::string ext_command;
      for(tokenized_inputs _token : input){
        if(_token.quote == tokenized_inputs::no_quote){
          ext_command.append(_token.token + " ");
        }
        else if(_token.quote == tokenized_inputs::single_quote){
          ext_command.append("'"+ _token.token + "'" + " ");
        }
        
      }
      system(ext_command.c_str());  
      break;
    }

    
    case unknown:
      std::cout << input[0].token << ": command not found" << std::endl;
      break;
    }
  }
}