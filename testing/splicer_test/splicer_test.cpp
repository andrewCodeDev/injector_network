#include "../../include/file_splicer.hpp"
#include <bits/stdc++.h>

int main(void){

  splicer::character str_spl(30, 1000);

  std::ifstream ifs("atlas_ref.txt");
  std::ofstream ofs("atlas_splice.txt", std::ios::app);

  str_spl.splice_all(ifs);

  str_spl.write_file(ofs);

  str_spl.display_up_to(4);

  return 0;
}