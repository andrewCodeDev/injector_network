#ifndef CHAR_ENC_H
#define CHAR_ENC_H

#include <cstdlib>
#include <string>
#include <map>

template<
  std::floating_point num,
  std::size_t         rows,
  std::size_t         cols
> class one_hot {
public:

  using decoder_map = std::map<char, std::size_t>;
  using sparse_mtx  = std::array<std::vector<num>, rows>;
  using sparse_vec  = std::vector<num>;

  one_hot(){
    
    for(std::size_t i{0}; i < 10; ++i){
      full_map [fundamental[i]] = i;
      lower_map[fundamental[i]] = i;
    }

    for( std::size_t i{0}, pos{10}; i < 10; ++i, ++pos; ){
      full_map [numeric[i]] = pos;
      lower_map[numeric[i]] = pos;
    }
 
    for( std::size_t i{0}, pos{20}; i < alphabet.size(); ++i, ++pos; ){
      
      full_map[alphabet[i]]    = pos;

      if( std::is_lower(alphabet[i]) ){
        lower_map[alphabet[i]] = pos;
      }
    }

    for( sparse_vec& vec : encoded_mtx ) { vec.resize(cols); }
  }

  const sparse_vec& cycle_next() const { 
    if( current < limit ){
      return encoded_mtx[current++] 
    }

    else {
      current = 1; return encoded_mtx[0];
    }
  }

  std::size_t size(){ return limit; }

  private:
    inline static const std::string fundamental{" .,!?;:-()"};
    inline static const std::string alphabet{"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"};
    inline static const std::string numeric{"0123456789"};

    decoder_map full_map, lower_map;
    sparse_mtx encoded_mtx;

    std::size_t current{0}, limit{rows};
}
  void display_mtx()   const{
    std::cout << "| Encoded Matrix |\n";
    for( std::size_t i{0}; i < rows; ++i ){
      std::cout << '\t';
    for( std::size_t j{0}; j < rows; ++j ){
      std::cout << encoded_mtx[i][j] << ' ';
    } std::cout << '\n';
    } std::cout << '\n';
  }

  void display_full()  const {
    std::cout << "| Full Map |\n";
    for( auto& p : full_map ){
      std::cout << '\t' << p.first << " : " << p.second << '\n';
    } std::cout << '\n';
  }

  void display_lower() const {
    std::cout << "| Lowercase Map |\n";
    for( auto& p : lower_map ){
      std::cout << '\t' << p.first << " : " << p.second << '\n';
    } std::cout << '\n';
  }

  void full_encode( const std::string& str ){

    std::size_t tmp; current = 0;

    for( std::size_t i{0}; i < rows; ++i ){

      if( i < str.size() ){
        tmp = full_map[str[i]];
      }

      else{
        limit = i; break;
      }

      for( std::size_t j{0}; j < cols; ++j ){
        encoded_mtx[i][j] = (num)(j == tmp);
      } 
    }

    limit = rows;
  }

};


#endif