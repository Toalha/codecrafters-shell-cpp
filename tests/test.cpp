#include <iostream>
#include <filesystem>
#include <cstring>
#include <vector>

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


  for(std::string instr: _commands){
    if(!instr.compare("..")){
      aux = aux.parent_path();
    }
    else if(!instr.compare(".")){
      //do nothing
    }
    else{
      aux.append(instr);
    }
  }
  //checks if directory is valid, if not returns the original directory
  if(std::filesystem::is_directory(aux)){
    return aux;
  }
  else{
    std::cout << "cd: " << aux << ": No such file or directory" << std::endl;
    return curr_path;
  }
}





// separates words by " " and "' '"
// example: input:  "aaa 'bbb ccc' ddd"
//          output: [aaa, 'bbb ccc', ddd]
std::vector<std::string> process_input_commands(std::string input){
  int start(0), end(0);
  bool is_quote_open = false;
  std::vector<std::string> output;

  for(int i = 0; i < input.length(); i++){
    if(!is_quote_open){
      if(input[i] == ' ' && start!=-1){
        output.push_back(input.substr(start, i));
        start=-1;
      }
      else if(input[i] == '\''){
        is_quote_open = !is_quote_open;
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
        output.push_back(input.substr(start, i));
        is_quote_open = !is_quote_open;
        start = -1;
      }
    }
  }
  // saves the rest of the input if there's any left
  if(input.substr(start).length() != 0){
    output.push_back(input.substr(start));
  }
  return output;
}








int main(int argc, char *argv[]){ 

    std::string aaaa("cd           /tmp/blueberry");
    //std::string aux(skip_spaces(aaaa.substr(2)));

    std::vector<std::string> clean_input = process_input_commands(aaaa);
    for(auto word : clean_input){
      std::cout << word << std::endl;
    }


    // std::cout << "is directory? " << std::filesystem::is_directory(aux) << std::endl;
    // std::cout << "path " << current_directory_path << std::endl;
    


    //current_directory_path = update_current_directory_path(current_directory_path, aux);
    // std::cout << "auxpath " << auxpath << std::endl;
    // std::cout << "is directory? " << std::filesystem::is_directory(auxpath) << std::endl;

    // if(std::filesystem::is_directory(auxpath)){

    //   if(std::filesystem::path(aux).is_absolute()){
    //     std::cout << "absolute" << std::endl;
    //     current_directory_path = aux;
    //   }
    //   else{
    //   std::cout << "not absolute" << std::endl;
    //     current_directory_path = auxpath;
    //   }
    // }
    // else{
    //   std::cout << "cd: " << aux << ": No such file or directory" << std::endl;
//    }

    //std::cout << current_directory_path << std::endl;


    // std::cout << std::filesystem::is_directory("/home/") << std::endl;  
    // std::cout << std::filesystem::is_directory("/home2/") << std::endl;
    // std::cout << std::filesystem::is_directory("/home/toalha/Desktop/codecrafters-shell-cpp/CMakeLists.txt") << std::endl; 
    return 0;
}