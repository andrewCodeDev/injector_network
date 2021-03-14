#ifndef SENSORS_H
#define SENSORS_H

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

  // base formula modifiers ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  template<std::floating_point num> struct IMDmodifier { num coef{1}, rate{1}, cntr{0}; };
  template<std::floating_point num> struct SEQmodifier { num coef{1}, rate{1}, cntr{0}, coef_upd{0}, rate_upd{0}, cntr_upd{0}; };

  // sensor base class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  template<std::floating_point num, typename modifier> class sensor_base {

    public:
      const modifier& operator[](const std::size_t& i ) const { return data[i]; }
            modifier& operator[](const std::size_t& i )       { return data[i]; }

      auto begin() const { return data.begin(); }
      auto begin()       { return data.begin(); }
      auto end()   const { return data.end(); }
      auto end()         { return data.end(); }

      auto cbegin() const { return data.cbegin(); }
      auto cend()   const { return data.cend();   }

      auto rbegin() { return data.rbegin(); }
      auto rend()   { return data.rend();   }

      std::size_t size() const { return data.size(); }

      operator num() const { return signal; }
    
    protected:
      num signal{0};
      std::vector<modifier> data;

      friend class logit::immediate<num>;
  };


  template <
    std::floating_point num
  > struct IMDsensor : public sensor_base<num, IMDmodifier<num>>{

    using modifier = IMDmodifier<num>;

    IMDsensor() = default;
    IMDsensor(const size_t& size){

      this->data.resize(size);

      for( auto& m : this->data ){
        m.coef = rand_gen<num>();
        m.rate = rand_gen<num>();
        m.cntr = rand_gen<num>();
      }
    };
    
    num dy_dx( const num& x, std::size_t n_drv = 1 ){

      assert( 0 < n_drv && n_drv < 3 );

      if( n_drv == 1 ) {

        return std::transform_reduce(this->data.cbegin(), this->data.cend(), (num)0, std::plus<num>{},
          [&x](const modifier& m) 
          { 
            num e_p = std::exp(m.rate * (x - m.cntr));
            return -(m.coef * m.rate * e_p) / (((num)1 + e_p) * ((num)1  + e_p));
          });
      }
      
      if( n_drv == 2 ) {

        return std::transform_reduce(this->data.cbegin(), this->data.cend(), (num)0, std::plus<num>{}, 
          [&x](const modifier& m)
          { 
            num e_p = std::exp(m.rate * (x - m.cntr));
            return -(m.coef * m.rate * m.rate * e_p * (-e_p + (num)1)) / (((num)1 + e_p) * ((num)1 + e_p) * ((num)1 + e_p));
          });
        }

        return 0.0f; // otherwise it complains
      }

    void calibrate( const num& x, const num& error, num lr = 0.01 ){

      num one{1}, adj{error * lr}, e_p, denom;
        
      for( modifier& m : this->data ) {
        e_p = std::exp(m.rate * (x - m.cntr)); 
        denom = (one + e_p) * (one + e_p);

        m.rate   -=  ((-m.coef * (x - m.cntr) * e_p ) / denom ) * adj;
        m.cntr -=  (( m.coef * m.rate * e_p ) / denom ) * adj;
        m.coef   -=  (one / (one + e_p)) * adj;
      }
    }

    num operator()( const num& x ){
      this->signal = std::transform_reduce(std::execution::seq, this->data.cbegin(), this->data.cend(), static_cast<num>(0), std::plus<num>{},
        [&x](const modifier& m)
        {
          return m.coef / (static_cast<num>(1) + std::exp(m.rate * (x - m.cntr)));
        });
      return this->signal;
    }

  };

    template <
    std::floating_point num
  > class SEQsensor {

    private:

      using modifier = SEQmodifier<num>;

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

      // implicit conversions ~~~~~~~~~~~~~~~~~~~~~~~~
      
      operator num() const { return signal; }

  };

}

#endif