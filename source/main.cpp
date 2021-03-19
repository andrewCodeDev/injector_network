#include "../include/conflux.hpp"


int main(void){

  encoder::one_hot<float, 29, encoder::fndm> o_h;

  o_h.open_file("/home/andrew/Documents/cpp_projects/scratch/injector_network/sample_txt/atlas_spl.txt");
 
  sampler::indexical idx_sampler(o_h.get_trg().size(), 1);
  

  // network ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  injector::bilayer<float, o_h.out_size(), logit::dyn, logit::seq> inj_net(o_h.out_size(), 3);
  // injector::shallow<float, o_h.out_size(), logit::dyn> inj_net(o_h.out_size(), 3);
  

  // test string ~~~~~~~~~~~~~~~~~~~~~~~
  std::string test_str{"Socialism is "}, out_str{""};

  char new_char;

  for( std::size_t epoch{0}; epoch < 100'000; ++epoch ){

    if( epoch % 1'000 == 0 ){ 
      
      for( std::size_t i{0}; i < 20; ++i ){
        
        o_h.encode(test_str, ' ');

        inj_net.forward(o_h);

        new_char = o_h.idx_to_char( idx_sampler.sample_top(inj_net.output()) ); 

        std::cout << "new_char: " << new_char << '\n';
        
        out_str += new_char;

        test_str = std::string(test_str, 1) + new_char;
      }
              
      std::cout << '\n'; inj_net.display_output();

      inj_net.reset_logits();

      std::cout << out_str << '\n';

      out_str = "";

      test_str = "Socialism is ";
    }

    else {
      o_h.next_input();

      inj_net.forward(o_h);

      inj_net.calibrate(o_h.get_trg());
    }

  }

  std::cout << '\n';
  return 0;
}

/*

  // injector::shallow<float, 4, logit::dyn> inj_net(4, 3);
  
  // for( std::size_t epoch{0}; epoch < 2'000; ++epoch ){

  //   inj_net.forward(inp);

  //   if( epoch % 1'000 == 0 ){ inj_net.display_output(); }

  //   inj_net.calibrate(trg[0]);

  // }


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
