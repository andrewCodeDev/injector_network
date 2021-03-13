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

template <std::floating_point num> class immediate;
template <std::floating_point num> class sequential;

}

namespace sensor {

  template <
    std::floating_point num
  > class IMDsensor {

    private:

      struct modifier { num coef{1}, rate{1}, center{0}; };

      std::vector<modifier> data; 
      
      num signal{0};

      friend class logit::immediate<num>;

    public:
      IMDsensor() = default;
      IMDsensor(const size_t& size) : data(size){
        for( auto& m : data ){
          m.coef   = rand_gen<num>();
          m.rate   = rand_gen<num>();
          m.center = rand_gen<num>();
        }
      };
      
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

      void calibrate( const num& x, const num& error, num lr = 0.01 ){

        num one{1}, adj{error * lr}, e_p, denom;

          // NOTE! derivative with repect to c is the negation of the derivative with respect to x
          
        for( modifier& m : data ) {
          e_p = std::exp(m.rate * (x - m.center)); 
          denom = (one + e_p) * (one + e_p);

          m.rate   -=  ((-m.coef * (x - m.center) * e_p ) / denom ) * adj;
          m.center -=  (( m.coef * m.rate * e_p ) / denom ) * adj;
          m.coef   -=  (one / (one + e_p)) * adj;
        }
      }

      num operator()( const num& x ){
        signal = std::transform_reduce(std::execution::seq, data.cbegin(), data.cend(), static_cast<num>(0), std::plus<num>{},
          [&x](const modifier& m)
          {
            return m.coef / (static_cast<num>(1) + std::exp(m.rate * (x - m.center)));
          });
        return signal;
      }

      const modifier& operator[](const std::size_t& i ) const { return data[i]; }
            modifier& operator[](const std::size_t& i )       { return data[i]; }

      IMDsensor& operator++(){ data.emplace_back( modifier{} ); return *this; }
      IMDsensor& operator--(){ data.pop_back(); return *this; }
      
      // using friend function for commutative property

      auto begin() const { return data.begin(); }
      auto begin()       { return data.begin(); }
      auto end()   const { return data.end(); }
      auto end()         { return data.end(); }

      auto cbegin() const { return data.cbegin(); }
      auto cend()   const { return data.cend();   }

      auto rbegin() { return data.rbegin(); }
      auto rend()   { return data.rend();   }

      std::size_t size() const { return data.size(); }

      friend num operator+( const num& x, const IMDsensor& s ) { return x + s.signal; }
      friend num operator-( const num& x, const IMDsensor& s ) { return x - s.signal; }
      friend num operator*( const num& x, const IMDsensor& s ) { return x * s.signal; }
      friend num operator/( const num& x, const IMDsensor& s ) { return x / s.signal; }

      friend num operator+( const IMDsensor& s, const num& x ) { return s.signal + x; }
      friend num operator-( const IMDsensor& s, const num& x ) { return s.signal - x; }
      friend num operator*( const IMDsensor& s, const num& x ) { return s.signal * x; }
      friend num operator/( const IMDsensor& s, const num& x ) { return s.signal / x; }

  };

