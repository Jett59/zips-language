#include "error.h"
#include <iostream>

namespace zips {
    void error(const std::string &fileName, size_t line, size_t column, const std::string &message) {
      std::cout << fileName << ":" << line << ":" << column << ":" << std::endl;
      std::cout << message << std::endl;
    }
}
