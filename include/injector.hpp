#ifndef INJECTOR_H
#define INJECTOR_H

#include "logits.hpp"

namespace injector {

  template <std::floating_point num, size_t out_size, class logit_t> class shallow;

// shallow injector networks ~~~~~~~~~~~~~~~

  template <

    std::floating_point num, size_t out_size
  
  > class shallow< num, out_size, logit::imd >{

    public:
      using  logit_ref =       logit::immediate<num>&;
      using clogit_ref = const logit::immediate<num>&;

      shallow( const size_t& inp_size, const size_t& n_terms ){
        for( logit_ref l_pos : logits ) { l_pos.initialize(inp_size, n_terms); }
      };

      template<class container>
      void forward( const container& inp ){
        
        for( logit_ref l_pos : logits ){
          for( size_t i{0}; i < logits.size(); ++i ){ l_pos.take(i, inp[i]); }
        }
        activation::softmax_diff<num>(logits);
      }

      template<class container_a, class container_b>
      void calibrate( const container_a& inp, const container_b& trg ){
        
        for( size_t i{0}; i < logits.size(); ++i ){
          logits[i].mul_error( logits[i].view() - trg[i] );
        }

        for( logit_ref l_pos : logits ){ l_pos.calibrate(); }
      }

      void display_output(){
        for( const auto& l_pos : logits ){
          std::cout << l_pos.view() << ' ';
        } std::cout << '\n';
      }

      void display_formulas(){
        for( clogit_ref l_pos : logits ){
        for( size_t i{0}; i < logits.size(); ++i ){
          standard_form(l_pos[i]);
        } std::cout << '\n';
        } std::cout << '\n';
      }

      size_t size() const { return logits.size(); }

      auto operator[]( const size_t& i ) const { return logits[i]; }
      auto operator[]( const size_t& i )       { return logits[i]; }

    private:
      std::array<logit::immediate<num>, out_size> logits;
  };

// ~~~~~~~~~~~~~~~~~~~~~~~~

  template <
    std::floating_point num,
    size_t         out_size
  > class shallow< num, out_size, logit::seq >{

  public:

    using clogit_ref = const logit::sequential<num>&;
    using  logit_ref =       logit::sequential<num>&;

    shallow( const size_t& inp_size, const size_t& n_terms ){
      for( logit_ref l_pos : logits ) { l_pos.initialize(inp_size, n_terms); }
    };

    std::array<logit::sequential<num>, out_size>& output() const { return logits; }
    std::array<logit::sequential<num>, out_size>& output()       { return logits; }

    size_t size() const { return logits.size(); }

    clogit_ref operator[]( const size_t& i ) const { return logits[i]; }
     logit_ref operator[]( const size_t& i )       { return logits[i]; }

    void reset_logits(){
      for( auto& l_pos : logits ){ l_pos.full_reset(); }
    }

    template<class container>
    void forward( const container& packets ){
      
      for( auto& packet : packets ){
        for( auto& l_pos : logits ){ l_pos.take(packet.first, packet.second); }
      }
      activation::softmax<num>(logits);
    }

    void calibrate( const size_t& trg ){

    // for( size_t i{0}; i < logits.size(); ++i ){
        logits[trg].mul_error( -(num)1 / logits[trg].view() );
        // logits[i].mul_error( logits[i]() - (num)(i == trg) );
        logits[trg].calibrate();

        for( auto& l_pos : logits ){ l_pos.full_reset(); }
    }

    void display_output(){
      for( clogit_ref l_pos : logits ){
        std::cout << l_pos.view() << ' ';
      } std::cout << '\n';
    }

    void display_formulas(){
      for( clogit_ref l_pos : logits ){
      for( size_t i{0}; i < logits.size(); ++i ){
        standard_form(l_pos[i]);
      } std::cout << '\n';
      } std::cout << '\n';
    }

  private:

    std::array<logit::sequential<num>, out_size> logits;

  };

/*

  template <
    std::floating_point num,
    size_t         out_size
  > class shallow< num, out_size, logit::dyn >{

  private:

    std::array<logit::dynamic<num>, out_size> logits;

  public:

    using clogit_ref = const logit::dynamic<num>&;
    using  logit_ref =       logit::dynamic<num>&;

    shallow( const size_t& inp_size, const size_t& n_terms ){
      for( logit_ref l_pos : logits ) { l_pos.initialize(inp_size, n_terms); }
    };

    std::array<logit::dynamic<num>, out_size>& output() const { return logits; }
    std::array<logit::dynamic<num>, out_size>& output()       { return logits; }

    size_t size() const { return logits.size(); }

    clogit_ref operator[]( const size_t& i ) const { return logits[i]; }
     logit_ref operator[]( const size_t& i )       { return logits[i]; }

    void reset_logits(){
      for( auto& l_pos : logits ){ l_pos.full_reset(); }
    }

    template<class container>
    void forward( const container& inp ){
      
      for( auto& r_inp : inp    )
      for( auto& l_pos : logits )
      for( size_t i{0}; i < r_inp.size(); ++i ){
        
        // std::cout << r_inp[i] << ' ';
        l_pos(i, r_inp[i]); 
      }
      
      // display_output();
      activation::softmax<num>(logits);
    }

    template<class container>
    void calibrate( const container& trg ){
      for( size_t i{0}; i < logits.size(); ++i ){
        logits[i].mul_error(( -trg[i] / logits[i]() ) / (num)trg.size() );
        // logits[i].mul_error( logits[i]() - trg[i] );
        logits[i].calibrate();
      }
    }

    void display_output(){
      for( clogit_ref l_pos : logits ){
        std::cout << l_pos() << ' ';
      } std::cout << '\n';
    }

    void display_formulas(){
      for( clogit_ref l_pos : logits ){
      for( size_t i{0}; i < logits.size(); ++i ){
        standard_form(l_pos[i]);
      } std::cout << '\n';
      } std::cout << '\n';
    }

  };

*/