    template <
    std::floating_point num
  > class SEQsensor {

    private:

      struct modifier { num coef{1}, rate{1}, cntr{0}, coef_upd{0}, rate_upd{0}, cntr_upd{0}; };

      std::vector<modifier> data; 
      
      num signal{0};

      friend class logit::sequential<num>;

    public:
      SEQsensor() = default;
      SEQsensor(const size_t& size) : data(size){
        for( auto& m : data ){
          m.coef = rand_gen<num>();
          m.rate = rand_gen<num>();
          m.cntr = rand_gen<num>();
        }
      };
      
      num dy_dx( const num& x, std::size_t n_drv = 1 ){

      assert( 0 < n_drv && n_drv < 3 );

      if( n_drv == 1 ) {

        return std::transform_reduce(data.cbegin(), data.cend(), (num)0, std::plus<num>{},
          [&x](const modifier& m) 
          { 
            num e_p = std::exp(m.rate * (x - m.cntr));
            return -(m.coef * m.rate * e_p) / (((num)1 + e_p) * ((num)1  + e_p));
          });
      }
      
      if( n_drv == 2 ) {

        return std::transform_reduce(data.cbegin(), data.cend(), (num)0, std::plus<num>{}, 
          [&x](const modifier& m)
          { 
            num e_p = std::exp(m.rate * (x - m.cntr));
            return -(m.coef * m.rate * m.rate * e_p * (-e_p + (num)1)) / (((num)1 + e_p) * ((num)1 + e_p) * ((num)1 + e_p));
          });
        }

        return 0.0f; // otherwise it complains
      }

      void calibrate(const num& error, num lr = 0.01 ){
        for( modifier& m : data ) {
          m.rate -= m.rate_upd * error * lr;
          m.cntr -= m.cntr_upd * error * lr;
          m.coef -= m.coef_upd * error * lr;

          m.rate_upd = 0;
          m.cntr_upd = 0;
          m.coef_upd = 0;
        }
      }

      num operator()( const num& x ){

        signal = std::transform_reduce(std::execution::seq, data.cbegin(), data.cend(), static_cast<num>(0), std::plus<num>{},
          [&x](const modifier& m)
          {
            return m.coef / (static_cast<num>(1) + std::exp(m.rate * (x - m.cntr)));
          });

        num one{1}, e_p, denom;

        for( modifier& m : data ) {
          e_p = std::exp(m.rate * (x - m.cntr)); 
          denom = (one + e_p) * (one + e_p);

          m.rate_upd += ((-m.coef * (x - m.cntr) * e_p ) / denom );
          m.cntr_upd += (( m.coef * m.rate * e_p ) / denom );
          m.coef_upd += (one / (one + e_p));
        }

        return signal;
      }

      const modifier& operator[](const std::size_t& i ) const { return data[i]; }
            modifier& operator[](const std::size_t& i )       { return data[i]; }

      SEQsensor& operator++(){ data.emplace_back( modifier{} ); return *this; }
      SEQsensor& operator--(){ data.pop_back(); return *this; }

      auto begin() const { return data.begin(); }
      auto begin()       { return data.begin(); }
      auto end()   const { return data.end(); }
      auto end()         { return data.end(); }

      auto cbegin() const { return data.cbegin(); }
      auto cend()   const { return data.cend();   }

      auto rbegin() { return data.rbegin(); }
      auto rend()   { return data.rend();   }

      std::size_t size() const { return data.size(); }

      // using friend function for commutative property
      friend num operator+( const num& x, const SEQsensor& s ) { return x + s.signal; }
      friend num operator-( const num& x, const SEQsensor& s ) { return x - s.signal; }
      friend num operator*( const num& x, const SEQsensor& s ) { return x * s.signal; }
      friend num operator/( const num& x, const SEQsensor& s ) { return x / s.signal; }

      friend num operator+( const SEQsensor& s, const num& x ) { return s.signal + x; }
      friend num operator-( const SEQsensor& s, const num& x ) { return s.signal - x; }
      friend num operator*( const SEQsensor& s, const num& x ) { return s.signal * x; }
      friend num operator/( const SEQsensor& s, const num& x ) { return s.signal / x; }

  };

}
  // types ~~~~~~~~~~


namespace logit {

  struct imd { };
  struct seq { };

  template < 
    std::floating_point num
  > class immediate {

  public:
    using sns_type = sensor::IMDsensor<num>;
    using  sns_ref = sns_type&;
    using csns_ref = const sns_ref&;

    immediate() = default;
    immediate( const std::size_t& num_s ){ sensors.resize(num_s); };

    immediate( std::size_t num_s, std::size_t s_size ){ initialize(num_s, s_size); };

    void initialize( std::size_t num_s, std::size_t s_size ){
      sensors.reserve(num_s); 
      weights.reserve(num_s); 
      while( 0 < num_s-- ){ 
        sensors.emplace_back(s_size);
        weights.emplace_back(rand_gen<num>());
      }
    }
  
    immediate& operator=(const immediate& other) = default;
    immediate& operator=(const num& x){ stimuli = x; return *this; }
    

    csns_ref operator[](const std::size_t& i ) const { return sensors[i]; }
     sns_ref operator[](const std::size_t& i )       { return sensors[i]; }


    void operator()(const std::size_t& i, const num& x ){ stimuli += weights[i] * sensors[i](x); }

    num operator()() const { return stimuli; }
 
    void add_sensor( std::size_t new_s, const std::size_t& n_terms ){
      while( 0 < new_s-- ){
        sensors.emplace_back(sns_type(n_terms));
        weights.emplace_back(rand_gen<num>());
      }; 
        sensors.shrink_to_fit();
        weights.shrink_to_fit();
    };

    void rmv_sensor( std::size_t rmv_s ){
      while( 0 < rmv_s-- ){ sensors.pop_back(); }
    }

    template<class functor> void transform(const functor& f ){ stimuli = f(stimuli); }

    void calibrate( const std::size_t& i, const num& x ){
      
      sensors[i].calibrate(x, error * weights[i]); 
      weights[i] -= error * sensors[i].signal * (num)0.01;
      
    }

    num dy_dx( const std::size_t& i, const num& x ){ return sensors[i].dy_dx(x) * error; }

    void add_error( const num& dx ){ error += dx; }
    void mul_error( const num& dx ){ error *= dx; }
    num  get_error(){ return error; }
    void show_error() { std::cout << "error: " << error << '\n'; }

    auto begin() const { return sensors.begin(); }
    auto end()   const { return sensors.end();   }

    auto cbegin() const { return sensors.cbegin(); }
    auto cend()   const { return sensors.cend();   }

    auto begin() { return sensors.begin(); }
    auto end()   { return sensors.end();   }

    auto rbegin() { return sensors.rbegin(); }
    auto rend()   { return sensors.rend();   }

    void reset() { stimuli = 0; error = 1; }

    std::size_t size() const { return sensors.size(); }

    friend bool operator==( const immediate& lhs, const immediate& rhs ){ return lhs.stimuli == rhs.stimuli; }
    friend bool operator!=( const immediate& lhs, const immediate& rhs ){ return lhs.stimuli != rhs.stimuli; }
    friend bool operator<=( const immediate& lhs, const immediate& rhs ){ return lhs.stimuli <= rhs.stimuli; }
    friend bool operator>=( const immediate& lhs, const immediate& rhs ){ return lhs.stimuli >= rhs.stimuli; }
    friend bool operator< ( const immediate& lhs, const immediate& rhs ){ return lhs.stimuli <  rhs.stimuli; }
    friend bool operator> ( const immediate& lhs, const immediate& rhs ){ return lhs.stimuli >  rhs.stimuli; }

  private:
    num stimuli{0}, error{1};
    std::vector<num> weights;
    std::vector<sensor::IMDsensor<num>> sensors;
  };













