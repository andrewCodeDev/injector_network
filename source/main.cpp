#include "../include/conflux.hpp"

int main(void){

  splicer::character chr_spl(30, 1000);

  encoder::one_hot<float, 30, encoder::fndm> o_h;

  std::ifstream ifs("/home/andrew/Documents/cpp_projects/scratch/injector_network/sample_txt/atlas_ref.txt");
  std::ofstream ofs("/home/andrew/Documents/cpp_projects/scratch/injector_network/sample_txt/atlas_spl.txt");

  chr_spl.splice_all(ifs, o_h.get_vocab());

  chr_spl.write_file(ofs);

  chr_spl.display_up_to(4);

  // o_h.display_vocab();
  
  // for(int epoch = 0; epoch < 10'000; ++epoch){

  //   b_1.forward(vec1);
  //   if( epoch % 100 == 0 ){ b_1.display_output(); }
  //   b_1.calibrate(vec2[0]);

  // }



  std::cout << '\n';
  return 0;
}
