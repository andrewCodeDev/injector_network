#include <bits/stdc++.h>


#include <compare>


template <std::floating_point num> struct implicit{

    num x{1};

    operator num() const { return x; }
};




int main(void){

  std::cout << std::boolalpha;

  implicit<float> a{1}, b{2}, c{3};


  std::cout << 3.0f * b << '\n';
  std::cout << c / b << '\n';

  
  


  return 0;
}