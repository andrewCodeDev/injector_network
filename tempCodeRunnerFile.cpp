      for( std::size_t i{1}; i < rows - 1; ++i ){
        for( logit<num>& l_pos : logits[i] ) { l_pos.initialize(cols, n_terms); }
      }