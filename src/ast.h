#ifndef ZIPS_AST_H
#define ZIPS_AST_H

#include "type.h"
#include <memory>
#include <vector>

namespace zips {
enum class AstNodeType { COMPILATION_UNIT, FUNCTION };

class AstNode {
  AstNodeType type;

public:
  AstNode(AstNodeType type) : type(type) {}
  virtual ~AstNode() {}
  AstNodeType getType() { return type; }
};
class CompilationUnitNode : public AstNode {
  std::vector<std::unique_ptr<AstNode>> nodes;

public:
  CompilationUnitNode(std::vector<std::unique_ptr<AstNode>> nodes)
      : AstNode(AstNodeType::COMPILATION_UNIT), nodes(std::move(nodes)) {}
  const std::vector<std::unique_ptr<AstNode>> &getNodes() { return nodes; }
};
class FunctionNode : public AstNode {
  std::string name;
  std::vector<NamedType> parameters;
  std::vector<std::unique_ptr<AstNode>> body;

public:
  FunctionNode(std::string name, std::vector<NamedType> parameters,
               std::vector<std::unique_ptr<AstNode>> body)
      : AstNode(AstNodeType::FUNCTION), name(std::move(name)),
        parameters(std::move(parameters)), body(std::move(body)) {}
  const std::string &getName() { return name; }
  const std::vector<NamedType> &getParameters() { return parameters; }
  const std::vector<std::unique_ptr<AstNode>> &getBody() { return body; }
};
} // namespace zips

#endif