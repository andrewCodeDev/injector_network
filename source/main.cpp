#include "../include/conflux.hpp"

int main(void){
srand(time(0));


  // encoder::packet<float> p_enc(60);

  // p_enc.open_file("/home/andrew/Documents/cpp_projects/scratch/algorithm/splicer_test/atlas_spl.txt");

  

  // network ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // injector::graph<float, 10, p_enc.out_size> inj_net(p_enc.out_size, 4);

  // std::string test_str{"Socialism is a value system "}, out_str{""};
  
  // char new_char;

/*
//   for( std::size_t epoch{0}; epoch < 100; ++epoch ){

    // if( epoch % 1 == 0 ){ 
  
    //   inj_net.reset_logits();
      
    //   for( std::size_t i{0}; i < 20; ++i ){
        
    //     p_enc.encode(test_str);

    //     inj_net.forward(p_enc);

    //     new_char = p_enc.idx_to_char( idx_sampler.sample_top(inj_net.output()) ); 
        
    //     out_str += new_char;

    //     test_str = std::string(test_str, 1) + new_char;
    //   }
              
    //   std::cout << '\n'; inj_net.display_output();

    //   inj_net.reset_logits();

    //   std::cout << test_str << new_char << '\n';

    //   out_str = "";

    //   test_str = "Socialism is a value system ";
    // }

    // else {

      // std::cout << '\n';
      
      // p_enc.next_input();
      
      // // p_enc.display_inp();
      // // p_enc.display_trg();

      // std::cout << inj_net.output().size() << '\n';

      // inj_net.forward(p_enc);

      // inj_net.calibrate(p_enc);

      // inj_net.display_output();
    // }

  // }

*/
  


  encoder::packet<double> p_enc1(15, "/home/andrew/Documents/cpp_projects/scratch/injector_network/sample_txt/char_test.txt");
  encoder::packet<double> p_enc2(15, "/home/andrew/Documents/cpp_projects/scratch/injector_network/sample_txt/char_test.txt");
  encoder::packet<double> p_enc3(15, "/home/andrew/Documents/cpp_projects/scratch/injector_network/sample_txt/char_test.txt");


  sampler::indexical idx_sampler(p_enc1.out_size, 5);

  // network ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  injector::graph<double, 15, p_enc1.out_size> inj_net(p_enc1.out_size, 3);

  p_enc1.next_input();
  p_enc2.next_input().next_input();
  p_enc3.next_input().next_input().next_input();

  // std::cout << p_enc1.current_size() << "\n\n";
  // std::cout << inj_net.size() << "\n\n";

  p_enc1.display_inp();
  p_enc1.display_trg();

  for( std::size_t epoch{0}; epoch < 2'000; ++epoch ){

    inj_net.forward(p_enc1);
    if(epoch % 100 == 0){ inj_net.display_output(); std::cout << '\n'; }
    inj_net.calibrate(p_enc1);

    inj_net.forward(p_enc2);
    inj_net.calibrate(p_enc2);

    inj_net.forward(p_enc3);
    inj_net.calibrate(p_enc3);
  }

  inj_net.reset_logits();

  inj_net.forward(p_enc1);
  std::cout << p_enc1.idx_to_char( idx_sampler.sample_top(inj_net.output()) );

  inj_net.reset_logits();

  inj_net.forward(p_enc2);
  std::cout << p_enc1.idx_to_char( idx_sampler.sample_top(inj_net.output()) );

  inj_net.reset_logits();  

  inj_net.forward(p_enc3);
  std::cout << p_enc1.idx_to_char( idx_sampler.sample_top(inj_net.output()) );
  
 
  std::cout << '\n';
  return 0;
}

