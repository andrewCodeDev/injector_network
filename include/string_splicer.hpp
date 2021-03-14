#ifndef STR_SPLICER_H
#define STR_SPLICER_H

#include <algorithm>
#include <fstream>
#include <string>
#include <vector>

namespace splicer {

  class character {
    
    public:
      [[nodiscard]] explicit character( std::string inp_path, std::string out_path,  std::size_t size )              
        : window_size(size) {
        ifs.open(inp_path);
        ofs.open(out_path);
      }    

      bool splice_next(){ 

        if( std::getline(ifs, inp_str) ){

          if( inp_str.size() < windows_size ){
            for( std::size_t i{0}; i + window_size < inp_str.size(); ++i ){ strs.emplace_back(inp_str.substr(i, i + window)) }
          }

          else {
            strs.emplace_back(inp_str); 
          }

          return true;
        }

        else {
          return false;
        }

      }
      
      void shuffle(){
        std::random_shuffle(strs.begin(), strs.end());
      }

      void write_file(){ 
        for( const std::string& str : strs) { ofs << str << '\n'; }
      };

    private:
      std::ifstream ifs;
      std::ofstream ofs;
      std::string inp_str{""};
      std::size_t window_size{2};
      std::vector<std::string> strs;

  };

}

#endif