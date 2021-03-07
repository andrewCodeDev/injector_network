#include <type_traits>
#include <functional>
#include <algorithm>
#include <execution>
#include <concepts>
#include <iterator>
#include <iostream>
#include <iomanip>
#include <numeric>
#include <vector>
#include <assert.h>
#include <cmath>

/*
  properties: 

    let p and q be polynumbers, and c be a scalar value:

      (p + q)(c) = p(c) + q(c);
      (p * q)(c) = p(c) * q(c);

      p(x) - p(y) has the factor (x - y) where x and y are poly numbers;

      if p(x) - p(0), then a - c divides p(x);

      Laurent Polynumber - power of poly({0, 1}) in base - used for negative powers
        
        ex: a(1.0f) / b.pow(2)(10.0f) = 0.01;


*/

template <
  std::floating_point num
> num factorial(std::size_t n){
  
  std::size_t prd{1};
  while( n > 1) { prd *= n--; } return static_cast<num>(prd);
}


template <
  std::floating_point num
> num dx_coef(std::size_t n_drv, std::size_t pos){
  
  int prd{1};
  while( n_drv-- > 0) { prd *= pos--; } return static_cast<num>(prd);
}

template <
  std::floating_point num
> num n_C_k(std::size_t n, std::size_t k){
  return factorial<num>(n) / (factorial<num>(k) * factorial<num>(n - k));
}


namespace poly {

  template <class T> void print(const T&);
  template <class T> void biprint(const T&);
  template <class T> void standard_form(const T&, int precision = 4);

  struct col_t { col_t() = default; };
  struct row_t { row_t() = default; };

  template<class p_num>
  concept is_col_t = std::is_same_v<col_t, typename p_num::dim>;
  
  template<class p_num>
  concept is_row_t = std::is_same_v<row_t, typename p_num::dim>;


namespace integer {

  template <
    std::integral  num_type,
    typename       dim_type
  > struct integral {

    using num = num_type;
    using dim = dim_type;

    integral() : data(1, 0) { };
    integral(std::size_t size) : data(size, 0) { };
    integral(std::initializer_list<num_type> l) : data(l) { };
    
    num_type operator[](std::size_t i) const { return ( i < data.size() ) ? data[i] : static_cast<num_type>(0); }

    auto begin() const { return data.begin(); }
    auto begin()       { return data.begin(); }
    auto end()   const { return data.end(); }
    auto end()         { return data.end(); }

    auto cbegin() const { return data.cbegin(); }
    auto cend()   const { return data.cend();   }

    auto rbegin() { return data.rbegin(); }
    auto rend()   { return data.rend();   }

    std::size_t size() const { return data.size(); }

    std::size_t degree() {
      return std::distance(std::find_if(data.rbegin(), data.rend(), [](const num_type& x) { return (x != static_cast<num_type>(0)); }), data.rend());
    }

    std::vector<num_type> data; 

    friend bool operator==(const integral& lhs, const integral& rhs) {

      auto lhs_itr = std::find_if( lhs.rbegin(), lhs.rend(), [](const num_type& x){ return (x != static_cast<num_type>(0)); });
      auto rhs_itr = std::find_if( rhs.rbegin(), rhs.rend(), [](const num_type& x){ return (x != static_cast<num_type>(0)); });

      if( std::distance(lhs_itr, lhs.rend()) == std::distance(rhs_itr, rhs.rend()) ){
        return std::equal(lhs_itr, lhs.rend(), rhs_itr);
      } else {
        return false;
      }
    }
  
    friend bool operator!=(const integral& lhs, const integral& rhs) { return !(lhs == rhs); }

    friend integral operator+(const integral& lhs, const integral& rhs) {
      
      integral tmp(std::max(lhs.data.size(), rhs.data.size()));

      for(std::size_t i{0}; i < tmp.size(); ++i) { tmp.data[i] = lhs[i] + rhs[i]; };

      return tmp;
    }

    friend integral operator-(const integral& lhs, const integral& rhs) {
      
      integral tmp(std::max(lhs.data.size(), rhs.data.size()));

      for(std::size_t i{0}; i < tmp.size(); ++i) { tmp.data[i] = lhs[i] - rhs[i]; };

      return tmp;
    }

    friend integral operator*(const integral& lhs, const integral& rhs) {

      integral tmp(lhs.size() + rhs.size() - 1);

      for( std::size_t i{0}; i < lhs.size(); ++i ) {
      for( std::size_t j{0}; j < rhs.size(); ++j )
        { tmp.data[i + j] += lhs.data[i] * rhs.data[j]; }
      } 
     
      return tmp;
    }

    friend integral operator*(const num_type& scalar, const integral& rhs) {
      
      integral tmp = rhs;

      std::transform(tmp.cbegin(), tmp.cend(), tmp.begin(), [&](const num_type& x) { return scalar * x; });

      return tmp;
    }

    friend integral operator*(const integral& lhs, const num_type& scalar) {
      return scalar * lhs;
    }

    integral pow(int exponent) {

      integral tmp = *this;
        
      while( exponent > 1 ){
        tmp = tmp * (*this); --exponent;
      }

      return tmp;
    }

    num_type operator()(num_type c){{
      return std::transform_reduce(std::execution::unseq,
        data.cbegin(), data.cend(), static_cast<num_type>(0), std::plus<num_type>(), [c, r{0}, p{1}](const num_type& x) mutable { r = x * p; p *= c; return r; });
      }
    }
  };

