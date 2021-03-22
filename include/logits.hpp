#ifndef LOGITS_H
#define LOGITS_H

#include "sensors.hpp"


namespace logit {

  // logit base ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  
  template<std::floating_point num, typename sns_type> class logit_base {

    public:

      using float_type = num;

      logit_base() = default;
      logit_base( size_t num_s, size_t s_size ){ initialize(num_s, s_size); };
      
      logit_base& operator=( const logit_base& other ) = default;
      logit_base& operator=( const num& x ){ stimuli = x; return *this; }

    // sensor creation ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

      void initialize( size_t num_s, const size_t& s_size ){
        bias.reserve(num_s);
        weights.reserve(num_s);
        sensors.reserve(num_s); 

        avg_rcp = (num)1 / (num)num_s;

        while( 0 < num_s-- ){ 
          bias.emplace_back(rand_gen<num>());
          weights.emplace_back(rand_gen<num>());
          sensors.emplace_back(s_size);
        }
      }

      void add_sensor( size_t new_s, const size_t& n_terms ){
        while( 0 < new_s-- ){
          sensors.emplace_back(sns_type(n_terms));
          weights.emplace_back(rand_gen<num>());
        }; 
          sensors.shrink_to_fit();
          weights.shrink_to_fit();
      };

    // stimuli functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~

      void take(const size_t& i, const num& x ){ 
        stimuli += avg_rcp * (bias[i] + weights[i] * sensors[i](x));
      }

      num view() const { return stimuli; }

      num send(){ sent = true; return stimuli; }

    // error functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

      void calibrate( const size_t& i, num lr = 0.01 ){

          weights[i] -= error * avg_rcp * sensors[i] * lr;
          bias[i]    -= error * avg_rcp * lr;
          sensors[i].calibrate(error * weights[i] * avg_rcp, lr); 

          this->logit_reset();
      }

      void mul_error( const num& dx ){ error *= dx; }

      num view_error(){ return error; }

      num dy_dx( const size_t& i, const num& x ) { return avg_rcp * error * sensors[i].dy_dx(x) * weights[i]; }

    // reset functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

      void logit_reset(){ 
        stimuli = 0;
        error   = 1;
        sent    = false;
      }
      
      void sensor_reset(){ 
        for( auto& sns : sensors ){ sns.reset(); };
      }

      void full_reset() {
        logit_reset();
        sensor_reset();
      }

    // helper functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

      operator bool() { return sent; }

      size_t size() const { return sensors.size(); }

      const sns_type& operator[](const size_t& i ) const { return sensors[i]; }
            sns_type& operator[](const size_t& i )       { return sensors[i]; }

      friend bool operator==( const logit_base& lhs, const logit_base& rhs ){ return lhs.stimuli == rhs.stimuli; }
      friend bool operator!=( const logit_base& lhs, const logit_base& rhs ){ return lhs.stimuli != rhs.stimuli; }
      friend bool operator<=( const logit_base& lhs, const logit_base& rhs ){ return lhs.stimuli <= rhs.stimuli; }
      friend bool operator>=( const logit_base& lhs, const logit_base& rhs ){ return lhs.stimuli >= rhs.stimuli; }
      friend bool operator< ( const logit_base& lhs, const logit_base& rhs ){ return lhs.stimuli <  rhs.stimuli; }
      friend bool operator> ( const logit_base& lhs, const logit_base& rhs ){ return lhs.stimuli >  rhs.stimuli; }
  
    protected:

      bool sent{false};
      std::vector<sns_type> sensors;
      std::vector<num> weights, bias;
      num stimuli{0}, error{1}, avg_rcp{1};
  };


  // logit types based on sensor ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  // shorthand for network arguments

  struct imd { };
  struct seq { };

  // logit type defs ~~~~~~~~~~~~~~

  template<std::floating_point num>
  using immediate = logit_base<num, sensor::immediate<num>>;

  template<std::floating_point num>
  using sequential = logit_base<num, sensor::sequential<num>>;


}

#endif