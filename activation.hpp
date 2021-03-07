#ifndef ACTIVATION_H
#define ACTIVATION_H

namespace activation {

  template<std::floating_point num> void softmax(auto& cntr){
    num denominator = std::transform_reduce( std::execution::seq, 
      cntr.cbegin(), cntr.cend(), static_cast<num>(0), std::plus<num>{}, [](const auto& l_pos) { return std::exp(l_pos.send()); }
    );
    
    std::transform( std::execution::seq,
      cntr.cbegin(), cntr.cend(), cntr.begin(), [&denominator](const auto& l_pos){ return std::exp(l_pos.send()) / denominator; }
    );
  }

}

#endif