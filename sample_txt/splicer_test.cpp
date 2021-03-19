#include <bits/stdc++.h>

#include "../include/file_splicer.hpp"
#include "../include/encodings.hpp"


int main(void){

  encoder::packet<float> pkt(60);


  std::ifstream ifs("/home/andrew/Documents/cpp_projects/scratch/injector_network/sample_txt/atlas_ref.txt");
  std::ofstream ofs("atlas_splice.txt");

  splicer::character str_spl(60, 1000);

  str_spl.splice_all(ifs, pkt.get_vocab());

  str_spl.write_file(ofs);

  str_spl.display_up_to(4);

  return 0;
}