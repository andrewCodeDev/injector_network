#ifndef ACTIVATION_H
#define ACTIVATION_H

#include <algorithm>
#include <execution>
#include <concepts>
#include <iostream>
#include <numeric>
#include <limits>
#include <cmath>

namespace activation {

  template<std::floating_point num> void softmax_diff( auto& logits ){

    num denominator = std::transform_reduce( std::execution::seq, 
      logits.cbegin(), logits.cend(), static_cast<num>(0), std::plus<num>{}, [](const auto& l_pos) { return std::exp(l_pos.view() + (num)1e-8); }
    );
    
    std::transform( std::execution::seq,
      logits.cbegin(), logits.cend(), logits.begin(), [&denominator](const auto& l_pos){ return std::exp(l_pos.view()) /(denominator + 1e-8); }
    );
  }

  template<std::floating_point num> void softmax_scaled( auto& logits ){

    num denominator = std::transform_reduce( std::execution::seq, 
      logits.cbegin(), logits.cend(), static_cast<num>(0), std::plus<num>{}, [](const auto& l_pos) { return std::exp(l_pos + (num)1e-8); }
    );
    
    std::transform( std::execution::seq,
      logits.cbegin(), logits.cend(), logits.begin(), [&denominator](const auto& l_pos){ return std::exp(l_pos) /(denominator + 1e-8); }
    );
  }


  template<std::floating_point num> void softmax( auto& logits ){

    num denominator = std::transform_reduce( std::execution::seq, 
      logits.cbegin(), logits.cend(), static_cast<num>(0), std::plus<num>{}, [](const auto& l_pos) { return std::exp(l_pos.view() + 1e-8); }
    );
    
    for( auto & l_pos : logits ){ 
      l_pos = std::exp(l_pos.view()) / (denominator + (num)1e-8);
      l_pos.mul_error(l_pos.view() * ((num)1 - l_pos.view()));
    }
  }

  template <std::floating_point num> void min_max(auto& arr, auto& logits ){

    num min_stm = (*std::min_element(logits.cbegin(), logits.cend())).view();
    num max_stm = (*std::max_element(logits.cbegin(), logits.cend())).view();
    num rcp     = static_cast<num>(1) / (max_stm - min_stm + (num)1e-8);

    for( size_t i{0}; i < logits.size(); ++i ) {
       arr[i] = (logits[i].view() - min_stm) * rcp; logits[i].mul_error(rcp); 
    }
  }

 
/* 
  template <std::floating_point num> void abs_max( auto& logits ){

    num maxm = 0;

    for( auto& l_pos : logits ){ maxm = std::max(maxm, std::abs(l_pos)); }

    if(maxm < (num)1){ return; }

    else {
      maxm = pow(static_cast<num>(10), -floor(log10(floor(maxm * static_cast<num>(10)))));
    
      for( auto& l_pos : logits ){ l_pos = l_pos * maxm; l_pos.mul_error(maxm);}
    }

  }
*/

}

// namespace indexing {
  
//   template<std::floating_point num>
//   float linear(num x, num pos, num incr, num length)
//   { 
//     return -( (2.0f * x - 1.0f) * (incr - pos) / length ) + x ;
//   }
// }

#endif