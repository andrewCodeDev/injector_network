#include <bits/stdc++.h>
#include "include/activation.hpp"



int main(void){

  std::string str{"abcdefg"};


  std::string test(str.c_str(), 4);

  std::cout << str.size() << ' ' << str.length() << '\n';
  std::cout << test << ' ' << str.back() << '\n';


  return 0;
}