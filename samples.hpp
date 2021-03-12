#ifndef SAMPLES_H
#define SAMPLES_H

#include <vector>

// static const unsigned gsamp_size{4};

namespace set2x1 {

  namespace and_gate {
    static const std::vector<std::vector<float>> inp{{0, 0}, {1, 0}, {0, 1}, {1, 1}};
    static const std::vector<std::vector<float>> trg{{0},    {0},    {0},    {1}};
  }

  namespace nand_gate {
    static const std::vector<std::vector<float>> inp{{0, 0}, {1, 0}, {0, 1}, {1, 1}};
    static const std::vector<std::vector<float>> trg{{1},    {1},    {1},    {0}};
  }

  // namespace or_gate {
  //   static const std::vector<std::vector<float>> inp{{0, 0}, {1, 0}, {0, 1}, {1, 1}};
  //   static const std::vector<std::vector<float>> trg{{0, 1}, {1, 0}, {1, 0}, {1, 0}};
  // }

  // namespace nor_gate {
  //   static const std::vector<std::vector<float>> inp{{0, 0}, {1, 0}, {0, 1}, {1, 1}};
  //   static const std::vector<std::vector<float>> trg{{1, 0}, {0, 1}, {0, 1}, {0, 1}};
  // }

  // namespace p_nq_and {
  //   static const std::vector<std::vector<float>> inp{{0, 1}, {1, 0}, {0, 1}, {1, 1}};
  //   static const std::vector<std::vector<float>> trg{{0, 1}, {1, 0}, {0, 1}, {0, 1}};
  // }

  namespace xor_gate {
    static const std::vector<std::vector<float>> inp{{0, 0}, {1, 0}, {0, 1}, {1, 1}};
    static const std::vector<std::vector<float>> trg{{0},    {1},    {1},    {0}};
  }
};

namespace set2x2 {

  namespace and_gate {
    static const std::vector<std::vector<float>> inp{{0, 0}, {1, 0}, {0, 1}, {1, 1}};
    static const std::vector<std::vector<float>> trg{{0, 1}, {0, 1}, {0, 1}, {1, 0}};
  }

  namespace nand_gate {
    static const std::vector<std::vector<float>> inp{{0, 0}, {1, 0}, {0, 1}, {1, 1}};
    static const std::vector<std::vector<float>> trg{{1, 0}, {1, 0}, {1, 0}, {0, 0}};
  }

  namespace or_gate {
    static const std::vector<std::vector<float>> inp{{0, 0}, {1, 0}, {0, 1}, {1, 1}};
    static const std::vector<std::vector<float>> trg{{0, 1}, {1, 0}, {1, 0}, {1, 0}};
  }

  namespace nor_gate {
    static const std::vector<std::vector<float>> inp{{0, 0}, {1, 0}, {0, 1}, {1, 1}};
    static const std::vector<std::vector<float>> trg{{1, 0}, {0, 1}, {0, 1}, {0, 1}};
  }

  namespace p_nq_and {
    static const std::vector<std::vector<float>> inp{{0, 1}, {1, 0}, {0, 1}, {1, 1}};
    static const std::vector<std::vector<float>> trg{{0, 1}, {1, 0}, {0, 1}, {0, 1}};
  }

  namespace xor_gate {
    static const std::vector<std::vector<float>> inp{{0, 0}, {1, 0}, {0, 1}, {1, 1}};
    static const std::vector<std::vector<float>> trg{{0, 1}, {1, 0}, {1, 0}, {0, 1}};
  }

}

#endif