#include <bits/stdc++.h>
#include "include/activation.hpp"



int main(void){

  std::vector<float> test1{ 14, 16, -2, 7, -21, 8, 9 };
  std::vector<float> test2{ -14, -16, -2, -7, -21, -8, -9 };

  activation::min_max<float>(test1);
  activation::min_max<float>(test2);


  for(auto& x : test1) { std::cout << x << ' '; } std::cout << '\n';
  for(auto& x : test2) { std::cout << x << ' '; } std::cout << '\n';


  return 0;
}