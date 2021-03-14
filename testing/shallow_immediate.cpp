#include "../include/conflux.hpp"
#include "samples.hpp"


template<class network> bool binary_eval(const network& n) {
  return (n[0]() > n[1]()) ? true : false;
}


int main(void){

  // BINARY UNIT TESTS
    { // AND GATE

      using namespace set2x2::and_gate;


      injector::shallow<float, 2, logit::imd> b_1(2, 3);

      for(int epoch = 0; epoch < 10'000; ++epoch){

        for(int i = 0; i < 4; ++i){

          b_1.forward(inp[i]);

          b_1.calibrate(inp[i], trg[i]);

        }
      }

      b_1.forward(inp[0]);
      assert(binary_eval(b_1) == (false & false));

      b_1.forward(inp[1]);
      assert(binary_eval(b_1) == (true & false));

      b_1.forward(inp[2]);
      assert(binary_eval(b_1) == (false & true));

      b_1.forward(inp[3]);
      assert(binary_eval(b_1) == (true & true));

    }

    { // NAND GATE

    using namespace set2x2::nand_gate;


      injector::shallow<float, 2, logit::imd> b_1(2, 3);

      for(int epoch = 0; epoch < 10'000; ++epoch){

        for(int i = 0; i < 4; ++i){

          b_1.forward(inp[i]);

          b_1.calibrate(inp[i], trg[i]);

        }
      }

      b_1.forward(inp[0]);
      assert(binary_eval(b_1) == !(false & false));

      b_1.forward(inp[1]);
      assert(binary_eval(b_1) == !(true & false));

      b_1.forward(inp[2]);
      assert(binary_eval(b_1) == !(false & true));

      b_1.forward(inp[3]);
      assert(binary_eval(b_1) == !(true & true));

    }

    { // OR GATE

    using namespace set2x2::or_gate;


      injector::shallow<float, 2, logit::imd> b_1(2, 3);

      for(int epoch = 0; epoch < 10'000; ++epoch){

        for(int i = 0; i < 4; ++i){

          b_1.forward(inp[i]);

          b_1.calibrate(inp[i], trg[i]);

        }
      }

      b_1.forward(inp[0]);
      assert(binary_eval(b_1) == (false | false));

      b_1.forward(inp[1]);
      assert(binary_eval(b_1) == (true | false));

      b_1.forward(inp[2]);
      assert(binary_eval(b_1) == (false | true));

      b_1.forward(inp[3]);
      assert(binary_eval(b_1) == (true | true));

    }

    { // NOR GATE

      using namespace set2x2::nor_gate;


      injector::shallow<float, 2, logit::imd> b_1(2, 3);

      for(int epoch = 0; epoch < 10'000; ++epoch){

        for(int i = 0; i < 4; ++i){

          b_1.forward(inp[i]);

          b_1.calibrate(inp[i], trg[i]);

        }
      }

      b_1.forward(inp[0]);
      assert(binary_eval(b_1) == !(false | false));

      b_1.forward(inp[1]);
      assert(binary_eval(b_1) == !(true | false));

      b_1.forward(inp[2]);
      assert(binary_eval(b_1) == !(false | true));

      b_1.forward(inp[3]);
      assert(binary_eval(b_1) == !(true | true));

    }

  std::cout << "\nAll Tests Passed\n\n";

}
