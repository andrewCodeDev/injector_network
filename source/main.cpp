#include "../include/conflux.hpp"
#include "../testing/samples.hpp"


using namespace set2x2::and_gate;


int main(void){

  std::vector<std::vector<float>> vec1 { {1, 0, 0, 0}, {0, 1, 0, 0 } };
  std::vector<std::vector<float>> vec2 { {0, 0, 1, 0} };


  injector::shallow<float, 4, logit::dyn> b_1(4, 3);

  b_1.display_formulas();
  
  for(int epoch = 0; epoch < 109'000; ++epoch){

    b_1.forward(vec1);

    // b_1.display_output();
    if(epoch % 10'000 == 0) { b_1.display_output(); }

    b_1.calibrate(vec2[0]);
  }

  std::cout << '\n';

  // b_1.forward(inp[0]); b_1.display_output();
  // b_1.forward(inp[1]); b_1.display_output();
  // b_1.forward(inp[2]); b_1.display_output();
  // b_1.forward(inp[3]); b_1.display_output();

  

  std::cout << '\n';
  return 0;
}
