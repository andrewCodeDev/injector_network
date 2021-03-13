#include "../include/conflux.hpp"
#include "../testing/samples.hpp"


using namespace set2x2::and_gate;


int main(void){

  // encoder::one_hot<float, 14, encoder::lwrc> o_h;

  std::vector<std::vector<float>> vec1 { {1, 0, 0, 0}, {0, 1, 0, 0 } };
  std::vector<std::vector<float>> vec2 { {0, 0, 0, 1} };

  // // o_h.encode("have a nice da", 'y');

  injector::shallow<float, 4, logit::seq> b_1(4, 3);
  // injector::binary<float> b_1(3);j

  for(int epoch = 0; epoch < 1000; ++epoch){

    for(int i = 0; i < 2; ++i){

      b_1.forward(vec1[i]);

      if(epoch % 10 == 0) { b_1.display_output(); }
    }
      b_1.calibrate(vec1[0]);
  }

  // for(int epoch = 0; epoch < 1'000; ++epoch){

  //   for(std::size_t i = 0; i < o_h.current_size(); ++i){

  //     b_1.forward(o_h.cycle_inp());

  //   }
  //     if(epoch % 100 == 0) { b_1.display_output(); }

  //     b_1.calibrate(o_h.get_trg());
  // }
  std::cout << '\n';

  // b_1.forward(inp[0]); b_1.display_output();
  // b_1.forward(inp[1]); b_1.display_output();
  // b_1.forward(inp[2]); b_1.display_output();
  // b_1.forward(inp[3]); b_1.display_output();

  

  std::cout << '\n';
  return 0;
}
