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

void func2(std::unique_ptr<std::string> &path){
  path = std::make_unique<std::string>("foi mudado na 2");
}

void func1(std::unique_ptr<std::string> &path){
  path = std::make_unique<std::string>("foi mudado na 1");
  std::cout << *path << std::endl;
  func2(path);
  return;
}

int main(int argc, char *argv[]){

    // std::string test("cavalo azul");
    // std::cout << test.find_first_of(" ") << std::endl;

    std::unique_ptr<std::string> path = std::make_unique<std::string>("initt");
    func1(path);
    std::cout << "main " << *path << std::endl;  
    return 0;
}