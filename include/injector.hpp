#ifndef INJECTOR_H
#define INJECTOR_H

#include "logits.hpp"

namespace injector {

  template<std::floating_point num, std::size_t out_size, class logit_t> class shallow;

  template <
    std::floating_point num,
    std::size_t         out_size
  > class shallow< num, out_size, logit::imd >{

  private:

    std::array<logit::immediate<num>, out_size> logits;

  public:

    using logit_ref  =       logit::immediate<num>&;
    using clogit_ref = const logit::immediate<num>&;

    shallow( const std::size_t& inp_size, const std::size_t& n_terms ){
      for( logit_ref l_pos : logits ) { l_pos.initialize(inp_size, n_terms); }
    };

    std::size_t size() const { return logits.size(); }

    clogit_ref operator[]( const std::size_t& i ) const { return logits[i]; }
    logit_ref  operator[]( const std::size_t& i )       { return logits[i]; }

    template<class container>
    void forward( const container& inp ){

      for( logit_ref l_pos : logits ){
      for( std::size_t i{0}; i < logits.size(); ++i ){
        l_pos.receive(i, inp[i]); 
      }
        l_pos();
      }
      activation::softmax<num>(logits);
    }

    template<class container_a, class container_b>
    void calibrate( const container_a& inp, const container_b& trg ){

      for( std::size_t i{0}; i < logits.size(); ++i ){
        logits[i].add_error( logits[i].send() - trg[i] );
      }

      for( logit_ref l_pos : logits ){
      for( std::size_t i{0}; i < trg.size(); ++i ){
        l_pos.calibrate(i, inp[i]);
      }
        l_pos.zero_error();
      }
    }

    void display_output(){
      for( const logit_ref l_pos : logits ){
        std::cout << l_pos.send() << ' ';
      } std::cout << '\n';
    }

    void display_formulas(){
      for( const logit_ref l_pos : logits ){
      for( std::size_t i{0}; i < logits.size(); ++i ){
        standard_form(l_pos[i]);
      } std::cout << '\n';
      } std::cout << '\n';
    }

  };

  template <
    std::floating_point num,
    std::size_t         out_size
  > class shallow< num, out_size, logit::seq >{

  private:

    std::array<logit::immediate<num>, out_size> logits;

  public:

    using clogit_ref = const logit::immediate<num>&;
    using  logit_ref =       logit::immediate<num>&;

    shallow( const std::size_t& inp_size, const std::size_t& n_terms ){
      for( logit_ref l_pos : logits ) { l_pos.initialize(inp_size, n_terms); }
    };

    std::size_t size() const { return logits.size(); }

    clogit_ref operator[]( const std::size_t& i ) const { return logits[i]; }
    logit_ref  operator[]( const std::size_t& i )       { return logits[i]; }

    template<class container>
    void forward( const container& inp ){
      
      for( logit_ref l_pos : logits ){
      for( std::size_t i{0}; i < inp.size(); ++i ){
        l_pos.receive(i, inp[i]); 
      }
        l_pos();
      }
      activation::min_max<num>(logits);
      activation::softmax<num>(logits);
    }

    template<class container_a, class container_b>
    void calibrate( const container_a& inp, const container_b& trg ){

      for( std::size_t i{0}; i < logits.size(); ++i ){
        logits[i].add_error( logits[i].send() - trg[i] );
      }

      for( logit_ref l_pos : logits ){
      for( std::size_t i{0}; i < trg.size(); ++i ){
        l_pos.calibrate(i, inp[i]);
      }
        l_pos.zero_error();
      }
    }

    void display_output(){
      for( const logit_ref l_pos : logits ){
        std::cout << l_pos.send() << ' ';
      } std::cout << '\n';
    }

    void display_formulas(){
      for( const logit_ref l_pos : logits ){
      for( std::size_t i{0}; i < logits.size(); ++i ){
        standard_form(l_pos[i]);
      } std::cout << '\n';
      } std::cout << '\n';
    }

  };

  template <
    std::floating_point num
  > struct binary {

    num error{0};

    std::pair<logit::immediate<num>, logit::immediate<num>> l_pair;

    binary( const std::size_t& n_terms ){
      l_pair.first.add_sensor(1, n_terms);
      l_pair.second.add_sensor(1, n_terms);
    };

    bool eval( const num& x, const num& y ) { forward(x, y); return ( l_pair.first.send() < l_pair.second.send() ) ? false : true; }

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

    void forward( const num& x, const num& y ){

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

        l_pair.first.zero_error();
        l_pair.second.zero_error(); 

        l_pair.first.add_error(l_pair.first.send() - v_trg[0]);
        l_pair.first.calibrate(0, inp[0]);

        l_pair.second.add_error(l_pair.second.send() - v_trg[1]);
        l_pair.second.calibrate(0, inp[1]);

    }

    void display_formulas(){
      standard_form(l_pair.first[0]); std::cout << '\n';
      standard_form(l_pair.second[0]); std::cout << '\n';
    }

    void display_output(){
      std::cout << '{' << l_pair.first.send() << ", " << l_pair.second.send() << "}\n";
    }

  };

}

#endif