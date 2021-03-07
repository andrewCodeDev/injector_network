#ifndef BIN_SWITCH_H
#define BIN_SWITCH_H

#include "logits.hpp"
  
  template <
    std::floating_point num
  > struct binary_switch {

    num error{0};

    std::pair<logit<num>, logit<num>> l_pair;

    binary_switch( const std::size_t& n_terms ){
      l_pair.first.add_sensor(1, n_terms);
      l_pair.second.add_sensor(1, n_terms);
    };

    bool eval(const num& x, const num& y) { forward(x, y); return ( l_pair.first.send() < l_pair.second.send() ) ? false : true; }

    template<class container>
    void forward( const container& inp ){

      l_pair.first.receive(0, inp[0]); 

      l_pair.first();

      l_pair.second.receive(0, inp[1]); 
      
      l_pair.second();

      num denom = std::exp(l_pair.first.send()) +  std::exp(l_pair.second.send());

      l_pair.first  = std::exp(l_pair.first.send())  / denom;
      l_pair.second = std::exp(l_pair.second.send()) / denom;

    }

    void forward( const num& x, const num& y){

      l_pair.first.receive(0, x); 

      l_pair.first();

      l_pair.second.receive(0, y); 
      
      l_pair.second();

      num denom = std::exp(l_pair.first.send()) +  std::exp(l_pair.second.send());

      l_pair.first  = std::exp(l_pair.first.send()) / denom;
      l_pair.second = std::exp(l_pair.second.send()) / denom;
    }


    template<class container_a, class container_b>
    void calibrate( const container_a &inp, const container_b& v_trg ){

        l_pair.first.add_dx(l_pair.first.send() - v_trg[0]);
        l_pair.first.calibrate(0, inp[0]);

        l_pair.second.add_dx(l_pair.second.send() - v_trg[1]);
        l_pair.second.calibrate(0, inp[1]);

      l_pair.first.zero_error();
      l_pair.second.zero_error(); 

    }


    void display_formulas(){
      standard_form(l_pair.first[0]); std::cout << '\n';
      standard_form(l_pair.second[0]); std::cout << '\n';
    }

    void display_output(){
      std::cout << '{' << l_pair.first.send() << ", " << l_pair.second.send() << "}\n";
    }

  };

#endif