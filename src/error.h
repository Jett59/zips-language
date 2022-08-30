#ifndef ZIPS_ERROR_H
#define ZIPS_ERROR_H

#include <string>
#include <cstdint>

namespace zips {
    void error(const std::string &fileName, size_t line, size_t column, const std::string& msg);
}

#endif