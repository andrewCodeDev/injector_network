#include <functional>
#include <algorithm>
#include <execution>
#include <concepts>
#include <iterator>
#include <iostream>
#include <numeric>
#include <vector>
#include <assert.h>
#include <cmath>

// #include <ranges>


/*

  total number of paths where at all rows have at least one active logit:

    2^(r*c) - (sum|i=0...r-1|( r_C_i * [2^(c(r-i)) - 1] ) + 1);

*/

template <
  std::floating_point FT
> FT rand_gen(){ return (FT)rand() / (FT)RAND_MAX * (FT)2 - (FT)1; }


template <
  class T
> void standard_form( const T& poly_num ){

  for(std::size_t i{0}; i < poly_num.size(); ++i) {
    if( i == (poly_num.size() - 1)) {
      std::cout << poly_num[i].coef << "/(1 + e^(" << poly_num[i].rate << "(x - " << poly_num[i].center << ")))";
    } else {
      std::cout << poly_num[i].coef << "/(1 + e^(" << poly_num[i].rate << "(x - " << poly_num[i].center << "))) + ";
    }
  } std::cout << '\n';
}

template <std::floating_point num> class logit;

template <
  std::floating_point num
> class sensor {

private:
  struct modifier { num coef{1}, rate{1}, center{0}; };

  std::vector<modifier> data; num signal{0};

  friend class logit<num>;

public:
  sensor() = default;
  sensor(const size_t& size) : data(size){
    for( auto& m : data ){
      m.coef   = rand_gen<num>();
      m.rate   = rand_gen<num>();
      m.center = rand_gen<num>();
    }
  };
  
  const modifier& operator[](const std::size_t& i ) const { return data[i]; }
        modifier& operator[](const std::size_t& i )       { return data[i]; }

  num operator()( const num& x ){
    signal = std::transform_reduce(std::execution::seq, data.cbegin(), data.cend(), static_cast<num>(0), std::plus<num>{},
      [&x](const modifier& m)
      {
        return m.coef / (static_cast<num>(1) + std::exp(m.rate * (x - m.center)));
      });
    return signal;
  }

  sensor& operator++(){ data.emplace_back( modifier{} ); return *this; }
  sensor& operator--(){ data.pop_back(); return *this; }
  
  // using friend function for commutative property
  friend num operator+( const num& x, const sensor& s ) { return x + s.signal; }
  friend num operator-( const num& x, const sensor& s ) { return x - s.signal; }
  friend num operator*( const num& x, const sensor& s ) { return x * s.signal; }
  friend num operator/( const num& x, const sensor& s ) { return x / s.signal; }

  friend num operator+( const sensor& s, const num& x ) { return s.signal + x; }
  friend num operator-( const sensor& s, const num& x ) { return s.signal - x; }
  friend num operator*( const sensor& s, const num& x ) { return s.signal * x; }
  friend num operator/( const sensor& s, const num& x ) { return s.signal / x; }

  bool valid() { return 0 < data.size(); }

  auto begin() const { return data.begin(); }
  auto begin()       { return data.begin(); }
  auto end()   const { return data.end(); }
  auto end()         { return data.end(); }

  auto cbegin() const { return data.cbegin(); }
  auto cend()   const { return data.cend();   }

  auto rbegin() { return data.rbegin(); }
  auto rend()   { return data.rend();   }

  std::size_t size() const { return data.size(); }

  num dy_dx( const num& x, std::size_t n_drv = 1 ){

    assert( 0 < n_drv && n_drv < 3 );

    if( n_drv == 1 ) {

      return std::transform_reduce(data.cbegin(), data.cend(), (num)0, std::plus<num>{},
        [&x](const modifier& m) 
        { 
          num e_p = std::exp(m.rate * (x - m.center));
          return -(m.coef * m.rate * e_p) / (((num)1 + e_p) * ((num)1  + e_p));
        });
    }
    
    if( n_drv == 2 ) {

      return std::transform_reduce(data.cbegin(), data.cend(), (num)0, std::plus<num>{}, 
        [&x](const modifier& m)
        { 
          num e_p = std::exp(m.rate * (x - m.center));
          return -(m.coef * m.rate * m.rate * e_p * (-e_p + (num)1)) / (((num)1 + e_p) * ((num)1 + e_p) * ((num)1 + e_p));
        });
      }

      return 0.0f; // otherwise it complains
    }

  void calibrate( const num& x, const num& error, num scale = 1.0, num lr = 0.01 ){

    num one{1}, adj{error * lr}, e_p, denom;

      // NOTE! derivative with repect to c is the negation of the derivative with respect to x
      
    for( modifier& m : data ) {
      e_p = std::exp(m.rate * (x - m.center)); 
      denom = (one + e_p) * (one + e_p);

      m.rate   -= scale * ((-m.coef * (x - m.center) * e_p ) / denom ) * adj;
      m.center -= scale * (( m.coef * m.rate * e_p ) / denom ) * adj;
      m.coef   -= scale * (one / (one + e_p)) * adj;
    }
  }

};


