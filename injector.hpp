#ifndef INJECTOR_H
#define INJECTOR_H

#include "activation.hpp"
#include "logits.hpp"

namespace injector {

  template <
    std::floating_point num,
    std::size_t         out_size
  > class shallow {

  private:

    num depth;
    std::vector<num> inp_memo;
    std::array<sequential_logit<num>, out_size> logits;

  public:

    using logit_ref = sequential_logit<num>&;

    shallow( const std::size_t& inp_rows, const std::size_t& inp_cols, const std::size_t& n_terms ){
      depth = (num)inp_rows; initialize(inp_cols, n_terms);
    };

    std::size_t size() const { return logits.size(); }

    const logit_ref operator[]( const std::size_t& i ) const { return logits[i]; }
          logit_ref operator[]( const std::size_t& i )       { return logits[i]; }

    template<class container>
    void forward( const container& inp ){

      std::transform(inp.cbegin(), inp.cend(), inp_memo.cbegin(), inp_memo.begin(), std::plus<num>{});
      
      for( logit_ref l_pos : logits ){
      for( std::size_t i{0}; i < logits.size(); ++i ){
        l_pos.receive(i, inp[i]); 
      }
        l_pos();
      }

      activation::softmax<num>(logits);
    }

    template<class container>
    void calibrate( const container& trg ){
      
      for( std::size_t i{0}; i < logits.size(); ++i ){
        logits[i].add_dx( logits[i].send() - trg[i] );
      }

      for( logit_ref l_pos : logits ){
      for( std::size_t i{0}; i < inp_memo.size(); ++i ){
        l_pos.calibrate(i, (num)0, depth - inp_memo[i]);
        l_pos.calibrate(i, (num)1,         inp_memo[i]);
      }
        l_pos.zero_error();
      }
      
      std::fill(logits.begin(),   logits.end(),   (num)0);
      std::fill(inp_memo.begin(), inp_memo.end(), (num)0);
    }

    template<std::unsigned_integral unum>
    void initialize( const unum& inp_size, const std::size_t& n_terms ){

      inp_memo.resize(inp_size);

      for( logit_ref l_pos : logits ) { l_pos.initialize(inp_size, n_terms); }
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

}

#endif