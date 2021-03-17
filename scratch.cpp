#include <bits/stdc++.h>

#include "include/sensors.hpp"

void print_test(std::string str, const float& x)
{
  std::cout << str << ": " <<  x << '\n';
}

  template<std::floating_point num> struct SCRmodifier {
    num coef{0.5},     cntr{0.5},
        coef_upd{0}, cntr_upd{0},
        coef_dyn{0}, cntr_dyn{0},
        dx_mem{0};
    };

  template <
    std::floating_point num
  > class SCRsensor : public sensor::sensor_base<num, SCRmodifier<num>>{
    
    public:
      SCRsensor() = default;
      SCRsensor(const size_t& size){
        this->data.resize(size);
        for( auto& m : this->data ){
          m.coef = 0.5;
          m.cntr = 0.5;
        }
      };

      void calibrate(const num& error, num lr = 0.01 ){

        for( auto& m : this->data ) {
          m.coef -= m.coef_upd * error * lr;
          m.cntr -= m.cntr_upd * error * lr;

          m.coef_upd = 0;
          m.cntr_upd = 0;

          m.coef_dyn = 0;
          m.cntr_dyn = 0;

          m.dx_mem   = 0;
        }
      }

      num operator()( const num& x ){
        for( auto& m : this->data ){

          num tmp = f(x + m.dx_mem, m.coef, m.cntr);

          print_test("tmp", tmp);

          // print_test("cntr + h", f(x + m.coef_dyn, m.coef + (num)1e-3, m.cntr));
          print_test("cntr dyn", m.cntr_dyn);

          // print_test("dx_mem", m.dx_mem);

          print_test("cntr", (f(x + m.cntr_dyn, m.coef, m.cntr + (num)1e-3) - tmp) / (num)1e-3);

          m.coef_upd += (f(x + m.coef_dyn, m.coef + (num)1e-3, m.cntr) - tmp) / (num)1e-3;
          m.cntr_upd += (f(x + m.cntr_dyn, m.coef, m.cntr + (num)1e-3) - tmp) / (num)1e-3;

          m.dx_mem = g(x + m.dx_mem, m.coef, m.cntr);

          m.coef_dyn = g(x + m.coef_dyn, m.coef + (num)1e-3, m.cntr);
          m.cntr_dyn = g(x + m.cntr_dyn, m.coef, m.cntr + (num)1e-3);


          this->signal += tmp;
        }
        return this->signal;
      }

      num f( const num& x, const num& coef, const num& cntr ) { return coef * std::pow(x - cntr, 2); }
      num g( const num& x, const num& coef, const num& cntr ) { return (num)2 * coef * (x - cntr);   } 
  };

int main(void){

  std::cout << std::boolalpha;

  SCRsensor<float> ds(1);

  ds(1.0f);
  ds(1.0f);
  ds(1.0f);



  std::cout << '\n';


// //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//   std::cout << '\n'; //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  
// //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


  return 0;
}