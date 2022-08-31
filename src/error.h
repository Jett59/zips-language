#ifndef ZIPS_ERROR_H
#define ZIPS_ERROR_H

#include <string>
#include <cstdint>
#include <exception>
#include "ast.h"

namespace zips {
    void error(const std::string &fileName, size_t line, size_t column, const std::string& msg);

    struct ZipsError : public std::exception {
      Location location;
      std::string message;
        ZipsError(Location location, std::string message) : location(location), message(message) {}
        const char *what() const noexcept override { return message.c_str(); }
    };
}

#endif