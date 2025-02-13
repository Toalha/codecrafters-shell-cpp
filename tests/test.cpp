#include <iostream>
#include <filesystem>
#include <cstring>
#include <vector>


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


int main(int argc, char *argv[]){

    std::string input("isto é um teste,vai ser comprido");
    std::vector test = mystrtok(input);

    for(auto it = test.begin(); it != test.end(); it++){
        std::cout << (*it) << std::endl;
    }

    return 0;
}