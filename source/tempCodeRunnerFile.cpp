  inj_net.reset_logits();

  inj_net.forward(o_h2);
  std::cout << o_h1.idx_to_char( idx_sampler.sample_top(inj_net.output()) );