//~~~~~~~~~~~~~~~~~~~~~~~


template <
  std::floating_point num
> class logit {

private:

  std::vector<sensor<num>> sensors; num stimuli{0}, error{0}; bool state{false};

public:
  logit() = default;
  logit( const std::size_t& num_s ){ sensors.resize(num_s); };

  logit( std::size_t num_s, std::size_t s_size ){
    sensors.reserve(num_s); while( num_s > 0 ){ sensors.emplace_back(s_size); --num_s; };
   };
  logit& operator=(const logit& other) = default;  
  
  num send() const { return stimuli; }

  const sensor<num>& operator[](const std::size_t& i ) const { return sensors[i]; }
        sensor<num>& operator[](const std::size_t& i )       { return sensors[i]; }

  num operator()(){
    stimuli = std::accumulate(sensors.cbegin(), sensors.cend(), static_cast<num>(0)); //+ (num)0.05 * stimuli;
    state   = (static_cast<num>(0) < stimuli);
    return stimuli;
  }

  num receive( const std::size_t& i, const num& x ){ return sensors[i](x); }

  void add_sensor( std::size_t new_s, const std::size_t& n_terms ){
    while( 0 < new_s-- ){ sensors.emplace_back(sensor<num>(n_terms)); }; sensors.shrink_to_fit();
  };

  void rmv_sensor( std::size_t rmv_s ){
    while( rmv_s > 0 ){ sensors.pop_back(); --rmv_s; }
  }

  void initialize( std::size_t new_s, const std::size_t& n_terms ) {
    add_sensor(new_s, n_terms);
    state = false; // state only becomes true if external stimulation is greater than 0
  };

  void add_dx( const num& dx ){ error += dx; }

  void calibrate( const std::size_t& i, const num& x, num scale = 1 ){ sensors[i].calibrate(x, error, scale); }

  num dy_dx( const std::size_t& i, const num& x ){ return sensors[i].dy_dx(x) * error; }

  void zero_error() { error = (num)0.0; }

  bool valid() const { return 0 < sensors.size(); }
  bool is_on() const { return state; }

  auto begin() const { return sensors.begin(); }
  auto begin()       { return sensors.begin(); }
  auto end()   const { return sensors.end(); }
  auto end()         { return sensors.end(); }

  auto cbegin() const { return sensors.cbegin(); }
  auto cend()   const { return sensors.cend();   }

  auto rbegin() { return sensors.rbegin(); }
  auto rend()   { return sensors.rend();   }

  logit& operator=(const num& x){ stimuli = x; return *this; }

  std::size_t size() const { return sensors.size(); }
};


//~~~~~~~~~~~~~~~~~~~~~~~


namespace grid {

