#ifndef LOGITS_H
#define LOGITS_H

#include <functional>
#include <assert.h>
#include <iostream>
#include <iterator>
#include <vector>

#include "activation.hpp"

// immediate helper functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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


//sensor ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


namespace logit {

  struct imd { };
  struct seq { };

  template <std::floating_point num> class immediate;
  template <std::floating_point num> class sequential;

  template <
    std::floating_point num
  > class sensor {

  private:
    struct modifier { num coef{1}, rate{1}, center{0}; };

    std::vector<modifier> data; num signal{0};

    friend class immediate<num>;

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


  // types ~~~~~~~~~~


  template <
    std::floating_point num
  > class immediate {

  private:

    num stimuli{0}, error{0};
    std::vector<num> weights;
    std::vector<sensor<num>> sensors;

  public:
    immediate() = default;
    immediate( const std::size_t& num_s ){ sensors.resize(num_s); };

    immediate( std::size_t num_s, std::size_t s_size ){
      sensors.reserve(num_s); 
      weights.reserve(num_s); 
      while( 0 < num_s-- ){ 
        sensors.emplace_back(s_size);
        weights.emplace_back(rand_gen<num>());
      };
    };
  
    immediate& operator=(const immediate& other) = default;
    immediate& operator=(const num& x){ stimuli = x; return *this; }
    
    num send() const { return stimuli; }

    const sensor<num>& operator[](const std::size_t& i ) const { return sensors[i]; }
          sensor<num>& operator[](const std::size_t& i )       { return sensors[i]; }

    num operator()(){
      stimuli = std::inner_product(sensors.cbegin(), sensors.cend(), weights.cbegin(), static_cast<num>(0));
      return stimuli;
    }

    num receive( const std::size_t& i, const num& x ){ return sensors[i](x); }

    void add_sensor( std::size_t new_s, const std::size_t& n_terms ){
      while( 0 < new_s-- ){
        sensors.emplace_back(sensor<num>(n_terms));
        weights.emplace_back(rand_gen<num>());
      }; 
        sensors.shrink_to_fit();
        weights.shrink_to_fit();
    };

    void rmv_sensor( std::size_t rmv_s ){
      while( rmv_s > 0 ){ sensors.pop_back(); --rmv_s; }
    }

    template<class functor> void transform(const functor& f ){ stimuli = f(stimuli); }

    void add_error( const num& dx ){ error += dx; }
    void mul_error( const num& dx ){ error *= dx; }
    num  get_error(){ return error; }

    void calibrate( const std::size_t& i, const num& x, num scale = 1 ){
      
      sensors[i].calibrate(x, error * weights[i], scale); 
      weights[i] -= error * sensors[i].signal * (num)0.01;
      
    }

    num dy_dx( const std::size_t& i, const num& x ){ return sensors[i].dy_dx(x) * error; }

    void zero_error() { error = (num)0; }
    void show_error() { std::cout << "error: " << error << '\n'; }

    auto begin() const { return sensors.begin(); }
    auto begin()       { return sensors.begin(); }
    auto end()   const { return sensors.end(); }
    auto end()         { return sensors.end(); }

    auto cbegin() const { return sensors.cbegin(); }
    auto cend()   const { return sensors.cend();   }

    auto rbegin() { return sensors.rbegin(); }
    auto rend()   { return sensors.rend();   }

    std::size_t size() const { return sensors.size(); }

    friend bool operator==( const immediate& lhs, const immediate& rhs ){ return lhs.stimuli == rhs.stimuli; }
    friend bool operator!=( const immediate& lhs, const immediate& rhs ){ return lhs.stimuli != rhs.stimuli; }

    friend bool operator<=( const immediate& lhs, const immediate& rhs ){ return lhs.stimuli <= rhs.stimuli; }
    friend bool operator>=( const immediate& lhs, const immediate& rhs ){ return lhs.stimuli >= rhs.stimuli; }

    friend bool operator<( const immediate& lhs, const immediate& rhs ){ return lhs.stimuli < rhs.stimuli; }
    friend bool operator>( const immediate& lhs, const immediate& rhs ){ return lhs.stimuli > rhs.stimuli; }
  };

/*
  template <
    std::floating_point num
  > class sequential {

  private:

    std::vector<sensor<num>> sensors; num stimuli{0}, error{0};

  public:

    using reference = sequential&;

    sequential() = default;
    sequential( const std::size_t& num_s ){ sensors.resize(num_s); };

    sequential( std::size_t num_s, std::size_t s_size ){
      sensors.reserve(num_s); while( num_s > 0 ){ sensors.emplace_back(s_size); --num_s; };
    };
  
    reference operator=(const reference other) = default;
    reference operator=(const num& x){ stimuli = x; return *this; }
    
    num send() const { return stimuli; }

    const sensor<num>& operator[](const std::size_t& i ) const { return sensors[i]; }
          sensor<num>& operator[](const std::size_t& i )       { return sensors[i]; }

    num operator()(){
      stimuli = std::accumulate(sensors.cbegin(), sensors.cend(), static_cast<num>(0)) + (num)0.05 * stimuli;
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
    };

    void add_dx( const num& dx ){ error += dx; }
    void mul_dx( const num& dx ){ error *= dx; }
    num  get_dx(){ return error; }

    void calibrate( const std::size_t& i, const num& x, num scale = 1 ){ sensors[i].calibrate(x, error, scale); }

    num dy_dx( const std::size_t& i, const num& x ){ return sensors[i].dy_dx(x) * error; }

    void zero_error() { error = (num)0; }

    auto begin() const { return sensors.begin(); }
    auto begin()       { return sensors.begin(); }
    auto end()   const { return sensors.end(); }
    auto end()         { return sensors.end(); }

    auto cbegin() const { return sensors.cbegin(); }
    auto cend()   const { return sensors.cend();   }

    auto rbegin() { return sensors.rbegin(); }
    auto rend()   { return sensors.rend();   }

    std::size_t size() const { return sensors.size(); }

    friend bool operator==( const reference lhs, const reference rhs ){ return lhs.stimuli == rhs.stimuli; }
    friend bool operator!=( const reference lhs, const reference rhs ){ return lhs.stimuli != rhs.stimuli; }

    friend bool operator<=( const reference lhs, const reference rhs ){ return lhs.stimuli <= rhs.stimuli; }
    friend bool operator>=( const reference lhs, const reference rhs ){ return lhs.stimuli >= rhs.stimuli; }

    friend bool operator<( const reference lhs, const reference rhs ){ return lhs.stimuli < rhs.stimuli; }
    friend bool operator>( const reference lhs, const reference rhs ){ return lhs.stimuli > rhs.stimuli; }
  };

*/
}

#endif