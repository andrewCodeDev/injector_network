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


// packet encoders ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


template <std::floating_point num> class packet {
    
    public:

      using packet_vec  = std::vector<std::pair<std::size_t, num>>;
      using sparse_vec  = std::vector<num>;

      packet( std::size_t example_size ){
        vocab[' '] = 0;
        for(const auto& c : numeric )       { vocab[c] = 0; }
        for(const auto& c : punctuation )   { vocab[c] = 0; }
        for(const auto& c : lower_alphabet ){ vocab[c] = 0; }
        for(const auto& c : upper_alphabet ){ vocab[c] = 0; }

        for( auto& p : vocab ){ p.second = cols++; }

        packets.resize(example_size);
        id_vec.resize(cols);
      }

      const std::pair<std::size_t, num>& cycle_inp() { 
        if( current < limit ){
          return packets[current++];
        }

        else {
          current = 1; return packets[0];
        }
      }

      std::size_t current_size() const { return limit; }

      constexpr std::size_t out_size(){ return 2 * case_size + nums_size + punc_size + 1; }

      const std::vector<num>& operator[]( const std::size_t i ) const { return packets[i]; }
            std::vector<num>& operator[]( const std::size_t i )       { return packets[i]; }

      
      std::size_t get_trg() { return target; }

      void display_inp()   const {
        std::cout << "| Input Characers | {'";
        for( std::size_t i{0}; i < limit; ++i) {
          std::cout << '(' << idx_to_char(packets[i].first) << ", " << packets[i].second << ") ";
        } std::cout << "\b}\n\n";
      }

      void display_trg()   const {
        std::cout << "| Target | {"<< target << ", '" << idx_to_char(target) << "'}\n\n";
      }

      void display_vocab() const {
        std::cout << "{ Lowercase Map }\n\n";
        for( auto& p : vocab ){
          std::cout << '\t' << p.first << " : " << p.second << '\n';
        } std::cout << '\n';
      }

      char idx_to_char( const std::size_t i ) const { return std::next(vocab.begin(), i)->first; }

      void open_file( const std::string& filepath ){
        
        ifs.open(filepath);

      }
            
      void encode( const std::string& inp ){

        // num uid = unique_id(inp);
         std::size_t i{0};

        while( i < inp.size() - 1 && i < packets.size() - 1 ){

          packets[i].first  = vocab[ inp[i] ];
          packets[i].second = 1;
        
          ++i;
        }

        target  = vocab[ inp[i] ];
        limit   = i;
        current = 0; 
      }

      bool next_input(){

        if( std::getline(ifs, inp_str) ){

          encode(inp_str);

          return true;
        }

        else {

          return false;

        }
      }

      auto begin() const { return packets.begin(); }
      auto begin()       { return packets.begin(); }
      auto end()   const { return packets.begin() + limit; }
      auto end()         { return packets.begin() + limit; }

    private:

      num unique_id( const std::string& str ){

        std::fill(id_vec.begin(), id_vec.end(), (num)0);

        for(const auto& c : str){ id_vec[ vocab[c] ] += (num)1; }

        num negative = -std::accumulate(id_vec.rbegin(), id_vec.rend(), (num)0, [incr = (num)1](const auto& x, const auto& y) mutable { return x + (y / incr++); });
        num positive =  std::accumulate(id_vec.cbegin(), id_vec.cend(), (num)0, [incr = (num)1](const auto& x, const auto& y) mutable { return x + (y / incr++); });

        return negative + positive; 

      }

      std::ifstream ifs; 
      std::string inp_str{""};

      encodings   vocab;
      sparse_vec  id_vec;
      packet_vec  packets;
      std::size_t current{0}, limit{0}, cols{0}, target{0};
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

      template <class logit_arr>
      std::size_t sample_top_n( const logit_arr& logits ){

          sort_indexes(logits);

          return indexes[distribution(gen)];
      }

      template <class logit_arr>
      std::size_t sample_top( const logit_arr& logits ){

        return (std::size_t)std::distance(logits.begin(), std::max_element(logits.begin(), logits.end()));
      }


    private:

      template <class logit_arr>
      void sort_indexes( const logit_arr& logits ){

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
