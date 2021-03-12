#ifndef ACTIVATION_H
#define ACTIVATION_H

#include <algorithm>
#include <execution>
#include <concepts>
#include <numeric>
#include <limits>
#include <cmath>

namespace activation {

  template<std::floating_point num> void softmax( auto& logits ){

    num denominator = std::transform_reduce( std::execution::seq, 
      logits.cbegin(), logits.cend(), static_cast<num>(0), std::plus<num>{}, [](const auto& l_pos) { return std::exp(l_pos.send()); }
    );
    
    std::transform( std::execution::seq,
      logits.cbegin(), logits.cend(), logits.begin(), [&denominator](const auto& l_pos){ return std::exp(l_pos.send()) / denominator; }
    );
  }


  template <std::floating_point num> void abs_max( auto& logits ){

    num min_stm = std::max_element(logits.cbegin(), logits.cend())->send();
    num max_stm = std::min_element(logits.cbegin(), logits.cend())->send();
    num rcp     = static_cast<num>(1) / (max_stm - min_stm);

    for(auto& x : logits) { x = (x.send() - min_stm) / rcp; x.mul_error(rcp); }
  }

}

namespace indexing {
  
  template<std::floating_point num>
  float linear(num x, num pos, num incr, num length)
  { 
    return -( (2.0f * x - 1.0f) * (incr - pos) / length ) + x ;
  }
}

#endif