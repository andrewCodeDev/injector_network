#include <bits/stdc++.h>



int main(void){

  std::cout << std::boolalpha;


  running_deriv t;

  std::vector<float> vec2{1, 0, 1};




  

  for( std::size_t i{0}; i < vec2.size(); ++i ){
    std::cout << t.forward(vec2[i]) << '\n';
    std::cout << t.d_theta << '\n';
  } std::cout << '\n';


  // std::cout << (t.f(t.theta + 1e-4f, 1.0f + t.fp(t.theta + 1e-4f, 0.0f + t.fp(t.theta + 1e-4f, 1.0f))) - t.f(t.theta, 1.0f + t.fp(t.theta, 0.0f + t.fp(t.theta, 1.0f)))) / 1e-4f;

  return 0;
}