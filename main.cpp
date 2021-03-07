#include "conflux.hpp"

/*

  total number of paths where at all rows have at least one active logit:

    2^(r*c) - (sum|i=0...r-1|( r_C_i * [2^(c(r-i)) - 1] ) + 1);

*/


int main(void){


  std::string inp{"have a nice da"};

  std::vector<std::vector<float>> mtx(inp.size(), std::vector<float>(25, 0));
  std::vector<float> trg(25, 0); trg[23] = 1.0f;

  sparse_encoder<float>(inp, mtx);

  injector::shallow<float, 25> s_grid(mtx.size(), 25, 3);

  for( std::size_t epoch{0}; epoch < 20'000; ++epoch ){
  for( const auto& vec : mtx ){
    s_grid.forward(vec); 
  } 
  
    if( epoch % 2'000 == 0 ) { s_grid.display_output(); }
    s_grid.calibrate(trg);
      
  }



  // std::vector<float> mtx(4, 0);
  // std::vector<float> trg(4, 0);


  std::vector<float> memo(4, 0);


  // std::vector<std::vector<float>> trg {{1, 0}, {1, 0}, {0, 1}, {1, 0}};


  // grid::binary_switch<float> b_switch(3);

  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~



  std::cout << '\n';
  return 0;
}
