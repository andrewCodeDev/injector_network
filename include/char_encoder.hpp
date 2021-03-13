#ifndef CHAR_ENC_H
#define CHAR_ENC_H

#include <cstdlib>
#include <fstream>
#include <string>
#include <map>

namespace encoder {

  static const std::string numeric{"0123456789"};
  static const std::string punctuation{".,!?;:-"};
  static const std::string upper_alphabet{"ABCDEFGHIJKLMNOPQRSTUVWXYZ"};
  static const std::string lower_alphabet{"abcdefghijklmnopqrstuvwxyz"};

  static const std::size_t case_size{26};
  static const std::size_t nums_size{10};
  static const std::size_t punc_size{8};

  struct full { full() = default; };
  struct lwrc { lwrc() = default; };

  template< std::floating_point num, std::size_t rows, typename vocab_t > class one_hot;

  template<
    std::floating_point num,
    std::size_t         rows
  > class one_hot< num, rows, encoder::lwrc >{
    
    const std::size_t cols = case_size + 1;
    
    public:
      using decoder_map = std::map<char, std::size_t>;
      using sparse_mtx  = std::array<std::vector<num>, rows>;
      using sparse_vec  = std::vector<num>;

      one_hot(){
        vocab[' '] = 0;
        for(const auto& c : numeric ){ vocab[c] = 0; }
        for(const auto& c : punctuation ){ vocab[c] = 0; }
        for(const auto& c : lower_alphabet ){ vocab[c] = 0; }

        std::size_t idx{0};

        for( auto& p : vocab ){ p.second = idx++; }

        for( sparse_vec& vec : encoded_mtx ) { vec.resize(cols); }
        for( sparse_vec& vec : loading_mtx ) { vec.resize(cols); }
        target.resize(cols);
      }

      const sparse_vec& cycle_inp() { 
        if( current < limit ){
          return encoded_mtx[current++];
        }

        else {
          current = 1; return encoded_mtx[0];
        }
      }
      const sparse_vec& get_trg() const { return target; }

      std::size_t current_size(){ return limit; }

      void display_inp()   const {
        std::cout << "| Encoded Matrix |\n\n";
        for( std::size_t i{0}; i < limit; ++i ){
          std::cout << '\t';
        for( std::size_t j{0}; j < cols;  ++j ){
          std::cout << encoded_mtx[i][j] << ' ';
        } std::cout << '\n';
        } std::cout << '\n';
      }

      void display_trg()   const {
        std::cout << "| Target Vector |\n\n\t";
        for( const float& x : target ){
          std::cout << x << ' ';
        } std::cout << "\n\n";
      }

      void display_vocab() const {
        std::cout << "{ Lowercase Map }\n\n";
        for( auto& p : vocab ){
          std::cout << '\t' << p.first << " : " << p.second << '\n';
        } std::cout << '\n';
      }

      void encode( const std::string& inp, char trg ){

        std::size_t tmp{ vocab[trg] };

        for( std::size_t i{0}; i < target.size(); ++i ){
          target[i] = (i == tmp);
        }

        for( std::size_t i{0}; i < inp.size(); ++i ){

          tmp = vocab[inp[i]];

          for( std::size_t j{0}; j < cols;     ++j ){
            encoded_mtx[i][j] = (j == tmp);
          }
        }
        current = 0; limit = inp.size();
      }

      void open_file(const std::string& filepath){
        
        ifs.open(filepath);

      }

      void load_input(){

        std::getline(ifs, inp_str);
        
        // write an assert here based on inp_string condition or ifs

        if( rows < inp_str.length() ) {
          encode(std::string(inp_str.c_str(), rows), inp_str[rows]);
        }

        else {
          encode(std::string(inp_str.c_str(), inp_str.size() - 1), inp_str.back());
        }
      }


    private:

      std::ifstream ifs; 
      std::string inp_str{""};

      decoder_map vocab;
      sparse_mtx encoded_mtx;
      sparse_mtx loading_mtx;
      sparse_vec target;
      std::size_t current{0}, limit{rows};
  };



  // template <std::floating_point float_type, std::size_t M, std::size_t N> class multi {

  // public:
  //   using sstream_iter = std::istream_iterator<float_type>;
  //   using memory_type  = numeric_memory<float_type>;
  //   using loader_array = std::array<memory_type, N>;

  //   multi(const std::string& filepath)
  //   { 
  //     ifs.open(filepath);
        
  //     inp_ss = next_stream(ifs, inp_str); trg_ss = next_stream(ifs, trg_str);

  //     assert(inp_str.size() % (M * N) == 0);

  //     for(memory_type& mem : inp) { mem.resize(N); }

  //     for(std::size_t i{0}, j{i}; i < M; ++i, ++j) {
  //       std::copy(std::execution::seq, sstream_iter(inp_ss) + i * N, sstream_iter(inp_ss) + j * N, inp[i].value.begin()); 
  //     } std::copy(std::execution::seq, sstream_iter(trg_ss),         sstream_iter(), std::back_inserter(trg.value));

  //     trg.value.shrink_to_fit();

  //     inp.deriv.resize(inp.value.size());
  //   };

  //   void operator()(){
  //     inp_ss = next_stream(ifs, inp_str); trg_ss = next_stream(ifs, trg_str);
      
  //     for(std::size_t i{0}, j{i}; i < M; ++i, ++j) {
  //       std::copy(std::execution::seq, sstream_iter(inp_ss) + i * N, sstream_iter(inp_ss) + j * N, inp[i].value.begin()); 
  //     } std::copy(std::execution::seq, sstream_iter(trg_ss),         sstream_iter(), trg.value.begin());
  //   }

  //   loader_array inp; memory_type trg;

  // private:
  //   trg_str{""}; std::istringstream inp_ss, trg_ss;
  // };



}

/*

  encoder::one_hot<float, 3, encoder::lwrc> o_h;

  o_h.display_vocab();

  o_h.encode("abc", 'd');

  o_h.display_inp();
  o_h.display_trg();

  for(int i = 0; i < 10; ++i){
    for(const auto& x : o_h.cycle_inp()) { std::cout << x << ' '; } std::cout << '\n';
  } std::cout << '\n';

  o_h.encode("ab", 'c');

  o_h.display_inp();
  o_h.display_trg();
    
  for(int i = 0; i < 10; ++i){
    for(const auto& x : o_h.cycle_inp()) { std::cout << x << ' '; } std::cout << '\n';
  }

*/

#endif
