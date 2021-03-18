#ifndef CHAR_ENC_H
#define CHAR_ENC_H

#include <cstdlib>
#include <fstream>
#include <string>
#include <random>
#include <map>


namespace encoder {

  static const std::string numeric{"0123456789"};
  static const std::string punctuation{"().,!?;:-"};
  static const std::string upper_alphabet{"ABCDEFGHIJKLMNOPQRSTUVWXYZ"};
  static const std::string lower_alphabet{"abcdefghijklmnopqrstuvwxyz"};

  static const std::size_t case_size{26};
  static const std::size_t nums_size{10};
  static const std::size_t punc_size{9};

  struct cust { cust() = default; };
  struct fndm { fndm() = default; };
  struct lwrc { lwrc() = default; };

  using encodings = std::map<char, std::size_t>;

  template <std::floating_point num, std::size_t rows, typename vocab_t> class one_hot;

  template <
    std::floating_point num,
    std::size_t         rows
  > class one_hot< num, rows, encoder::lwrc >{
    
    const std::size_t cols = case_size + 1;
    
    public:
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

      const std::vector<num>& operator[]( const std::size_t i ) const { return encoded_mtx[i]; }
            std::vector<num>& operator[]( const std::size_t i )       { return encoded_mtx[i]; }

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

      bool next_input(){

        if( std::getline(ifs, inp_str) ){

          if( rows < inp_str.length() ) {
            encode(std::string(inp_str.c_str(), rows), inp_str[rows]);
          }

          else {
            encode(std::string(inp_str.c_str(), inp_str.size() - 1), inp_str.back());
          }

          return true;
        }

        else {
          return false;
        }
      }

    private:
      std::ifstream ifs; 
      std::string inp_str{""};

      encodings   vocab;
      sparse_mtx  encoded_mtx;
      sparse_mtx  loading_mtx;
      sparse_vec  target;
      std::size_t current{0}, limit{rows};
  };


template <
    std::floating_point num,
    std::size_t         rows
  > class one_hot< num, rows, encoder::fndm >{
    
    public:
    
      using sparse_mtx  = std::array<std::vector<num>, rows>;
      using sparse_vec  = std::vector<num>;

      one_hot(){
        vocab[' '] = 0;
        for(const auto& c : numeric ){ vocab[c] = 0; }
        for(const auto& c : punctuation ){ vocab[c] = 0; }
        for(const auto& c : lower_alphabet ){ vocab[c] = 0; }
        for(const auto& c : upper_alphabet ){ vocab[c] = 0; }

        std::size_t idx{0};

        for( auto& p : vocab ){ p.second = idx++; }

        cols = --idx;

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

      std::size_t current_size() const { return limit; }

      constexpr std::size_t out_size(){ return 2 * case_size + nums_size + punc_size; }

      const std::vector<num>& operator[]( const std::size_t i ) const { return encoded_mtx[i]; }
            std::vector<num>& operator[]( const std::size_t i )       { return encoded_mtx[i]; }

      const encodings& get_vocab() { return vocab; }

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

      char idx_to_char( const std::size_t i ) { return std::next(vocab.begin(), i)->first; }

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

      bool next_input(){

        if( std::getline(ifs, inp_str) ){

          if( rows < inp_str.size() ) {

            std::cout << inp_str << '\n';
            std::cout << inp_str.substr(0, rows) << '\n';
            std::cout << inp_str[rows] << '\n';

            encode(inp_str.substr(0, rows), inp_str[rows]);
          }

          else {
            encode(std::string(inp_str.c_str(), inp_str.size() - 1), inp_str.back());
          }

          return true;
        }

        else {
          return false;
        }
      }

    private:
      std::ifstream ifs; 
      std::string inp_str{""};

      encodings   vocab;
      sparse_mtx  encoded_mtx;
      sparse_mtx  loading_mtx;
      sparse_vec  target;
      std::size_t current{0}, limit{rows}, cols{0};
  };


  template <
    std::floating_point num,
    std::size_t         rows
  > class one_hot< num, rows, encoder::cust >{
    
    const std::size_t cols = case_size + 1;

    public:
      using sparse_mtx  = std::array<std::vector<num>, rows>;
      using sparse_vec  = std::vector<num>;

      one_hot(std::string str){
        
        ifs.open(str);

        char c;
        while(ifs.get(c)){ if(!vocab.contains(c)){ vocab[c] = 0; } }
        
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

      const std::vector<num>& operator[]( const std::size_t i ) const { return encoded_mtx[i]; }
            std::vector<num>& operator[]( const std::size_t i )       { return encoded_mtx[i]; }

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
        std::cout << "{ Custom Map }\n\n";
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

      bool next_input(){

        if( std::getline(ifs, inp_str) ){

          if( rows < inp_str.length() ) {
            encode(std::string(inp_str.c_str(), rows), inp_str[rows]);
          }

          else {
            encode(std::string(inp_str.c_str(), inp_str.size() - 1), inp_str.back());
          }

          return true;
        }

        else {
          return false;
        }
      }

    private:
      std::ifstream ifs; 
      std::string inp_str{""};

      encodings   vocab;
      sparse_mtx  encoded_mtx;
      sparse_mtx  loading_mtx;
      sparse_vec  target;
      std::size_t current{0}, limit{rows};
  };

}

namespace sampler {
  
  class indexical {
    public:

      [[nodiscard]] explicit indexical( std::size_t num_idxs, std::size_t samp_size ) :
        indexes(num_idxs, 0), distribution(0, samp_size - 1) { };

      const std::size_t& operator[]( const std::size_t i ) const { return indexes[i]; }
            std::size_t& operator[]( const std::size_t i )       { return indexes[i]; }

      void display_idxs() const {
        std::cout << "| Indexes | ";
        for( const std::size_t& x : indexes ){
          std::cout << x << ' ';
        } std::cout << "\n\n";
      }

      template <class logit_t>
      std::size_t sample_top( const std::vector<logit_t> &logits ){

          sort_indexes(logits);

          return indexes[distribution(gen)];
      }

    private:

      template <class logit_t>
      void sort_indexes( const std::vector<logit_t> &logits ){

        std::iota(indexes.begin(), indexes.end(), 0);

        std::stable_sort(
          indexes.rbegin(), indexes.rend(), [&logits](const std::size_t& i1, const std::size_t& i2) {return logits[i1] < logits[i2]; }
        );
      }

      std::vector<std::size_t> indexes;

      std::mt19937 gen;

      std::uniform_int_distribution<std::size_t> distribution;
  };

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