/*
  encoder::packet<float> p_enc1(29);
  encoder::packet<float> p_enc2(29);
  encoder::packet<float> p_enc3(29);

  p_enc1.open_file("/home/andrew/Documents/cpp_projects/scratch/injector_network/sample_txt/char_test.txt");
  p_enc2.open_file("/home/andrew/Documents/cpp_projects/scratch/injector_network/sample_txt/char_test.txt");
  p_enc3.open_file("/home/andrew/Documents/cpp_projects/scratch/injector_network/sample_txt/char_test.txt");

  sampler::indexical idx_sampler(p_enc1.out_size(), 5);

  // network ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  injector::shallow<float, p_enc1.out_size(), logit::seq> inj_net(p_enc1.out_size(), 3);


  // injector::bilayer<float, p_enc1.out_size(), logit::dyn, logit::seq> inj_net(p_enc1.out_size(), 3);

  p_enc1.next_input();
  
  p_enc2.next_input(); p_enc2.next_input();
  p_enc3.next_input(); p_enc3.next_input(); p_enc3.next_input();

  p_enc1.display_vocab();

  p_enc1.display_trg();

  // std::cout << p_enc1.current_size() << '\n';

  for( auto& x : p_enc1 ) { std::cout << x.first << ' '; }

  std::cout << inj_net.output().size() << '\n';

 
  for( std::size_t epoch{0}; epoch < 2'000; ++epoch ){

    inj_net.forward(p_enc1);
    if(epoch % 100 == 0){ inj_net.display_output(); std::cout << '\n'; }
    inj_net.calibrate(p_enc1.get_trg());

    inj_net.forward(p_enc2);
    inj_net.calibrate(p_enc2.get_trg());

    inj_net.forward(p_enc3);
    inj_net.calibrate(p_enc3.get_trg());
    
  }
  

  // inj_net.reset_logits();

  // inj_net.forward(p_enc1);
  // std::cout << p_enc1.idx_to_char( idx_sampler.sample_top(inj_net.output()) );

  // inj_net.reset_logits();

  // inj_net.forward(p_enc2);
  // std::cout << p_enc1.idx_to_char( idx_sampler.sample_top(inj_net.output()) );

  // inj_net.reset_logits();  

  // inj_net.forward(p_enc3);
  // std::cout << p_enc1.idx_to_char( idx_sampler.sample_top(inj_net.output()) );

  // p_enc1.display_trg();
  // p_enc2.display_trg();
  // p_enc3.display_trg();

  // inj_net.display_formulas();


  for( std::size_t epoch{0}; epoch < 100'000; ++epoch ){

    if( epoch % 1'000 == 0 ){ 
  
      inj_net.reset_logits();
      
      for( std::size_t i{0}; i < 20; ++i ){
        
        p_enc.encode(test_str, ' ');

        inj_net.forward(p_enc);

        new_char = p_enc.idx_to_char( idx_sampler.sample_top(inj_net.output()) ); 

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
      p_enc.next_input();

      inj_net.forward(p_enc);

      inj_net.calibrate(p_enc.get_trg());
    }

  }


  // injector::shallow<float, 4, logit::dyn> inj_net(4, 3);
  
  // for( std::size_t epoch{0}; epoch < 2'000; ++epoch ){

  //   inj_net.forward(inp);

  //   if( epoch % 1'000 == 0 ){ inj_net.display_output(); }

  //   inj_net.calibrate(trg[0]);

  // }


  // splicer::character chr_spl(30, 1000);

  // encoder::one_hot<float, 30, encoder::fndm> p_enc;

  // std::ifstream ifs("/home/andrew/Documents/cpp_projects/scratch/injector_network/sample_txt/atlas_ref.txt");
  // std::ofstream ofs("/home/andrew/Documents/cpp_projects/scratch/injector_network/sample_txt/atlas_spl.txt");

  // chr_spl.splice_all(ifs, p_enc.get_vocab());

  // chr_spl.write_file(ofs);

  // chr_spl.display_up_to(4);

  // p_enc.display_vocab();
  
  // for(int epoch = 0; epoch < 10'000; ++epoch){

  //   b_1.forward(vec1);
  //   if( epoch % 100 == 0 ){ b_1.display_output(); }
  //   b_1.calibrate(vec2[0]);

  // }


*/