  // template <
  //   std::floating_point num,
  //   size_t         out_size
  // > class shallow< num, out_size, logit::dyn >{

  // private:

  //   std::array<logit::dynamic<num>, out_size> logits;

  // public:

  //   using clogit_ref = const logit::dynamic<num>&;
  //   using  logit_ref =       logit::dynamic<num>&;

  //   shallow( const size_t& inp_size, const size_t& n_terms ){
  //     for( logit_ref l_pos : logits ) { l_pos.initialize(inp_size, n_terms); }
  //   };

  //   std::array<logit::dynamic<num>, out_size>& output() const { return logits; }
  //   std::array<logit::dynamic<num>, out_size>& output()       { return logits; }

  //   size_t size() const { return logits.size(); }

  //   clogit_ref operator[]( const size_t& i ) const { return logits[i]; }
  //    logit_ref operator[]( const size_t& i )       { return logits[i]; }

  //   void reset_logits(){
  //     for( auto& l_pos : logits ){ l_pos.full_reset(); }
  //   }

  //   template<class container>
  //   void forward( const container& packets ){
      
  //     for( auto& packet : packets ){
  //       for( auto& l_pos : logits ){ l_pos(packet.first, packet.second); }
  //     }
  //     activation::softmax_diff<num>(logits);
  //   }

  //   void calibrate( const size_t& trg ){

  //     for( size_t i{0}; i < logits.size(); ++i ){
  //       // logits[i].mul_error( -(num)(i == trg) / logits[i]() );
  //       logits[i].mul_error( logits[i]() - (num)(i == trg) );
  //       logits[i].calibrate();
  //     }
  //   }

  //   void display_output(){
  //     for( clogit_ref l_pos : logits ){
  //       std::cout << l_pos() << ' ';
  //     } std::cout << '\n';
  //   }

  //   void display_formulas(){
  //     for( clogit_ref l_pos : logits ){
  //     for( size_t i{0}; i < logits.size(); ++i ){
  //       standard_form(l_pos[i]);
  //     } std::cout << '\n';
  //     } std::cout << '\n';
  //   }

  // };

// bilayer ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*

