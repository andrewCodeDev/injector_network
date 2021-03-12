#include <bits/stdc++.h>


void print_swap(){

  float acm{1.0f};
  bool state = false;

  for(float i = 5.0f; i < 10.0f; ++i){

    std::cout << "acm: " << acm << '\n';

    acm = i + (acm != 1.0f) * acm;

  }

    std::cout << "acm: " << acm << '\n';
    std::cout << '\n';

  for(float i = 9.0f; i >= 5.0f; --i){

    std::cout << "acm: " << acm << '\n';

    acm -= i;

  }
    acm = 1.0f;
    std::cout << "acm: " << acm << '\n';

}


int main(void){

  print_swap();

  return 0;
}