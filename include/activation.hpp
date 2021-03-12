#ifndef ACTIVATION_H
#define ACTIVATION_H

#include <algorithm>
#include <execution>
#include <concepts>
#include <numeric>
#include <cmath>

namespace activation {

  template<std::floating_point num> void softmax(auto& cntr){
    num denominator = std::transform_reduce( std::execution::seq, 
      cntr.cbegin(), cntr.cend(), static_cast<num>(0), std::plus<num>{}, [](const auto& l_pos) { return std::exp(l_pos.send()); }
    );
    
    std::transform( std::execution::seq,
      cntr.cbegin(), cntr.cend(), cntr.begin(), [&denominator](const auto& l_pos){ return std::exp(l_pos.send()) / denominator; }
    );
  }

  template<std::floating_point num> void sigmoid(auto& cntr){
    auto start = cntr.begin();

    *start = (num)1 / ((num)1 + std::exp(-start->send()));
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