  template < 
    std::floating_point num
  > class sequential {

    public:
      using sns_type = sensor::SEQsensor<num>;
      using  sns_ref = sensor::SEQsensor<num>&;
      using csns_ref = const sensor::SEQsensor<num>&;

      sequential() = default;
      sequential( const std::size_t& num_s ){ sensors.resize(num_s); };

      sequential( std::size_t num_s, std::size_t s_size ){ initialize(num_s, s_size); };

      void initialize( std::size_t num_s, std::size_t s_size ){
        sensors.reserve(num_s); 
        weights.reserve(num_s); 
        while( 0 < num_s-- ){ 
          sensors.emplace_back(s_size);
          weights.emplace_back(rand_gen<num>());
        }
      }
    
      sequential& operator=(const sequential& other) = default;
      sequential& operator=(const num& x){ stimuli = x; return *this; }

       sns_ref operator[](const std::size_t& i )       { return sensors[i]; }
      csns_ref operator[](const std::size_t& i ) const { return sensors[i]; }

      num  operator()() const { return stimuli; }
      void operator()( const std::size_t& i, const num& x ){ stimuli += weights[i] * sensors[i](x); }

      void add_sensor( std::size_t new_s, const std::size_t& n_terms ){
        while( 0 < new_s-- ){
          sensors.emplace_back(sns_type(n_terms));
          weights.emplace_back(rand_gen<num>());
        }; 
          sensors.shrink_to_fit();
          weights.shrink_to_fit();
      };

      void rmv_sensor( std::size_t rmv_s ){
        while( 0 < rmv_s-- ){ sensors.pop_back(); }
      }

      template<class functor> void transform(const functor& f ){ stimuli = f(stimuli); }

      void calibrate( num lr = 0.01 ){
        
        for( std::size_t i{0}; i < sensors.size(); ++i ){
          sensors[i].calibrate(error * weights[i], lr); 
          weights[i] -= error * sensors[i].signal * lr;
        }
        error = 1; stimuli = 0;
      }

      num dy_dx( const std::size_t& i, const num& x ){ return sensors[i].dy_dx(x) * error; }

      void add_error( const num& dx ){ error += dx; }
      void mul_error( const num& dx ){ error *= dx; }
      num  get_error(){ return error; }
      void zero_error() { error = 1; }
      void show_error() { std::cout << "error: " << error << '\n'; }

      auto begin() const { return sensors.begin(); }
      auto end()   const { return sensors.end();   }

      auto cbegin() const { return sensors.cbegin(); }
      auto cend()   const { return sensors.cend();   }

      auto begin() { return sensors.begin(); }
      auto end()   { return sensors.end();   }

      auto rbegin() { return sensors.rbegin(); }
      auto rend()   { return sensors.rend();   }

      std::size_t size() const { return sensors.size(); }

      friend bool operator==( const sequential& lhs, const sequential& rhs ){ return lhs.stimuli == rhs.stimuli; }
      friend bool operator!=( const sequential& lhs, const sequential& rhs ){ return lhs.stimuli != rhs.stimuli; }
      friend bool operator<=( const sequential& lhs, const sequential& rhs ){ return lhs.stimuli <= rhs.stimuli; }
      friend bool operator>=( const sequential& lhs, const sequential& rhs ){ return lhs.stimuli >= rhs.stimuli; }
      friend bool operator< ( const sequential& lhs, const sequential& rhs ){ return lhs.stimuli <  rhs.stimuli; }
      friend bool operator> ( const sequential& lhs, const sequential& rhs ){ return lhs.stimuli >  rhs.stimuli; }

    private:
      num stimuli{0}, error{1};
      std::vector<num> weights;
      std::vector<sensor::SEQsensor<num>> sensors;
  };











}

#endif