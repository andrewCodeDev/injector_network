#ifndef LOGITS_H
#define LOGITS_H

#include "sensors.hpp"


namespace logit {

  struct imd { };
  struct seq { };

  template < 
    std::floating_point num
  > class immediate {

  public:
    using sns_type = sensor::IMDsensor<num>;
    using  sns_ref = sns_type&;
    using csns_ref = const sns_ref&;

    immediate() = default;
    immediate( const std::size_t& num_s ){ sensors.resize(num_s); };

    immediate( std::size_t num_s, std::size_t s_size ){ initialize(num_s, s_size); };

    void initialize( std::size_t num_s, std::size_t s_size ){
      sensors.reserve(num_s); 
      weights.reserve(num_s); 
      while( 0 < num_s-- ){ 
        sensors.emplace_back(s_size);
        weights.emplace_back(rand_gen<num>());
      }
    }
  
    immediate& operator=(const immediate& other) = default;
    immediate& operator=(const num& x){ stimuli = x; return *this; }
    

    csns_ref operator[](const std::size_t& i ) const { return sensors[i]; }
     sns_ref operator[](const std::size_t& i )       { return sensors[i]; }


    void operator()(const std::size_t& i, const num& x ){ stimuli += weights[i] * sensors[i](x); }

    num operator()() const { return stimuli; }
 
    void add_sensor( std::size_t new_s, const std::size_t& n_terms ){
      while( 0 < new_s-- ){
        sensors.emplace_back(sns_type(n_terms));
        weights.emplace_back(rand_gen<num>());
      }; 
        sensors.shrink_to_fit();
        weights.shrink_to_fit();
    };

    void rmv_sensor( std::size_t rmv_s ){
      while( 0 < rmv_s-- ){ sensors.pop_back(); }
    }

    template<class functor> void transform(const functor& f ){ stimuli = f(stimuli); }

    void calibrate( const std::size_t& i, const num& x ){
      
      sensors[i].calibrate(x, error * weights[i]); 
      weights[i] -= error * sensors[i].signal * (num)0.01;
      
    }

    num dy_dx( const std::size_t& i, const num& x ){ return sensors[i].dy_dx(x) * error; }

    void add_error( const num& dx ){ error += dx; }
    void mul_error( const num& dx ){ error *= dx; }
    num  get_error(){ return error; }
    void show_error() { std::cout << "error: " << error << '\n'; }

    auto begin() const { return sensors.begin(); }
    auto end()   const { return sensors.end();   }

    auto cbegin() const { return sensors.cbegin(); }
    auto cend()   const { return sensors.cend();   }

    auto begin() { return sensors.begin(); }
    auto end()   { return sensors.end();   }

    auto rbegin() { return sensors.rbegin(); }
    auto rend()   { return sensors.rend();   }

    void reset() { stimuli = 0; error = 1; }

    std::size_t size() const { return sensors.size(); }

    friend bool operator==( const immediate& lhs, const immediate& rhs ){ return lhs.stimuli == rhs.stimuli; }
    friend bool operator!=( const immediate& lhs, const immediate& rhs ){ return lhs.stimuli != rhs.stimuli; }
    friend bool operator<=( const immediate& lhs, const immediate& rhs ){ return lhs.stimuli <= rhs.stimuli; }
    friend bool operator>=( const immediate& lhs, const immediate& rhs ){ return lhs.stimuli >= rhs.stimuli; }
    friend bool operator< ( const immediate& lhs, const immediate& rhs ){ return lhs.stimuli <  rhs.stimuli; }
    friend bool operator> ( const immediate& lhs, const immediate& rhs ){ return lhs.stimuli >  rhs.stimuli; }

  private:
    num stimuli{0}, error{1};
    std::vector<num> weights;
    std::vector<sensor::IMDsensor<num>> sensors;
  };


  template < 
    std::floating_point num
  > class sequential {

    public:
      using sns_type = sensor::SEQsensor<num>;
      using  sns_ref = sensor::SEQsensor<num>&;
      using csns_ref = const sensor::SEQsensor<num>&;

      sequential() = default;
      sequential( const std::size_t& num_s ){ sensors.resize(num_s); };

      sequential( std::size_t num_s, std::size_t s_size ){ initialize(num_s, s_size); };

      void initialize( std::size_t num_s, std::size_t s_size ){
        sensors.reserve(num_s); 
        weights.reserve(num_s); 
        while( 0 < num_s-- ){ 
          sensors.emplace_back(s_size);
          weights.emplace_back(rand_gen<num>());
        }
      }
    
      sequential& operator=(const sequential& other) = default;
      sequential& operator=(const num& x){ stimuli = x; return *this; }

       sns_ref operator[](const std::size_t& i )       { return sensors[i]; }
      csns_ref operator[](const std::size_t& i ) const { return sensors[i]; }

      num  operator()() const { return stimuli; }
      void operator()( const std::size_t& i, const num& x ){ stimuli += weights[i] * sensors[i](x); }

      void add_sensor( std::size_t new_s, const std::size_t& n_terms ){
        while( 0 < new_s-- ){
          sensors.emplace_back(sns_type(n_terms));
          weights.emplace_back(rand_gen<num>());
        }; 
          sensors.shrink_to_fit();
          weights.shrink_to_fit();
      };

      void rmv_sensor( std::size_t rmv_s ){
        while( 0 < rmv_s-- ){ sensors.pop_back(); }
      }

      template<class functor> void transform(const functor& f ){ stimuli = f(stimuli); }

      void calibrate( num lr = 0.01 ){
        
        for( std::size_t i{0}; i < sensors.size(); ++i ){
          sensors[i].calibrate(error * weights[i], lr); 
          weights[i] -= error * sensors[i].signal * lr;
        }
        error = 1; stimuli = 0;
      }

      num dy_dx( const std::size_t& i, const num& x ){ return sensors[i].dy_dx(x) * error; }

      void add_error( const num& dx ){ error += dx; }
      void mul_error( const num& dx ){ error *= dx; }
      num  get_error(){ return error; }
      void zero_error() { error = 1; }
      void show_error() { std::cout << "error: " << error << '\n'; }

      auto begin() const { return sensors.begin(); }
      auto end()   const { return sensors.end();   }

      auto cbegin() const { return sensors.cbegin(); }
      auto cend()   const { return sensors.cend();   }

      auto begin() { return sensors.begin(); }
      auto end()   { return sensors.end();   }

      auto rbegin() { return sensors.rbegin(); }
      auto rend()   { return sensors.rend();   }

      std::size_t size() const { return sensors.size(); }

      friend bool operator==( const sequential& lhs, const sequential& rhs ){ return lhs.stimuli == rhs.stimuli; }
      friend bool operator!=( const sequential& lhs, const sequential& rhs ){ return lhs.stimuli != rhs.stimuli; }
      friend bool operator<=( const sequential& lhs, const sequential& rhs ){ return lhs.stimuli <= rhs.stimuli; }
      friend bool operator>=( const sequential& lhs, const sequential& rhs ){ return lhs.stimuli >= rhs.stimuli; }
      friend bool operator< ( const sequential& lhs, const sequential& rhs ){ return lhs.stimuli <  rhs.stimuli; }
      friend bool operator> ( const sequential& lhs, const sequential& rhs ){ return lhs.stimuli >  rhs.stimuli; }

    private:
      num stimuli{0}, error{1};
      std::vector<num> weights;
      std::vector<sensor::SEQsensor<num>> sensors;
  };


}

#endif