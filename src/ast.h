#ifndef ZIPS_AST_H
#define ZIPS_AST_H

#include "location.hh"
#include "type.h"
#include <memory>
#include <optional>
#include <vector>
#include <string>
#include <map>

namespace zips {
enum class AstNodeType {
  COMPILATION_UNIT,
  FUNCTION,
  BINARY_EXPRESSION,
  VARIABLE_REFERENCE,
  RETURN_STATEMENT
};

struct Location {
  size_t line;
  size_t column;
  std::string file;

  Location(const zips::location &location)
      : line(location.begin.line), column(location.begin.column), file(location.begin.filename ? *location.begin.filename : "<Unknown>") {}
};

class AstNode {
  AstNodeType nodeType;
  Location location;

public:
  AstNode(AstNodeType nodeType, Location location)
      : nodeType(nodeType), location(location) {}
  virtual ~AstNode() {}
  AstNodeType getNodeType() { return nodeType; }
  const Location &getLocation() { return location; }

  std::optional<std::unique_ptr<Type>> type;

  std::string toString() {
    if (type) {
      return type->get()->toString() + ": " + toStringInternal();
    }else {
      return toStringInternal();
    }
  }

  private:
   virtual std::string toStringInternal() const = 0;
};
class CompilationUnitNode : public AstNode {
  std::vector<std::unique_ptr<AstNode>> nodes;

public:
  CompilationUnitNode(Location location, std::vector<std::unique_ptr<AstNode>> nodes)
      : AstNode(AstNodeType::COMPILATION_UNIT, location), nodes(std::move(nodes)) {}
  const std::vector<std::unique_ptr<AstNode>> &getNodes() { return nodes; }

  std::string toStringInternal() const override {
    std::string result = "CompilationUnitNode {\n";
    for (auto &node : nodes) {
      result += node->toString() + "\n";
    }
    result += "}";
    return result;
  }
};
class FunctionNode : public AstNode {
  std::string name;
  std::vector<NamedType> parameters;
  std::vector<std::unique_ptr<AstNode>> body;

public:
  FunctionNode(Location location, std::string name, std::vector<NamedType> parameters,
               std::vector<std::unique_ptr<AstNode>> body)
      : AstNode(AstNodeType::FUNCTION, location), name(std::move(name)),
        parameters(std::move(parameters)), body(std::move(body)) {}
  const std::string &getName() { return name; }
  const std::vector<NamedType> &getParameters() { return parameters; }
  const std::vector<std::unique_ptr<AstNode>> &getBody() { return body; }

  std::string toStringInternal() const override {
    std::string result = "FunctionNode {\n";
    result += "name: " + name + "\n";
    result += "parameters: [\n";
    for (auto &parameter : parameters) {
      result += parameter.name + ": " + parameter.type->toString() + "\n";
    }
    result += "]\n";
    result += "body: [\n";
    for (auto &node : body) {
      result += node->toString() + "\n";
    }
    result += "]\n";
    result += "}";
    return result;
  }
};
enum class BinaryOperator { ADD, SUBTRACT, MULTIPLY, DIVIDE, MODULO };
static std::map<BinaryOperator, std::string> binaryOperatorToString = {
    {BinaryOperator::ADD, "+"},
    {BinaryOperator::SUBTRACT, "-"},
    {BinaryOperator::MULTIPLY, "*"},
    {BinaryOperator::DIVIDE, "/"},
    {BinaryOperator::MODULO, "%"}
};
class BinaryExpressionNode : public AstNode {
  BinaryOperator operatorType;
  std::unique_ptr<AstNode> left;
  std::unique_ptr<AstNode> right;

public:
  BinaryExpressionNode(Location location, BinaryOperator operatorType,
                       std::unique_ptr<AstNode> left,
                       std::unique_ptr<AstNode> right)
      : AstNode(AstNodeType::BINARY_EXPRESSION, location), operatorType(operatorType),
        left(std::move(left)), right(std::move(right)) {}
  BinaryOperator getOperator() { return operatorType; }
  AstNode *getLeft() { return left.get(); }
  AstNode *getRight() { return right.get(); }

  std::string toStringInternal() const override {
    std::string result = "BinaryExpressionNode {\n";
    result += "operator: " + binaryOperatorToString[operatorType] + "\n";
    result += "left: " + left->toString() + "\n";
    result += "right: " + right->toString() + "\n";
    result += "}";
    return result;
  }
};
class ReturnStatementNode : public AstNode {
  std::unique_ptr<AstNode> expression;

public:
  ReturnStatementNode(Location location, std::unique_ptr<AstNode> expression)
      : AstNode(AstNodeType::RETURN_STATEMENT, location),
        expression(std::move(expression)) {}
  AstNode *getExpression() { return expression.get(); }

  std::string toStringInternal() const  override {
    std::string result = "ReturnStatementNode {\n";
    result += "expression: " + expression->toString() + "\n";
    result += "}";
    return result;
  }
};
class VariableReferenceNode : public AstNode {
  std::string name;

public:
  VariableReferenceNode(Location location, std::string name)
      : AstNode(AstNodeType::VARIABLE_REFERENCE, location), name(std::move(name)) {}
  const std::string &getName() { return name; }

  std::string toStringInternal() const  override {
    std::string result = "VariableReferenceNode {\n";
    result += "name: " + name + "\n";
    result += "}";
    return result;
  }
};
} // namespace zips

#endif