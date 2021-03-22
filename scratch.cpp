#include <bits/stdc++.h>

#include "include/logits.hpp"

// void print_test(std::string str, const float& x)
// {
//   std::cout << str << ": " <<  x << '\n';
// }

template<typename t> t sigmoid(const t& x)
{
  return 1.0 / (1.0 + std::exp(-x));
}



int main(void){

  logit::immediate<double> a(1, 3), b(1, 3), c(1, 3);

  for( size_t i{0}; i < 500; ++i ){
    
    a.take(0, 1);

    b.take(0, a.view());

    c.take(0, b.view());

    c = sigmoid(c.view());

    std::cout << c.view() << '\n';

  // backward ~~~~~~~~~~~~~~~~~~~~

    c.mul_error( c.view() - 1.0 );

    b.mul_error( c.dy_dx(0, b.view()) );
    c.calibrate();
    
    a.mul_error( b.dy_dx(0, a.view()) );
    b.calibrate();

    // update ~~~~~~~~~~~~~~~~~~~~

    a.calibrate();

  }



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  std::cout << '\n'; //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  return 0;
}