  template <
    std::integral num_type
  > using col = integral<num_type, col_t>;

  template <
    std::integral num_type
  > using row = integral<num_type, row_t>;

}

  namespace ratio {

    template <
      std::floating_point  num_type,
      typename             dim_type
    > struct rational {

      using num = num_type;
      using dim = dim_type;

      rational() : data(1, 0) { };
      rational(std::size_t size) : data(size, 0) { };
      rational(std::initializer_list<num_type> l) : data(l) { };
      
      num_type operator[](std::size_t i) const { return ( i < data.size() ) ? data[i] : static_cast<num_type>(0); }

      auto begin() const { return data.begin(); }
      auto begin()       { return data.begin(); }
      auto end()   const { return data.end(); }
      auto end()         { return data.end(); }

      auto cbegin() const { return data.cbegin(); }
      auto cend()   const { return data.cend();   }

      auto rbegin() { return data.rbegin(); }
      auto rend()   { return data.rend();   }

      std::size_t size() const { return data.size(); }

      std::size_t degree() const {
        return std::distance(std::find_if(data.rbegin(), data.rend(), [](const num_type& x) { return (x != static_cast<num_type>(0)); }), data.rend());
      }

      num dy_dx(const num& x, std::size_t n_drv) {
        
        if( !(n_drv < data.size()) ){ return 0.0; }

        else { 
          num sum{0}, prd{1};
          for(std::size_t i{n_drv}; i < data.size(); ++i, prd *= x) { sum += dx_coef<float>(n_drv, i) * data[i] * prd; }
          return sum;
        }
      }

      rational dy_dx(std::size_t n_drv) {
        
        if( !(n_drv < data.size()) ) { return rational{}; }

        else {
          rational tmp(data.size() - n_drv);
          std::transform( data.cbegin() + n_drv, data.cend(), tmp.begin(), [&, d{n_drv} ](const num& x) mutable { return x * dx_coef<num>(n_drv, d++); });
          return tmp;
        }
      }

      std::vector<num_type> data; 

      friend bool operator==(const rational& lhs, const rational& rhs) {

        auto lhs_itr = std::find_if( lhs.rbegin(), lhs.rend(), [](const num_type& x){ return (x != static_cast<num_type>(0)); });
        auto rhs_itr = std::find_if( rhs.rbegin(), rhs.rend(), [](const num_type& x){ return (x != static_cast<num_type>(0)); });

        if( std::distance(lhs_itr, lhs.rend()) == std::distance(rhs_itr, rhs.rend()) ){
          return std::equal(lhs_itr, lhs.rend(), rhs_itr);
        } else {
          return false;
        }
      }
    
      friend bool operator!=(const rational& lhs, const rational& rhs) { return !(lhs == rhs); }

      friend rational operator+(const rational& lhs, const rational& rhs) {
        
        rational tmp(std::max(lhs.data.size(), rhs.data.size()));

        std::copy(rhs.cbegin(), rhs.cend(), tmp.begin());
        std::transform(lhs.cbegin(), lhs.cend(), tmp.cbegin(), tmp.begin(), [](const num& x, const num& y){ return x + y; });

        return tmp;
      }

      friend rational operator-(const rational& lhs, const rational& rhs) {
        
        rational tmp(std::max(lhs.data.size(), rhs.data.size()));

        std::copy(rhs.cbegin(), rhs.cend(), tmp.begin());
        std::transform(lhs.cbegin(), lhs.cend(), tmp.cbegin(), tmp.begin(), [](const num& x, const num& y){ return x - y; });

        return tmp;
      }

      friend rational operator*(const rational& lhs, const rational& rhs) {

        rational tmp(lhs.size() + rhs.size() - 1);

        for( std::size_t i{0}; i < lhs.size(); ++i ) {
        for( std::size_t j{0}; j < rhs.size(); ++j )
          { tmp.data[i + j] += lhs.data[i] * rhs.data[j]; }
        } 
      
        return tmp;
      }

      friend rational operator*(const num_type& scalar, const rational& rhs) {
        
        rational tmp = rhs;

        std::transform(tmp.cbegin(), tmp.cend(), tmp.begin(), [&](const num_type& x) { return scalar * x; });

        return tmp;
      }

      friend rational operator*(const rational& lhs, const num_type& scalar) {
        return scalar * lhs;
      }

      friend rational operator/(rational lhs, const rational& rhs) { // remainder not yet suppoarted

        rational quo(0);

        auto lhs_itr = lhs.begin();
        auto rhs_itr = std::find_if(rhs.begin(), rhs.end(), [](const num_type& x) { return x != static_cast<num_type>(0); });

        while( std::distance(rhs_itr, rhs.end()) <= std::distance(lhs_itr, lhs.end()) ){

          if( *lhs_itr == static_cast<num_type>(0) ) { quo.data.push_back(static_cast<num_type>(0)); }

          else {
            

            num_type div = (*lhs_itr / *rhs_itr); quo.data.push_back(div);
            std::transform(rhs_itr, rhs.end(), lhs_itr, lhs_itr, [div](auto r, auto l){ return l - (r * div); });

            poly::print(lhs);

          } std::advance(lhs_itr, 1);
        } return quo;
      }

      rational pow(int exponent) {

        rational tmp = *this;
          
        while( exponent > 1 ){
          tmp = tmp * (*this); --exponent;
        }

        return tmp;
      }

      num_type operator()(num_type c){
        return std::transform_reduce(std::execution::unseq, data.cbegin(), data.cend(), static_cast<num_type>(0), std::plus<num_type>(),
         [c, r = static_cast<num>(0), p = static_cast<num>(1)](const num_type& x) mutable { r = x * p; p *= c; return r; }
        );
      }

    };