  template <
    std::floating_point num,
    size_t         out_size
  > class bilayer< num, out_size, logit::dyn, logit::seq >{

    private:


    public:

      std::array<logit::dynamic<num>,    out_size> logits_a;
      std::array<logit::sequential<num>, out_size> logits_b;

      bilayer( const size_t& inp_size, const size_t& n_terms ){
        for( auto& l_pos : logits_a ) { l_pos.initialize(inp_size, n_terms); }
        for( auto& l_pos : logits_b ) { l_pos.initialize(inp_size, n_terms); }
      };

      size_t size() const { return out_size; }

      void reset_logits(){
        for( auto& l_pos : logits_a ){ l_pos.full_reset(); }
        for( auto& l_pos : logits_b ){ l_pos.full_reset(); }
      }

      const std::array<logit::sequential<num>, out_size>& output() const { return logits_b; }
            std::array<logit::sequential<num>, out_size>& output()       { return logits_b; }

      template<class container>
      void forward( const container& inp ){
        
      for( size_t r{0}; r < inp.current_size(); ++r ){
        
        for( auto& l_pos : logits_a )
        for( size_t i{0}; i < inp[r].size(); ++i ){
          l_pos(i, inp[r][i]); 
        }

        for( auto& l_pos : logits_b )
        for( size_t i{0}; i < logits_a.size(); ++i ){
          l_pos(i, logits_a[i]());
        }
      }
        activation::softmax<num>(logits_b);
      }

      template<class container>
      void calibrate( const container& trg ){

        for( size_t i{0}; i < logits_b.size(); ++i ){
          logits_b[i].mul_error(( -trg[i] / logits_b[i]() ) / (num)trg.size() );
        }
        
        num tmp{0};
        
        for( size_t i{0}; i < logits_a.size(); ++i ){
        for( size_t j{0}; j < logits_b.size(); ++j ){
          tmp += logits_b[j].dy_dx(i, logits_a[i]());
        }
          logits_a[i].mul_error(tmp); tmp = 0;
        }

        for( size_t i{0}; i < out_size; ++i ){
          logits_a[i].calibrate();
          logits_b[i].calibrate();
        }
      }

      void display_output() const {
        for( const auto& l_pos : logits_b ){
          std::cout << l_pos() << ' ';
        } std::cout << '\n';
      }

      void display_formulas() const {
        for( const auto& l_pos : logits_a ){
        for( size_t i{0}; i < logits_a.size(); ++i ){
          standard_form(l_pos[i]);
        } std::cout << '\n';
        } std::cout << '\n';

        for( const auto& l_pos : logits_b ){
        for( size_t i{0}; i < logits_b.size(); ++i ){
          standard_form(l_pos[i]);
        } std::cout << '\n';
        } std::cout << '\n';
      }

  };

  template <
    size_t               lyr_size,
    size_t               inp_size,
    size_t               hid_size,
    size_t               out_size,
    typename             logit_t
  > class multi {

    private:

      // struct paths { size_t h{0}, m{0}, l{0}; }; 

      std::array<std::vector<logit_t>, lyr_size> logits;

    public:

      multi( const size_t& n_terms ){
        
        // input layer - hidden size, number of sensors equal to input size
          logits[0].resize(hid_size);

          for( auto& l_pos : logits[0]){ l_pos.initialize(inp_size, n_terms); }

        // hidden layer - hidden size, number of sensors equal to hidden size
          for( size_t i{1}; i < lyr_size - 1; ++i ){         
            
            logits[i].resize(hid_size);

            for( l_pos : logits[i] ){ l_pos.initialize(hid_size, n_terms); }
          }

        // output layer - output size, number of sensors equal to hidden size
          logits.back().resize(out_size);

          for( auto& l_pos : logits.back() ){ l_pos.initialize(hid_size, n_terms); }

      }


      void reset_logits(){

        for( auto& layer : logits ){
          for( auto& l_pos : layer ){ l_pos.full_reset(); }
        }
      }

      const std::array<logit_t, out_size>& output() const { return logits.back(); }
            std::array<logit_t, out_size>& output()       { return logits.back(); }

      template<class container>
      void forward( const container& inp ){
        
      for( size_t r{0}; r < inp.current_size(); ++r ){
        

      }
        activation::softmax<num>(logits_b);
      }

      template<class container>
      void calibrate( const container& trg ){

        for( size_t i{0}; i < logits_b.size(); ++i ){
          logits_b[i].mul_error(( -trg[i] / logits_b[i]() ) / (num)trg.size() );
        }
        
        num tmp{0};
        
        for( size_t i{0}; i < logits_a.size(); ++i ){
        for( size_t j{0}; j < logits_b.size(); ++j ){
          tmp += logits_b[j].dy_dx(i, logits_a[i]());
        }
          logits_a[i].mul_error(tmp); tmp = 0;
        }

        for( size_t i{0}; i < out_size; ++i ){
          logits_a[i].calibrate();
          logits_b[i].calibrate();
        }
      }

      void display_output() const {
        for( const auto& l_pos : logits_b ){
          std::cout << l_pos() << ' ';
        } std::cout << '\n';
      }

      void display_formulas() const {
        for( const auto& l_pos : logits_a ){
        for( size_t i{0}; i < logits_a.size(); ++i ){
          standard_form(l_pos[i]);
        } std::cout << '\n';
        } std::cout << '\n';

        for( const auto& l_pos : logits_b ){
        for( size_t i{0}; i < logits_b.size(); ++i ){
          standard_form(l_pos[i]);
        } std::cout << '\n';
        } std::cout << '\n';
      }

  };

*/


}

#endif