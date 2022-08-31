#ifndef ZIPS_ERROR_H
#define ZIPS_ERROR_H

#include "ast.h"
#include <cstdint>
#include <exception>
#include <string>

namespace zips {
void error(const std::string &fileName, size_t line, size_t column,
           const std::string &msg);

struct ZipsError : public std::exception {
  Location location;
  std::string message;
  ZipsError(Location location, std::string message)
      : location(location), message(message) {}
  const char *what() const noexcept override { return message.c_str(); }
};

static inline void error(const ZipsError &error) {
  zips::error(error.location.file, error.location.line, error.location.column,
              error.message);
}

void warn(const std::string &fileName, size_t line, size_t column,
          const std::string &msg);
static inline void warn(const Location &location, const std::string &message) {
  zips::warn(location.file, location.line, location.column, message);
}
} // namespace zips

#endif