    template <
      std::floating_point  num_type,
      typename             dim_type
    > struct bi_rational {

      using num  = num_type;
      using dim  = dim_type;
      using rat  = rational<num, dim>;
      using grid = std::vector<rat>;
      using load = std::initializer_list<std::initializer_list<num>>;
    
      bi_rational() : data(1, {0}) { };

      bi_rational(load&& l) {
        data.reserve(l.size()); for(auto& init : l) {
          this->magnitude = std::max(init.size(), magnitude); data.emplace_back(std::move(init)); 
      }
    };

      rat operator[](std::size_t i) const { return (i < data.size()) ? data[i] : rat{}; }

      auto begin() const { return data.begin(); }
      auto begin()       { return data.begin(); }
      auto end()   const { return data.end(); }
      auto end()         { return data.end(); }

      auto cbegin() const { return data.cbegin(); }
      auto cend()   const { return data.cend();   }

      auto rbegin() { return data.rbegin(); }
      auto rend()   { return data.rend();   }

      void push_back(rat&& r) { this->magnitude = std::max(r.size(), magnitude); data.push_back(std::move(r)); }

      std::size_t size() const { return data.size(); }

      grid data; std::size_t magnitude{0};

      
    
      // friend bool operator!=(const rational& lhs, const rational& rhs) { return !(lhs == rhs); }

      friend bi_rational operator+(const bi_rational& lhs, const bi_rational& rhs) {
        
        bi_rational tmp; tmp.data.reserve(std::max(lhs.data.size(), rhs.data.size()));

        for(std::size_t i{0}; i < tmp.size(); ++i) { tmp.push_back( lhs[i] + rhs[i] ); };

        return tmp;
      }

      friend bi_rational operator-(const bi_rational& lhs, const bi_rational& rhs) {
        
        bi_rational tmp; tmp.data.reserve(std::max(lhs.data.size(), rhs.data.size()));

        for(std::size_t i{0}; i < tmp.size(); ++i) { tmp.push_back( lhs[i] - rhs[i] ); };

        return tmp;
      }

      friend bi_rational operator*(const bi_rational& lhs, const bi_rational& rhs) {

        std::size_t M{ lhs.size() + rhs.size() - 1 }, N{ lhs.magnitude + rhs.magnitude - 1 };

        bi_rational tmp; tmp.data.resize(M); for(auto& r : tmp) { r.data.resize(N); }; tmp.magnitude = N;

        for( std::size_t i{0}; i < lhs.size(); ++i )
        for( std::size_t j{0}; j < rhs.size(); ++j )
        for( std::size_t k{0}; k < lhs.data[i].size(); ++k )
        for( std::size_t l{0}; l < rhs.data[j].size(); ++l ){
          tmp.data[i + j].data[k + l] += lhs.data[i].data[k] * rhs.data[j].data[l];
        } return tmp;
      }

      friend bi_rational operator*(const num& scalar, const bi_rational& rhs) {
        
        bi_rational tmp; tmp.reserve( rhs.size() );

        for(const auto& poly_num : rhs) { tmp.push_back(scalar * poly_num); }

        return tmp;
      }

      friend bi_rational operator*(const bi_rational& lhs, const num& scalar) {
        return scalar * lhs;
      }


/*
      // friend rational operator-(const rational& lhs, const rational& rhs) {
        
      //   rational tmp(std::max(lhs.data.size(), rhs.data.size()));

      //   for(std::size_t i{0}; i < tmp.size(); ++i) { tmp.data[i] = lhs[i] - rhs[i]; };

      //   return tmp;
      // }

      // friend rational operator*(const rational& lhs, const rational& rhs) {

      //   rational tmp(lhs.size() + rhs.size() - 1);

      //   for( std::size_t i{0}; i < lhs.size(); ++i ) {
      //   for( std::size_t j{0}; j < rhs.size(); ++j )
      //     { tmp.data[i + j] += lhs.data[i] * rhs.data[j]; }
      //   } 
      
      //   return tmp;
      // }

      // friend rational operator*(const num_type& scalar, const rational& rhs) {
        
      //   rational tmp = rhs;

      //   std::transform(tmp.cbegin(), tmp.cend(), tmp.begin(), [&](const num_type& x) { return scalar * x; });

      //   return tmp;
      // }

      // friend rational operator*(const rational& lhs, const num_type& scalar) {
      //   return scalar * lhs;
      // }

      // friend rational operator/(rational lhs, const rational& rhs) { // remainder not yet suppoarted

      //   rational quo(0), dif(rhs.size());

      //   auto lhs_itr = lhs.begin();
      //   auto rhs_itr = std::find_if(rhs.begin(), rhs.end(), [](const num_type& x) { return x != static_cast<num_type>(0); });

      //   while( std::distance(rhs_itr, rhs.end()) <= std::distance(lhs_itr, lhs.end()) ){

      //     if( *lhs_itr == static_cast<num_type>(0) ) { quo.data.push_back(static_cast<num_type>(0)); }

      //     else {
      //       num_type div = (*lhs_itr / *rhs_itr); quo.data.push_back(div);
      //       std::transform(rhs_itr, rhs.end(), lhs_itr, lhs_itr, [div](auto r, auto l){ return l - (r * div); });
      //     } std::advance(lhs_itr, 1);
      //   } return quo;
      // }

      rational pow(int exponent) {

        rational tmp = *this;
          
        while( exponent > 1 ){
          tmp = tmp * (*this); --exponent;
        }

        return tmp;
      }

      num_type operator()(num_type c){
        return std::transform_reduce(std::execution::unseq,
          data.cbegin(), data.cend(), static_cast<num_type>(0), std::plus<num_type>(), [c, r{0}, p{1}](const num_type& x) mutable { r = x * p; p *= c; return r; }
        );
      }

*/
    };

