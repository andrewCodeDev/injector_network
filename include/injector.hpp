#ifndef INJECTOR_H
#define INJECTOR_H

#include <iomanip>
#include "logits.hpp"

namespace injector {

  // template <std::floating_point num, size_t out_size, class logit_t> class shallow;

// shallow injector networks ~~~~~~~~~~~~~~~

  template <class T>
  void print_grid(const T& logits){
    
    for( const auto& vec : logits ){

      for( size_t i{0}; i < vec.size(); ++i ){

        if( 0.0f < std::ceil(std::abs(vec[i].view())) && (&vec != &logits.back())){
          std::cout << std::setprecision(3) << '(' << i << ',' << std::ceil(std::abs(vec[i].view())) << ") ";
        }

        else {
          std::cout << std::setprecision(3) << std::ceil(std::abs(vec[i].view())) << ' ';
        }

      }
      std::cout << std::endl;
    } std::puts("\n");
  }



/*
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
    void forward( const container& packets ){
      
      for( auto& packet : packets ){
        for( auto& l_pos : logits ){ l_pos.take(packet.first, packet.second); }
      }

      activation::min_max<num>(scaled, logits);
      activation::softmax_scaled<num>(scaled);
    }

    void calibrate( const size_t& trg ){

        logits[trg].mul_error( scaled[trg] - (num)1 );
        logits[trg].calibrate();

        for( auto& l_pos : logits ){ l_pos.full_reset(); }
    }

      void reset_logits(){
        for( auto& l_pos : logits ){ l_pos.full_reset(); }
      }

      void display_output(){
        for( const auto& s : scaled ){
          std::cout << s << ' ';
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

      auto output() const { return scaled; }  
      auto output()       { return scaled; }  

      auto operator[]( const size_t& i ) const { return logits[i]; }
      auto operator[]( const size_t& i )       { return logits[i]; }

    private:
      std::array<logit::immediate<num>, out_size> logits;
      std::array<num, out_size> scaled{0};
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

    auto output() const { return scaled; }
    auto output()       { return scaled; }

    size_t size() const { return logits.size(); }

    clogit_ref operator[]( const size_t& i ) const { return scaled[i]; }
     logit_ref operator[]( const size_t& i )       { return scaled[i]; }

    void reset_logits(){
      for( auto& l_pos : logits ){ l_pos.full_reset(); }
    }

    template<class container>
    void forward( const container& packets ){

      num dx_tmp = 0;

      for( auto& packet : packets ){
        for( auto& l_pos : logits ){ l_pos.take(packet.first, packet.second); }
      }
        activation::min_max<num>(scaled, logits);
        activation::softmax_scaled<num>(scaled);
    }

    template<class container>
    void calibrate( const container& packets ){

        logits[packets.get_trg()].mul_error( scaled[packets.get_trg()] - (num)1 );
        
        logits[packets.get_trg()].calibrate( packets.enc_trg() * 0.1 );

        for( auto& l_pos : logits ){ l_pos.full_reset(); }
    }

    void display_output(){
      for( const auto& s : scaled ){
        std::cout << s << ' ';
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
    std::array<num, out_size> scaled{0};

  };

*/

  template <std::floating_point num, size_t layers, size_t out_size> class graph {

    public:
      using  logit_ref =       logit::immediate<num>&;
      using clogit_ref = const logit::immediate<num>&;

      graph( const size_t& inp_size, const size_t& n_terms ){

      // input sensors only have one connection
        
        logits[0].resize(inp_size);
        for( auto& l_pos : logits[0] ){ l_pos.initialize(1, n_terms); }
        
      // all preceeding layers are fully connected
        for( size_t i{1}; i < layers; ++i ){
          
          logits[i].resize(inp_size);
          
          for( auto& l_pos : logits[i] ){ l_pos.initialize(inp_size, n_terms); }
        }

      }

      template<class container>
      void forward( const container& packets ){
        

        size_t i{1}, past, current;
      
        logits[ 0 ][ packets[0].first ].take(0, packets[0].second);


        // std::cout << packets[0].first << '\n';
        // print_grid(logits);

      // one logit layer or one packet is required for the output states
        while( i < layers - 1 && i < packets.current_size() - 1 ){

          past    = packets[i - 1].first;
          current = packets[  i  ].first;
        
        // add past stimuli to future logit position
          logits[i][ current ].take(past, logits[i - 1][ past ].view());

          // std::cout << packets[i].first << '\n';
          // print_grid(logits);

          ++i;
        }

      // required for calibration
        output_idx = i;

      // create all output states - current is now i - 1 after last ++i
        for( auto& l_pos : logits[i] ){
          l_pos.take(current, logits[i - 1][ current ].view());
        }

        // std::cout << packets[i].first << '\n';
        // print_grid(logits);

      // scale and activate

        activation::min_max<num>(scaled, logits[i]);
        activation::softmax_scaled<num>(scaled);

        // for( const auto& x : scaled ){
        //   std::cout << std::setprecision(3) << std::ceil(std::abs(x)) << ' ';
        // } std::cout << std::endl;

      }

      template<class container>
      void calibrate( const container& packets ){
        
        // logits[ output_idx ][ packets.get_trg() ].mul_error( scaled[ packets.get_trg() ] - 1.0 );


          // std::cout << packets[output_idx].first << '\n';
          // print_grid(logits);

        for( size_t i{0}; i < out_size; ++i ){

          logits[ output_idx ][i].mul_error( scaled[i] - (num)(packets.get_trg() == i) );

          if( packets.get_trg() != i ){
            logits[ output_idx ][i].calibrate( packets[ output_idx - 1 ].first );
          }

        }


          // std::cout << packets[output_idx].first << '\n';
          // print_grid(logits);

        size_t i{ output_idx }, past, current;
    
        while( 0 < i ){
          past    = packets[i - 1].first;
          current = packets[  i  ].first;

        // take derivative with respect to the future logit
          logits[i - 1][ past ].mul_error( logits[i][ current ].dy_dx(past, logits[i - 1][ past ].view()) );

          logits[i][ current ].calibrate(past);

          // std::cout << packets[i].first << '\n';
          // print_grid(logits);

          --i;
        }

        logits[0][ packets[0].first ].calibrate(0);

        // std::cout << packets[0].first << '\n';
        // print_grid(logits);         

        // for( auto& l_pos : logits[output_idx] ){ l_pos.full_reset(); }

        // std::cout << packets[0].first << '\n';
        // print_grid(logits);         
      }

      void display_output(){
        for( const auto& s : scaled ){
          std::cout << s << ' ';
        } std::cout << '\n';
      }
      
      // void display_output(){
      //   for( const auto& s : logits.back() ){
      //     std::cout << s.view() << ' ';
      //   } std::cout << '\n';
      // }
      
      void reset_logits(){
        for( auto& vec : logits ){
          for( auto& l_pos : vec ){ l_pos.full_reset(); }
        }
      }

      size_t size() const { return logits.size(); }

      auto output() const { return scaled; }  
      auto output()       { return scaled; }  

      auto operator[]( const size_t& i ) const { return logits[i]; }
      auto operator[]( const size_t& i )       { return logits[i]; }

    private:
      size_t future{0}, current{0}, output_idx{0};
      std::array<num, out_size> scaled{0};
      std::array<std::vector<logit::immediate<num>>, layers> logits;
  };

// */

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