  template<std::floating_point num> void softmax(auto& cntr){
    num denominator = std::transform_reduce( std::execution::seq, 
      cntr.cbegin(), cntr.cend(), static_cast<num>(0), std::plus<num>{}, [](const logit<num>& l_pos) { return std::exp(l_pos.send()); }
    );
    
    std::transform( std::execution::seq,
      cntr.cbegin(), cntr.cend(), cntr.begin(), [&denominator](const logit<num>& l_pos){ return std::exp(l_pos.send()) / denominator; }
    );
  }

  template <
    std::floating_point num,
    std::size_t         rows,
    std::size_t         cols
  > class symmetric {

  private:

    std::vector<num> inp_memo;
    std::array<std::vector<logit<num>>, rows> logits;

  public:

    symmetric( const std::size_t& inp_size, const std::size_t& out_size, const std::size_t& n_terms ){
      initialize(inp_size, out_size, n_terms);
    };

    const std::array<logit<num>, cols>& output() const { return logits.back(); }
          std::array<logit<num>, cols>& output()       { return logits.back(); }

    std::size_t row()  const { return rows; }
    std::size_t col()  const { return cols; }
    std::size_t size() const { return logits.size(); }

    template<class container> bool is_output(){
      return std::any_of(logits.back().cbegin(), logits.back().cend(), []( const auto& l_pos ){ return l_pos.is_on(); });
    }

    logit<num>& operator()( const std::size_t& i, const std::size_t& j ) { return logits[i][j]; }
    std::array<logit<num>, cols>&     operator[]( const std::size_t& i ) { return logits[i];    }

    template<class container>
    void forward( const container& inp ){

      std::transform(inp.cbegin(), inp.cend(), inp_memo.cbegin(), inp_memo.begin(), std::plus<num>{});
      
      std::size_t s_idx{0};
      for( logit<num>& l_pos : logits.front() ){
      for( const num&  inp_v : inp )
      {
        l_pos.receive(s_idx++, inp_v); 
      } 
        l_pos(); s_idx = 0;
      }

      for( std::size_t i{1}; i < rows; ++i ){
      for( logit<num>& l_pos : logits[i]   ){
        for( logit<num>& l_inp : logits[i-1] )
        {    
          l_pos.receive(s_idx++, l_inp.send());
        }
          l_pos(); s_idx = 0;
        }
      }
    }


    // template<class container>
    // void forward_with_state( const container& inp ){

    //   std::transform(inp.cbegin(), inp.cend(), inp_memo.cbegin(), inp_memo.begin(), std::plus<num>{});
      
    //   std::size_t s_idx{0};
    //   for( logit<num>& l_pos : logits.front() ){
    //   for( const num&  inp_v : inp )
    //   {
    //     l_pos.receive(s_idx++, inp_v); 
    //   } 
    //     l_pos(); s_idx = 0;
    //   }

    //   for( std::size_t i{1}; i < rows; ++i ){
    //   for( logit<num>& l_pos : logits[i]   ){

    //     if( !l_pos.is_on() ){ continue; }

    //     for( logit<num>& l_inp : logits[i-1] )
    //     {    
    //       if( !l_inp.is_on() ){ l_pos.receive(s_idx++, l_inp.send()); }
    //       l_pos(); s_idx = 0;
    //     }
    //     }
    //   }
    // }

    template<class container>
    void calibrate( const container& trg ){

      std::size_t s_idx{0};
      for( logit<num>& l_pos : logits.back() )
      for(  const num& v_trg : trg ){

        l_pos.add_dx( l_pos.send() - v_trg );
      }

      for( std::size_t i{rows-1}; 0 < i; --i )
      for( logit<num>& l_pos : logits[i]     ){
      for( logit<num>& l_inp : logits[i-1] ){
        l_inp.add_dx( l_pos.dy_dx(s_idx, l_inp.send()) );
        l_pos.calibrate(s_idx, l_inp.send());
        ++s_idx;
      }
        s_idx = 0; l_pos.zero_error();
      }

      for( logit<num>& l_pos : logits.front() ){
      do{
        l_pos.calibrate(s_idx, (num)0.0, (num)cols - inp_memo[s_idx]);
        l_pos.calibrate(s_idx, (num)1.0,             inp_memo[s_idx]);
      }while( ++s_idx < inp_memo.size() );
        s_idx = 0; l_pos.zero_error();
      }

      inp_memo.clear();

    }

    template<std::unsigned_integral unum>
    void initialize( const unum& inp_size, const std::size_t& out_size, const std::size_t& n_terms ){

      inp_memo.resize(inp_size);

        logits.front().resize(cols);
      for( logit<num>& l_pos : logits.front() ) { l_pos.initialize(inp_size, n_terms); }
      
      for( std::size_t i{1}; i < rows - 1; ++i ){
          logits[i].resize(cols);
        for( logit<num>& l_pos : logits[i] ) { l_pos.initialize(cols, n_terms); }
      }
        
      if(1 < rows * cols ) {
          logits.back().resize(out_size);
        for( logit<num>& l_pos : logits.back() ) { l_pos.initialize(out_size, n_terms); }
      }
    }

    void display_states(){
      for( const auto& row : logits ){
      for( const auto& pos : row  ){
        std::cout << pos.is_on() << ' ';
      } std::cout << '\n';
      } std::cout << '\n';
    }

    void display_values(){
      for( const auto& row : logits ){
      for( const auto& pos : row  ){
        std::cout << pos.send() << ' ';
      } std::cout << '\n';
      } std::cout << '\n';
    }

    void display_formulas(){
      for( const auto& row : logits )
      for( const auto& log : row  ){
      for( const auto& pos : log  ){
        standard_form(pos);
      } std::cout << '\n';
      } std::cout << '\n';
    }

  };

}

