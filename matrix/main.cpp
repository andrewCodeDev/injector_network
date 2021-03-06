#include <vector>
#include <algorithm>
#include <numeric>
#include <utility>
#include <concepts>
#include <assert.h>
#include <iostream>

auto mul = [](const size_t& lhs, const size_t& rhs) { return lhs * rhs; };

template <std::floating_point num> class ingress {

  public:
    ingress( std::initializer_list<size_t> l ) : dim(l.begin(), l.end()), mod(l.size(), 1) 
    {
      for(int i{(l.size() & 1) ? 1 : 0}; i < l.size() - 1; i += 2) { mod[i] = dim[i + 1]; }
    }

    std::size_t operator()(const std::initializer_list<size_t>& l) 
    { 
      return std::inner_product(l.begin(), l.end(), mod.rbegin(), 0);
    };
  
  private:
    std::vector<size_t> dim, mod;
};

int main(void){

  ingress<float> ing{2, 3, 3};

  std::cout << ing({, 2, 2}) << '\n';


  return 0;

};