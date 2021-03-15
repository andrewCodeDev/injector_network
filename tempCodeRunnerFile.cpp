
  for( std::size_t i{0}; i < vec1.size(); ++i ){
    std::cout << test.forward(vec2[i]) << '\t';
    std::cout << test.d_theta << '\t';
  } std::cout << '\n';