int main(void){

  std::vector<std::vector<float>> mtx {{1, 1},{1, 0},{0, 1},{0, 0}};
  std::vector<std::vector<float>> trg {{0, 1},{1, 0},{1, 0},{0, 1}};

  // std::vector<std::vector<float>> mtx {{1}, {0}};
  // std::vector<std::vector<float>> trg {{1}, {0}};

  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  grid::symmetric<float, 2, 1> l_grid(2, 2, 3);


  // for(auto& vec : mtx ){ std::generate(vec.begin(), vec.end(), rand_gen<float>); }

  l_grid.display_formulas();

  std::cout << l_grid.size() << "\n\n";

  for( std::size_t epoch{0}; epoch < 4'000; ++epoch)
  for( std::size_t i{0}; i < mtx.size();   ++i ){
      l_grid.forward(mtx[i]);
      l_grid.calibrate(trg[i]);
      
      if(epoch % 400 == 0){ l_grid.display_values(); }
  }

  l_grid.forward(mtx[0]);

  l_grid.display_values();
  l_grid.display_formulas();
  

  std::cout << '\n';
  return 0;
}

/*
  std::size_t rows = 2, s_idx{0};

  for( logit<float>& l_pos : grid.front() ){
  for( const float&  inp_v : vec)
  {
    l_pos.receive(s_idx++, inp_v); 
  } 
    l_pos(); s_idx = 0;
  }

  for( std::size_t i{1}; i < rows; ++i )
  for( logit<float>& l_pos : grid[i]   ){
  for( logit<float>& l_inp : grid[i-1] )
  {    
    if( l_inp.is_on() ){ l_pos.receive(s_idx++, l_inp.send()); }
  }
    l_pos(); s_idx = 0;
  }

  for(auto& row : grid){
  for(auto& pos : row ){
    std::cout << pos.is_on() << ' ';
  } std::cout << '\n';
  }

  for(auto& row : grid){
  for(auto& pos : row ){
    std::cout << pos.send() << ' ';
  } std::cout << '\n';
  }
*/