    template <
      std::floating_point num_type
    > using col = rational<num_type, col_t>;

    template <
      std::floating_point num_type
    > using row = rational<num_type, row_t>;


    template <
      std::floating_point num_type
    > using bi = bi_rational<num_type, row_t>;

    template <
      std::floating_point num_type
    > using bi = bi_rational<num_type, row_t>;
  }

    template <
      std::floating_point num_type
    > ratio::col<num_type> alpha_col() { return col<num_type>({0, 1}); }

    template <
      std::floating_point num_type
    > ratio::row<num_type> alpha_row() { return row<num_type>({0, 1}); }

  template <
    class T
  > void poly::print(const T& poly_num) 
  {
    if constexpr ( is_row_t<T> ) {
      std::cout << "| ";
      for(const auto& e : poly_num) {
        std::cout << e << ", "; 
      } std::cout << "\b\b |\n\n"; 
    }

    if constexpr ( is_col_t<T> ) {
      std::cout << " -\n";
      for(const auto& e : poly_num) {
        if( e > static_cast<T::num>(0) ) {
          std::cout << ' ' << e << ",\n"; 
        } else { std::cout << e << ",\n"; }
      } std::cout << " -\n\n"; 
    }
  }

  template <
    class T
  > void lprint( const T& poly_num ){
  
    for(std::size_t i{0}; i < poly_num.size(); ++i) {
      if( i == (poly_num.size() - 1)) {
        std::cout << poly_num[i].coef << "/(1 + e^(" << poly_num[i].rate << "(x - " << poly_num[i].trns << ")))";
      } else {
        std::cout << poly_num[i].coef << "/(1 + e^(" << poly_num[i].rate << "(x - " << poly_num[i].trns << "))) + ";
      }
    } std::cout << "\n\n";
  }

