  inj_net.reset_logits();

  inj_net.forward(p_enc1);
  std::cout << p_enc1.idx_to_char( idx_sampler.sample_top(inj_net.output()) );

  inj_net.reset_logits();

  inj_net.forward(p_enc2);
  std::cout << p_enc1.idx_to_char( idx_sampler.sample_top(inj_net.output()) );

  inj_net.reset_logits();  

  inj_net.forward(p_enc3);
  std::cout << p_enc1.idx_to_char( idx_sampler.sample_top(inj_net.output()) );