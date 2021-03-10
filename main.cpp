#include "conflux.hpp"
#include "samples.h"

/*

  total number of paths where at all rows have at least one active logit:

    2^(r*c) - (sum|i=0...r-1|( r_C_i * [2^(c(r-i)) - 1] ) + 1);

*/

// template<std::floating_point num>
// void operator+(std::array<num, 2>& lhs, const injector::binary<num>& rhs){
  
//   lhs[0] += rhs.l_pair.first.send();
//   lhs[1] += rhs.l_pair.second.send();

// }




int main(void){

  injector::shallow<float, 2, logit::imd> b_1(3);

  // for(int epoch = 0; epoch < 10'000; ++epoch){

  // }

  

  std::cout << '\n';
  return 0;
}