  template <
    class T
  > void poly::standard_form(const T& poly_num, int precision) {
    
    std::cout << std::fixed << std::setprecision(precision);

    for(std::size_t count{0}; count < poly_num.size(); ++count) {
      if(count < poly_num.size() - 1) {
        std::cout << '(' << poly_num.data[count] << "x^" << count << ") + ";
      } else {
        std::cout << '(' << poly_num.data[count]  << "x^" << count << ')';
      }
    } std::cout << "\n\n"; 
  }

  template <
  std::floating_point num_type
  > void calibrate(auto& poly_num, num_type input, num_type error, num_type lr = 0.001){
    
    num_type p{1};

    for(std::size_t i{0}; i < poly_num.size(); ++i){
      poly_num.data[i] -= p * error * lr;

      p *= input;
    }
  }

  template <
    class T
  > void poly::biprint(const T& bi_num) 
  {
    for(auto& poly_num : bi_num) {
      std::cout << "| ";
      for(const auto& e : poly_num) {
        std::cout << e << ", "; 
      } std::cout << "\b\b |\n"; 
    }   std::cout << '\n';
  }


template <
  std::floating_point num_type
> class matrix {

public:
  using num = num_type;

  matrix() : rows(1), cols(1), data(1, 0) { };
  matrix(std::size_t m, std::size_t n) : rows(m), cols(n), data(m*n, 0) { };

  num& operator()(std::size_t i, std::size_t j)       { return data[i * cols + j]; }
  num  operator()(std::size_t i, std::size_t j) const { return data[i * cols + j]; }

  auto begin() const { return data.begin(); }
  auto begin()       { return data.begin(); }
  auto end()   const { return data.end(); }
  auto end()         { return data.end(); }

  auto cbegin() const { return data.cbegin(); }
  auto cend()   const { return data.cend();   }

  auto rbegin() { return data.rbegin(); }
  auto rend()   { return data.rend();   }

  const std::size_t rows{1}, cols{1};

private:
  std::vector<num_type> data;
};



  template <
    std::floating_point FT
  > FT rand_gen(){ return (FT)rand() / (FT)RAND_MAX * (FT)2 - (FT)1; }


