#ifndef SAMPLES_H
#define SAMPLES_H

#include <vector>

namespace set2x2 {

  namespace and {
    static const std::vector<std::vector<float>> inp{{0, 0}, {1, 0}, {0, 1}, {1, 1}};
    static const std::vector<std::vector<float>> trg{{0, 1}, {0, 1}, {0, 1}, {1, 0}};
  }

  namespace ior {
    static const std::vector<std::vector<float>> inp{{0, 0}, {1, 0}, {0, 1}, {1, 1}};
    static const std::vector<std::vector<float>> trg{{0, 1}, {1, 0}, {1, 0}, {1, 0}};
  }

  namespace xor {
    static const std::vector<std::vector<float>> inp{{0, 0}, {1, 0}, {0, 1}, {1, 1}};
    static const std::vector<std::vector<float>> trg{{0, 1}, {1, 0}, {1, 0}, {0, 1}};
  }

}
#endif