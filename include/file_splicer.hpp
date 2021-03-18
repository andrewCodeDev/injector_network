#ifndef STR_SPLICER_H
#define STR_SPLICER_H

#include <algorithm>
#include <fstream>
#include <string>
#include <vector>
#include <iostream>

#include "encodings.hpp"

namespace splicer {

  class character {
    public:
      [[nodiscard]] explicit character( std::size_t win_size, std::size_t buff_size ) : window_size(win_size), buffer_size(buff_size) { }    

      void splice_all( std::ifstream& ifs, const encoder::encodings& enc ){
        while( std::getline(ifs, inp_str) ){ if( check_vocab(inp_str, enc) ){ create_splices(inp_str); } }
      }

/*
      void splice_buffer( std::ifstream& ifs, const encodings& enc ){

        strs.clear();
        
        while( std::getline(ifs, inp_str) ){

          if ( current + window_size * sizeof(std::string::value_type) < buffer_size ){
            
            if( check_vocab() ){ create_splices(inp_str); }
            continue;

          };

          break;
        }
      }
*/

      void write_file( std::ofstream& ofs ){ 
        
        this->shuffle();

        for( const std::string& str : strs) { ofs << str << '\n'; }
      }

      void display_up_to( std::size_t n ){
       
        for(std::size_t i{0}; i < n; ++i){
          std::cout << strs[i] << '\n';
        }

      }
   
    private:
      void create_splices( const std::string& str ){

        if( 0 < inp_str.size() ){
          for( std::size_t i{0}; i + window_size < inp_str.size(); ++i ){ this->strs.emplace_back(inp_str.substr(i, window_size)); }

        } else {
          this->strs.emplace_back(inp_str); 
        }
      }

      void shuffle(){
        std::random_shuffle(strs.begin(), strs.end());
      }

      bool check_vocab( const std::string& str, const encoder::encodings& enc ){

        for( const auto& c : str ) {

          if(!enc.contains(c)) { return false; }

        }

        return true;
      }

      std::string inp_str{""};
      std::vector<std::string> strs;
      std::size_t window_size{2}, buffer_size{0}, current{0};
  };

}

#endif