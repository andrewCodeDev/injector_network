#ifndef SENSORS_H
#define SENSORS_H

#include <functional>
#include <assert.h>
#include <iostream>
#include <iterator>
#include <vector>
// #include <ranges>

#include "activation.hpp"

// sensor helper functions ~~~~~~~~~~~~~~~~~~~~

template <
  std::floating_point FT
> FT rand_gen(){ return ((FT)rand() / (FT)RAND_MAX * (FT)2 - (FT)1); }

template <
  class T
> void standard_form( const T& poly_num ){

  for(std::size_t i{0}; i < poly_num.size(); ++i) {
    if( i == (poly_num.size() - 1)) {
      std::cout << poly_num[i].coef << "/(1 + e^(" << poly_num[i].rate << "(x - " << poly_num[i].cntr << ")))";
    } else {
      std::cout << poly_num[i].coef << "/(1 + e^(" << poly_num[i].rate << "(x - " << poly_num[i].cntr << "))) + ";
    }
  } std::cout << '\n';
}


// sensor ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


namespace sensor {


  // base formula values~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


  template<std::floating_point num> struct imd_term_values {
    num coef{1},     rate{1},     cntr{0},
        coef_upd{0}, rate_upd{0}, cntr_upd{0}; 
  };
  
  template<std::floating_point num> struct seq_term_values {
    num coef{1},     rate{1},     cntr{0},
        coef_upd{0}, rate_upd{0}, cntr_upd{0},
        coef_dyn{0}, rate_dyn{0}, cntr_dyn{0},
        dx_mem{0};
    };


  // sensor base class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


  template<std::floating_point num, class term_values> class sensor_base {

    public:

      sensor_base() = default;
      sensor_base(const size_t& size){
        this->data.resize(size);
        for( auto& m : this->data ){
          m.coef = rand_gen<num>();
          m.rate = rand_gen<num>();
          m.cntr = rand_gen<num>();
        }
      };

      num dy_dx( const num& x ){

        // assert( 0 < n_drv && n_drv < 3 );

        // if( n_drv == 1 ) {
          return std::transform_reduce(this->data.cbegin(), this->data.cend(), (num)0, std::plus<num>{},
            [&x](const auto& m) 
            { 
              num exp_m = std::exp(m.rate * (x - m.cntr));
              return -(m.coef * m.rate * exp_m) / (((num)1 + exp_m) * ((num)1  + exp_m));
            });
        // }
        
        // if( n_drv == 2 ) {

        //   return std::transform_reduce(this->data.cbegin(), this->data.cend(), (num)0, std::plus<num>{}, 
        //     [&x](const auto& m)
        //     { 
        //       num exp_m = std::exp(m.rate * (x - m.cntr));
        //       return -(m.coef * m.rate * m.rate * exp_m * (-exp_m + (num)1)) / (((num)1 + exp_m) * ((num)1 + exp_m) * ((num)1 + exp_m));
        //     });
        //   }

        //   return 0.0f; // otherwise it complains
        }

      void reset_base() { signal = 0; state = false; }

      operator num() const { return signal; }

      std::size_t size() const { return data.size(); }

      auto begin() const { return data.begin(); }
      auto begin()       { return data.begin(); }
      auto end()   const { return data.end(); }
      auto end()         { return data.end(); }

      auto cbegin() const { return data.cbegin(); }
      auto cend()   const { return data.cend();   }

      auto rbegin() { return data.rbegin(); }
      auto rend()   { return data.rend();   }

      auto& operator[](const std::size_t& i ) const { return data[i]; }
      auto& operator[](const std::size_t& i )       { return data[i]; }

      operator bool() { return state; }

    protected:
      num signal{0};
      bool state{false};
      std::vector<term_values> data;
  };


  // sensor derived types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


