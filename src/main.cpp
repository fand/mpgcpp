#include <iostream>
#include <fstream>

#include "mpg.hpp"


int main(int argc, char *argv[]){

  if (argc != 3) {
    std::cout << "args error" << std::endl;
    return -1;
  }


  // generate MPG object
  MPG mpg(argv[1]);
  std::cout << "generated!!" << std::endl << std::endl;
  
  // mpg.frame("I").glitch();

  mpg.output(argv[2]);
  std::cout << "copied" << std::endl;

  return 0;
}
  
