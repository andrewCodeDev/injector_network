#include "../include/conflux.hpp"


int main(void){

  std::vector<std::vector<float>> inp{{1, 0, 0, 0}, {0, 1, 0, 0}};
  std::vector<std::vector<float>> trg{{1, 0, 0, 1}};


  // encoder::one_hot<float, 29, encoder::fndm> o_h;
 
  // sampler::indexical idx_sampler(o_h.get_trg().size(), 5);

  // o_h.open_file("/home/andrew/Documents/cpp_projects/scratch/injector_network/sample_txt/atlas_spl.txt");

  // injector::bilayer<float, o_h.out_size(), logit::dyn, logit::seq> inj_net(o_h.out_size(), 3);


  // std::cout << o_h.current_size() << '\n';
  // std::cout << o_h.out_size() << '\n';

  // std::cout << o_h[0].size();
  
  // for( std::size_t epoch{0}; epoch < 1; ++epoch ){

  //   o_h.next_input();

  //   inj_net.forward(o_h);

  //   inj_net.display_output();
  // }

  // dcdr.display_idxs();

  std::cout << '\n';
  return 0;
}

/*

  // splicer::character chr_spl(30, 1000);

  // encoder::one_hot<float, 30, encoder::fndm> o_h;

  // std::ifstream ifs("/home/andrew/Documents/cpp_projects/scratch/injector_network/sample_txt/atlas_ref.txt");
  // std::ofstream ofs("/home/andrew/Documents/cpp_projects/scratch/injector_network/sample_txt/atlas_spl.txt");

  // chr_spl.splice_all(ifs, o_h.get_vocab());

  // chr_spl.write_file(ofs);

  // chr_spl.display_up_to(4);

  // o_h.display_vocab();
  
  // for(int epoch = 0; epoch < 10'000; ++epoch){

  //   b_1.forward(vec1);
  //   if( epoch % 100 == 0 ){ b_1.display_output(); }
  //   b_1.calibrate(vec2[0]);

  // }


*/
