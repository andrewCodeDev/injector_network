      for( logit<num>& l_pos : logits.front() ){
      do{
        l_pos.calibrate(s_idx, (num)cols - inp_memo[s_idx]);
      }while( ++s_idx < inp_memo.size() );
        s_idx = 0; l_pos.zero_error();
      }