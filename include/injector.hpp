#ifndef INJECTOR_H
#define INJECTOR_H

#include <iomanip>
#include "logits.hpp"

namespace injector {

  // template <std::floating_point num, size_t out_size, class logit_t> class shallow;

// shallow injector networks ~~~~~~~~~~~~~~~


  // debug
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


  template <std::floating_point num, size_t layers, size_t out_size> class graph {

    public:
      using clogit_ref = const logit::immediate<num>&;
      using  logit_ref =       logit::immediate<num>&;

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

      // one logit layer or one packet is required for the output states
        while( i < layers - 1 && i < packets.current_size() - 1 ){

          past    = packets[i - 1].first;
          current = packets[  i  ].first;
        
        // add past stimuli to future logit position
          logits[i][ current ].take(past, logits[i - 1][ past ].view());

          ++i;
        }

      // required for calibration
        output_idx = i;

      // create all output states - current is now i - 1 after last ++i
        for( auto& l_pos : logits[i] ){
          l_pos.take(current, logits[i - 1][ current ].view());
        }

      // scale and activate
        activation::min_max<num>(scaled, logits[i]);
        activation::softmax_scaled<num>(scaled);
      }

      template<class container>
      void calibrate( const container& packets ){

        for( size_t i{0}; i < out_size; ++i ){

          logits[ output_idx ][i].mul_error( scaled[i] - (num)(packets.get_trg() == i) );

          if( packets.get_trg() != i ){
            logits[ output_idx ][i].calibrate( packets[ output_idx - 1 ].first );
          }
        }

        size_t i{ output_idx }, past, current;
    
        while( 0 < i ){
          past    = packets[i - 1].first;
          current = packets[  i  ].first;

        // take derivative with respect to the future logit
          logits[i - 1][ past ].mul_error( logits[i][ current ].dy_dx(past, logits[i - 1][ past ].view()) );

          logits[i][ current ].calibrate(past);

          --i;
        }

        logits[0][ packets[0].first ].calibrate(0);       
      }

      void display_output(){
        for( const auto& s : scaled ){
          std::cout << s << ' ';
        } std::cout << '\n';
      }
      
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

}

#endif