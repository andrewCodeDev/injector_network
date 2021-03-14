#ifndef LOGITS_H
#define LOGITS_H

#include "sensors.hpp"


namespace logit {

  struct imd { };
  struct seq { };


  // logit base ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  
  template<std::floating_point num, typename sns_type> class logit_base {

    public:

      const sns_type& operator[](const std::size_t& i ) const { return sensors[i]; }
            sns_type& operator[](const std::size_t& i )       { return sensors[i]; }

      void initialize( std::size_t num_s, std::size_t s_size ){
        sensors.reserve(num_s); 
        weights.reserve(num_s); 
        while( 0 < num_s-- ){ 
          sensors.emplace_back(s_size);
          weights.emplace_back(rand_gen<num>());
        }
      }

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

      void add_error( const num& dx ){ error += dx; }
      void mul_error( const num& dx ){ error *= dx; }
      num  get_error(){ return error; }

      friend bool operator==( const logit_base& lhs, const logit_base& rhs ){ return lhs.stimuli == rhs.stimuli; }
      friend bool operator!=( const logit_base& lhs, const logit_base& rhs ){ return lhs.stimuli != rhs.stimuli; }
      friend bool operator<=( const logit_base& lhs, const logit_base& rhs ){ return lhs.stimuli <= rhs.stimuli; }
      friend bool operator>=( const logit_base& lhs, const logit_base& rhs ){ return lhs.stimuli >= rhs.stimuli; }
      friend bool operator< ( const logit_base& lhs, const logit_base& rhs ){ return lhs.stimuli <  rhs.stimuli; }
      friend bool operator> ( const logit_base& lhs, const logit_base& rhs ){ return lhs.stimuli >  rhs.stimuli; }
  
    protected:
      num stimuli{0}, error{1};
      std::vector<num> weights;
      std::vector<sns_type> sensors;
  };


  // logit types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


  template < 
    std::floating_point num
  > class immediate : public logit_base<num, sensor::IMDsensor<num>>{

    public:
      immediate() = default;
      immediate( std::size_t num_s, std::size_t s_size ){ this->initialize(num_s, s_size); };

      num  operator()() const { return this->stimuli; }
      void operator()(  const std::size_t& i, const num& x ){ this->stimuli += this->weights[i] * this->sensors[i](x); }

      immediate& operator=( const immediate& other ) = default;
      immediate& operator=( const num& x ){ this->stimuli = x; return *this; }

      void calibrate ( const std::size_t& i, const num& x, num lr = 0.01 ){
        this->sensors[i].calibrate(x, this->error * this->weights[i]); 
        this->weights[i] -= this->error * this->sensors[i].signal * lr;
      }

      num dy_dx( const std::size_t& i, const num& x ){ return this->weights[i] * this->sensors[i].dy_dx(x) * this->error; }
  };


  template < 
    std::floating_point num
  > class sequential : public logit_base<num, sensor::SEQsensor<num>>{

    public:
      sequential() = default;
      sequential( std::size_t num_s, std::size_t s_size ){ this->initialize(num_s, s_size); };

      num  operator()() const { return this->stimuli; }
      void operator()(  const std::size_t& i, const num& x ){ this->stimuli += this->weights[i] * this->sensors[i](x); }

      sequential& operator=( const sequential& other ) = default;
      sequential& operator=( const num& x ){ this->stimuli = x; return *this; }

      void calibrate( num lr = 0.01 ){
        
        for( std::size_t i{0}; i < this->sensors.size(); ++i ){
          this->sensors[i].calibrate(this->error * this->weights[i], lr); 
          this->weights[i] -= this->error * this->sensors[i].signal * lr;
        }
        this->reset();
      }

      num dy_dx( const std::size_t& i, const num& x ){ return this->weights[i] * this->sensors[i].dy_dx(x) * this->error; }; 

  };


}

#endif