#ifndef CHAR_ENC_H
#define CHAR_ENC_H

#include <cstdlib>
#include <fstream>
#include <string>
#include <random>
#include <map>

#include <iomanip>

namespace encoder {


  template <std::floating_point num> void normalize_uid(auto& arr){

    num min_id = (*std::min_element(arr.cbegin(), arr.cend()));
    num max_id = (*std::max_element(arr.cbegin(), arr.cend()));
    num rcp    = static_cast<num>(1) / (max_id - min_id + (num)1e-8);

    for( size_t i{0}; i < arr.size(); ++i ) {
      arr[i] = (num)1 - (arr[i] - min_id) * rcp;
    }
  }

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

      packet( std::size_t example_size, std::string filepath ){
     
        vocab[' '] = 0;
        for(const auto& c : numeric )       { vocab[c] = 0; }
        for(const auto& c : punctuation )   { vocab[c] = 0; }
        for(const auto& c : lower_alphabet ){ vocab[c] = 0; }
        for(const auto& c : upper_alphabet ){ vocab[c] = 0; }

        for( auto& p : vocab ){ p.second = cols++; }

        packets.resize(example_size);
        id_vec.resize(cols);

        open_file(filepath);
      }

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

      inline static const size_t out_size { 2 * case_size + nums_size + punc_size + 1 };
      
      std::size_t get_trg() const { return packets[limit - 1].first; }
      
      void display_inp()   const {
        std::cout << "| Input Characers | {'";
        for( std::size_t i{0}; i < limit - 1; ++i) {
          std::cout << '(' << idx_to_char(packets[i].first) << ", " << packets[i].second << ") ";
        } std::cout << "\b}\n\n";
      }

      void display_trg()   const {
        std::cout << "| Target | {'" << idx_to_char(get_trg()) << "', " << get_trg()  << "}\n\n";
      }

      void display_vocab() const {
        std::cout << "{ Lowercase Map }\n\n";
        for( auto& p : vocab ){
          std::cout << '\t' << p.first << " : " << p.second << '\n';
        } std::cout << '\n';
      }

      char idx_to_char( const std::size_t i ) const { return std::next(vocab.begin(), i)->first; }

      void open_file( const std::string& filepath ){
        
        nomralize_characters(filepath);
        ifs.open(filepath);

      }
            
      void encode( const std::string& inp ){

        std::size_t i{0};

        num num_words = std::count(inp.begin() + 1, inp.end() - 1, ' ');

        std::string::const_iterator ancr = inp.begin();
        std::string::const_iterator incr = inp.begin();

        do{

          incr = std::find(incr, inp.end(), ' ');

          size_t dist = std::distance(ancr, incr);

          while( i < std::distance(inp.begin(), incr) && i < inp.size() && i < packets.size() ){
         
            packets[i].first  = vocab[ inp[i] ];
            packets[i].second = nrm_uid[ vocab[inp[i]] ] * (num)dist / num_words;

            ++i;
          }
          
          ancr = incr++;
          
        }while( 0 < std::distance(incr, inp.end()) );

        limit   = i;
        current = 0; 

      }


      packet& next_input(){

        if( std::getline(ifs, inp_str) ){ encode(inp_str); }
        
        return *this;
      }

      operator bool() { return ifs.good(); }

      auto operator[]( const std::size_t i ) const { return packets[i]; }
      auto operator[]( const std::size_t i )       { return packets[i]; }
      
      auto begin() const { return packets.begin(); }
      auto begin()       { return packets.begin(); }
      auto end()   const { return packets.begin() + limit; }
      auto end()         { return packets.begin() + limit; }

    private:

      void nomralize_characters( std::string str ){

        std::ifstream uid_fs; uid_fs.open(str);
        
        for( size_t i{0}; i < 10'000 && uid_fs; ++i ){
          
          std::getline(uid_fs, str);
          
          for( auto& c : str ) { nrm_uid[ vocab[c] ] += 1; }

        }

        normalize_uid<num>(nrm_uid);

        // for( auto& c : nrm_uid ) { std::cout << c << ' '; }
      }

      std::ifstream ifs; 
      std::string inp_str{""};

      encodings   vocab;
      sparse_vec  id_vec;
      packet_vec  packets;
      std::size_t current{0}, limit{0}, cols{0};

      std::array<num, out_size> nrm_uid{0};
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
