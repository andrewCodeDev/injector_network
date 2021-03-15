#include <bits/stdc++.h>

#include "include/sensors.hpp"

void print_test(std::string str, const float& x)
{
  std::cout << str << ": " <<  x << '\n';
}

  template <std::floating_point num> void abs_max( auto& logits ){

    num maxm = 0;

    for( auto& l_pos : logits ){ maxm = std::max(maxm, std::abs(l_pos())); }

    if(maxm < (num)1){ return; }

    else {
      maxm = pow(static_cast<num>(10), -floor(log10(floor(maxm * static_cast<num>(10)))));
    
      for( auto& l_pos : logits ){ l_pos = l_pos() * maxm;}
    }

  }


int main(void){

  std::cout << std::boolalpha;

  sensor::DYNsensor<float> ds(3);


  std::cout << '\n';


//   float x = 1.5;

//   float ins_main = 0;
//   float ins_coef = 0;
//   float ins_rate = 0;
//   float ins_cntr = 0;
  
//   print_test("dyn_deriv", ins_main);
//   print_test("compute coef deriv", (ds.f(x + ins_coef, ds[0].coef + 1e-3f, ds[0].rate, ds[0].cntr) - ds.f(x + ins_main, ds[0].coef, ds[0].rate, ds[0].cntr)) / 1e-3f);
//   print_test("compute rate deriv", (ds.f(x + ins_rate, ds[0].coef, ds[0].rate + 1e-3f, ds[0].cntr) - ds.f(x + ins_main, ds[0].coef, ds[0].rate, ds[0].cntr)) / 1e-3f);
//   print_test("compute cntr deriv", (ds.f(x + ins_cntr, ds[0].coef, ds[0].rate, ds[0].cntr + 1e-3f) - ds.f(x + ins_main, ds[0].coef, ds[0].rate, ds[0].cntr)) / 1e-3f);
  
//   std::cout << '\n';
  

//   ins_main = ds.g(x + ins_main, ds[0].coef, ds[0].rate, ds[0].cntr);
//   ins_coef = ds.g(x + ins_main, ds[0].coef + 1e-3f, ds[0].rate, ds[0].cntr);
//   ins_rate = ds.g(x + ins_main, ds[0].coef, ds[0].rate + 1e-3f, ds[0].cntr);
//   ins_cntr = ds.g(x + ins_main, ds[0].coef, ds[0].rate, ds[0].cntr + 1e-3f);


//   print_test("dx_mem", ds[0].dx_mem);

//   ds(x);

//   print_test("coef_deriv", ds[0].coef_upd);
//   print_test("rate_deriv", ds[0].rate_upd);
//   print_test("cntr_deriv", ds[0].cntr_upd);


// //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//   std::cout << '\n'; //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  
// //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//   x = 2.1;
  

//   print_test("dyn_deriv", ins_main);
//   print_test("compute coef deriv", (ds.f(x + ins_coef, ds[0].coef + 1e-3f, ds[0].rate, ds[0].cntr) - ds.f(x + ins_main, ds[0].coef, ds[0].rate, ds[0].cntr)) / 1e-3f);
//   print_test("compute rate deriv", (ds.f(x + ins_rate, ds[0].coef, ds[0].rate + 1e-3f, ds[0].cntr) - ds.f(x + ins_main, ds[0].coef, ds[0].rate, ds[0].cntr)) / 1e-3f);
//   print_test("compute cntr deriv", (ds.f(x + ins_cntr, ds[0].coef, ds[0].rate, ds[0].cntr + 1e-3f) - ds.f(x + ins_main, ds[0].coef, ds[0].rate, ds[0].cntr)) / 1e-3f);
//   std::cout << '\n';
  
//   print_test("dx_mem", ds[0].dx_mem);

//   ds(x);
  
//   print_test("coef_deriv", ds[0].coef_upd);
//   print_test("rate_deriv", ds[0].rate_upd);
//   print_test("cntr_deriv", ds[0].cntr_upd);

  // print_test("output", ds.f(x, ds[0].coef, ds[0].rate, ds[0].cntr));


  std::vector<float> vec1{ 0.9, 0.2, -0.5, -0.7 };
  std::vector<float> vec2{ 1.9, 0.2, -3.5, -0.7 };
  std::vector<float> vec3{ 12.9, 49.2, -3.5, -0.7 };

  abs_max<float>(vec1);
  abs_max<float>(vec2);
  abs_max<float>(vec3);

  for(auto& x : vec1) { std::cout << x << ' '; } std::cout << '\n';
  for(auto& x : vec2) { std::cout << x << ' '; } std::cout << '\n';
  for(auto& x : vec3) { std::cout << x << ' '; } std::cout << '\n';


  return 0;
}