    template <
      std::floating_point num
    > class logit {

    private:

      struct modifier { num coef, rate, trns; };

      std::vector<modifier> data; num output;
    
    public:
      logit() : data(1, 0) { };

      logit(std::size_t size){
        data.resize(size); 
        for( modifier& m : data ){ 
          m.coef = poly::rand_gen<num>();
          m.rate = poly::rand_gen<num>();
          m.trns = poly::rand_gen<num>();
        }
      };
      
      modifier operator[](std::size_t i) const { return data[i]; }

      auto begin() const { return data.begin(); }
      auto begin()       { return data.begin(); }
      auto end()   const { return data.end(); }
      auto end()         { return data.end(); }

      auto cbegin() const { return data.cbegin(); }
      auto cend()   const { return data.cend();   }

      auto rbegin() { return data.rbegin(); }
      auto rend()   { return data.rend();   }

      std::size_t size() const { return data.size(); }

      num operator()(const num& x ){
        output = std::transform_reduce(std::execution::seq, data.cbegin(), data.cend(), (num)0, std::plus<num>{},
          [x, one = (num)1](const modifier& m) mutable
          {
            return m.coef / (one + std::exp(m.rate * (x - m.trns)));
          });
        return output;
      }

      num dy_dx(const num& x, std::size_t n_drv = 1) {
 
        assert( 0 < n_drv && n_drv < 3 );

        num one{1}, e_p{0};

        if( n_drv == 1 ) {

          return std::transform_reduce(data.cbegin(), data.cend(), (num)0, std::plus<num>{},
            [=](const modifier& m) mutable 
            { 
              e_p = std::exp(m.rate * (x - m.trns));
              return -(m.coef * m.rate * e_p) / ((one + e_p) * (one + e_p));
            });
        }
        
        if( n_drv == 2 ) {

          return std::transform_reduce(data.cbegin(), data.cend(), (num)0, std::plus<num>{}, 
            [=](const modifier& m) mutable 
            { 
              e_p = std::exp(m.rate * (x - m.trns));
              return -(m.coef * m.rate * m.rate * e_p * (-e_p + one)) / ((one + e_p) * (one + e_p) * (one + e_p));
            });
          }

          return 0.0f; // otherwise it complains
        }

      void calibrate(const num& x, const num& error, num lr = 0.01){

        num one{1}, adj{error * lr}, e_p, denom;

          // NOTE! derivative with repect to c is the negation of the derivative with respect to x
          
        for( modifier& m : data ) {
          e_p = std::exp(m.rate * (x - m.trns)); denom = (one + e_p) * (one + e_p);

          // std::cout << "rate: " <<  ((-m.coef * (x - m.trns) * e_p ) / denom ) << '\n';
          // std::cout << "trns: " <<  (( m.coef * m.rate * e_p ) / denom ) << '\n';
          // std::cout << "coef: " <<  (one / (one + e_p)) << "\n\n";
          
          m.rate -= ((-m.coef * (x - m.trns) * e_p ) / denom ) * adj;
          m.trns -= (( m.coef * m.rate * e_p ) / denom ) * adj;
          m.coef -= (one / (one + e_p)) * adj;
        }
      }

  };

  template <
    std::floating_point num
  > void dot(const matrix<num>& X, const matrix<num>& Y, matrix<num>& Z) {

    num scale{ static_cast<num>(1) / static_cast<num>(X.cols) };

    for(std::size_t i{0}; i < X.rows; ++i)
    for(std::size_t j{0}; j < Y.cols; ++j){
      num Z_tmp = 0;
    for(std::size_t k{0}; k < X.cols; ++k){
      Z_tmp += X(i, k) * Y(k, j);
    } Z(i, j) = Z_tmp * scale;
    }
  }

template <
  std::floating_point FT
> FT sigmoid(FT x){ return (FT)1 / ((FT)1 + std::exp(-x)); }

template <class mtx> 
void mprint(const mtx& X) {
  for(std::size_t i{0}; i < X.rows; ++i){
  for(std::size_t j{0}; j < X.cols; ++j){
    std::cout << X(i, j) << ' ';
  } std::cout << '\n';
  } std::cout << '\n';
}

template <
  std::floating_point FT
> FT stepwise(FT x){ return (x > FT(0)) ? (FT)1 : (FT)0; }


//~~~~~~~~~~~~~~~~~~~~~~~~
} // end of poly namespace
//~~~~~~~~~~~~~~~~~~~~~~~~



int main(void) {
  

  poly::logit<float> l(4);

  float x, y, p, e;

  poly::lprint(l);

  for(int i{0}; i < 1'000'000; ++i) {

    x = poly::rand_gen<float>();
    y = std::sin(x);

    p = l(x);
    e = p - y;

    l.calibrate(x, e);

}

  poly::lprint(l);

//~~~~~~~~~~~~~~~~~~
  std::cout << '\n';
  return 0;

}