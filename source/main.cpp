#include "../include/conflux.hpp"
#include "../testing/samples.hpp"
/*

  total number of paths where at all rows have at least one active logit:

    2^(r*c) - (sum|i=0...r-1|( r_C_i * [2^(c(r-i)) - 1] ) + 1);

*/

// template<std::floating_point num>
// void operator+(std::array<num, 2>& lhs, const injector::binary<num>& rhs){
  
//   lhs[0] += rhs.l_pair.first.send();
//   lhs[1] += rhs.l_pair.second.send();

// }


using namespace set2x2::nand_gate;



int main(void){

  injector::shallow<float, 2, logit::seq> b_1(2, 3);
  // injector::binary<float> b_1(3);

  for(int epoch = 0; epoch < 2'500; ++epoch){

    for(int i = 0; i < 4; ++i){

      b_1.forward(inp[i]);

      b_1.calibrate(inp[i], trg[i]);

    }
  }

  b_1.forward(inp[0]); b_1.display_output();
  b_1.forward(inp[1]); b_1.display_output();
  b_1.forward(inp[2]); b_1.display_output();
  b_1.forward(inp[3]); b_1.display_output();

  

  std::cout << '\n';
  return 0;
}