  template <
    std::floating_point num
  > class immediate final : public sensor_base<num, imd_term_values<num>>{
    
    public:
      immediate() = default;
      immediate(const size_t& size) : sensor_base<num, imd_term_values<num>>(size){ };

      void calibrate(const num& error, num lr = 0.01 ){

        this->reset_base();

        for( auto& m : this->data ) {
          m.rate -= (m.rate_upd * error * lr);
          m.cntr -= (m.cntr_upd * error * lr);
          m.coef -= (m.coef_upd * error * lr);

          m.rate_upd = 0;
          m.cntr_upd = 0;
          m.coef_upd = 0;
        }

      }

      void reset(){

        this->reset_base();

        for( auto& m : this->data ) {
          m.rate_upd = 0;
          m.cntr_upd = 0;
          m.coef_upd = 0;
        }
      }

      num operator()( const num& x ){

        this->state = true;
       
        // memoize function calls;
        num exp_m, div_m;

        for( auto& m : this->data ){

          exp_m = std::exp(m.rate * (x - m.cntr)); 
          div_m = ((num)1 + exp_m) * ((num)1 + exp_m);

          this->signal += m.coef / ((num)1 + exp_m);

          m.rate_upd += ((-m.coef * (x - m.cntr) * exp_m ) / div_m );
          m.cntr_upd += (( m.coef * m.rate * exp_m ) / div_m );
          m.coef_upd += ((num)1 / ((num)1 + exp_m));
        }

        return this->signal;
      }

  };


  template <
    std::floating_point num
  > class sequential final : public sensor_base<num, seq_term_values<num>>{
    
    public:
      sequential() = default;
      sequential(const size_t& size) : sensor_base<num, seq_term_values<num>>(size){ };

      void calibrate(const num& error, num lr = 0.01 ){
        
        this->reset_base();

        for( auto& m : this->data ) {
          m.rate -= (m.rate_upd * error * lr);
          m.cntr -= (m.cntr_upd * error * lr);
          m.coef -= (m.coef_upd * error * lr);

          m.rate_upd = 0;
          m.cntr_upd = 0;
          m.coef_upd = 0;

          m.rate_dyn = 0;
          m.cntr_dyn = 0;
          m.coef_dyn = 0;

          m.dx_mem   = 0;
        }

      }

      void reset(){

        this->reset_base();

        for( auto& m : this->data ){
          m.rate_upd = 0;
          m.cntr_upd = 0;
          m.coef_upd = 0;

          m.rate_dyn = 0;
          m.cntr_dyn = 0;
          m.coef_dyn = 0;

          m.dx_mem   = 0;
        }
      }

      num operator()( const num& x ){

        this->state = true;

        for( auto& m : this->data ){

          num tmp = f(x + m.dx_mem, m.coef, m.rate, m.cntr);

          m.coef_upd += (f(x + m.coef_dyn, m.coef + (num)1e-3, m.rate, m.cntr) - tmp) / (num)1e-3;
          m.rate_upd += (f(x + m.rate_dyn, m.coef, m.rate + (num)1e-3, m.cntr) - tmp) / (num)1e-3;
          m.cntr_upd += (f(x + m.cntr_dyn, m.coef, m.rate, m.cntr + (num)1e-3) - tmp) / (num)1e-3;

          m.dx_mem = g(x + m.dx_mem, m.coef, m.rate, m.cntr);

          m.coef_dyn = g(x + m.coef_dyn, m.coef + (num)1e-3, m.rate, m.cntr);
          m.rate_dyn = g(x + m.rate_dyn, m.coef, m.rate + (num)1e-3, m.cntr);
          m.cntr_dyn = g(x + m.cntr_dyn, m.coef, m.rate, m.cntr + (num)1e-3);

          this->signal += tmp;
        }
        return this->signal;
      }

    private:
      num f(const num& x, const num& coef, const num& rate, const num& cntr)
      {
        return coef / (static_cast<num>(1) + std::exp(rate * (x - cntr)));
      }

      num g(const num& x, const num& coef, const num& rate, const num& cntr)
      {
        num exp_m = std::exp(rate * (x - cntr));
        return -(coef * rate * exp_m) / (((num)1 + exp_m) * ((num)1  + exp_m));
      } 
  };

}

#endif