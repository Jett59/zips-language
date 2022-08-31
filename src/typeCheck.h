#ifndef ZIPS_TYPE_CHECK_H
#define ZIPS_TYPE_CHECK_H

#include "ast.h"
#include "type.h"
#include <exception>
#include <memory>

namespace zips {
struct Context {
  std::optional<std::unique_ptr<Type>> &currentFunctionReturnType;
};
class TypeError : public std::exception {
  std::string message;

public:
  TypeError(std::string message) : message(message) {}
  const char *what() const noexcept override { return message.c_str(); }
};
} // namespace zips

#endif