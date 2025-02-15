#include <iostream>
#include <filesystem>
#include <cstring>
#include <vector>


// Custom funtion to skip spaces between the command and the argument
inline std::string skip_spaces(std::string str){
  for(int pos = 0; pos<str.length(); pos++){
    if(str[pos] != ' '){
      return str.substr(pos);
    }
  }
  return " ";
}

int main(int argc, char *argv[]){ 
    std::filesystem::path current_directory_path = std::filesystem::current_path();
    std::cout << current_directory_path << std::endl;

    std::string aaaa("cd       /home/");
    std::string aux(skip_spaces(aaaa.substr(2))); // saves the input path without spaces 
    std::cout << aux << std::endl;

    if(std::filesystem::is_directory(aux)){
      current_directory_path = aux;
    }
    std::cout << current_directory_path.c_str() << std::endl;

    // std::cout << std::filesystem::is_directory("/home/") << std::endl;  
    // std::cout << std::filesystem::is_directory("/home2/") << std::endl;
    // std::cout << std::filesystem::is_directory("/home/toalha/Desktop/codecrafters-shell-cpp/CMakeLists.txt") << std::endl; 
    return 0;
}