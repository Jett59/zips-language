#ifndef ZIPS_TYPE_CHECK_H
#define ZIPS_TYPE_CHECK_H

#include "ast.h"
#include "type.h"
#include <exception>
#include <memory>
#include <map>
#include <vector>

namespace zips {
struct Context {
  std::optional<Type *> currentFunctionReturnType;
  std::vector<std::map<std::string, Type *>> symbolTable;
};
void checkTypes(AstNode *node, Context &context);
static inline void checkTypes(AstNode *ast) {
  Context context;
  checkTypes(ast, context);
}
} // namespace zips

#endif