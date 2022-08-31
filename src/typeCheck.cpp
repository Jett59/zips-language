#include "typeCheck.h"
#include "error.h"
#include <algorithm>

using namespace std::string_literals;

namespace zips {
static inline Type *executeBinaryExpression(BinaryOperator operatorType,
                                            Type *a, Type *b,
                                            const Location &location) {
  if (a->getType() == b->getType()) {
    switch (a->getType()) {
    case TypeType::PRIMITIVE: {
      auto aPrimitive = static_cast<PrimitiveTypeNode *>(a);
      auto bPrimitive = static_cast<PrimitiveTypeNode *>(b);
      if (isSigned(aPrimitive->getPrimitiveType()) !=
          isSigned(bPrimitive->getPrimitiveType())) {
        warn(location, "Performing binary operator " +
                           binaryOperatorToString[operatorType] +
                           " with types of different signedness: " +
                           a->toString() + " and " + b->toString());
      }
      if (getBits(aPrimitive->getPrimitiveType()) >=
          getBits(bPrimitive->getPrimitiveType())) {
        return a;
      } else {
        return b;
      }
      break;
    }
    case TypeType::FUNCTION: {
      throw ZipsError(location,
                      "Cannot execute binary expression on functions");
    }
    }
  } else {
    throw ZipsError(location, "Incompatible types for binary operator"s +
                                  binaryOperatorToString[operatorType] + ": " +
                                  a->toString() + " and " + b->toString());
  }
}

/**
 * @brief attempt the conversion to a given type.
 *
 * This function throws if the types are not convertible.
 *
 * @param from
 * @param to
 */
static void convert(Type *from, Type *to, const Location &location) {
  if (from->getType() == to->getType()) {
    switch (from->getType()) {
    case TypeType::PRIMITIVE: {
      auto fromPrimitiveType =
          static_cast<PrimitiveTypeNode *>(from)->getPrimitiveType();
      auto toPrimitiveType =
          static_cast<PrimitiveTypeNode *>(to)->getPrimitiveType();
      if (isSigned(fromPrimitiveType) != isSigned(toPrimitiveType)) {
        warn(location, "Converting from " + from->toString() + " to " +
                           to->toString() + " changes signedness");
      }
      if (getBits(fromPrimitiveType) > getBits(toPrimitiveType)) {
        warn(location, "Converting from " + from->toString() + " to " +
                           to->toString() + " loses precision");
      }
      break;
    }
    case TypeType::FUNCTION: {
      throw ZipsError(location,
                      "Converting of function types not yet supported");
      break;
    }
    }
  } else {
    throw ZipsError(location, "Can't convert "s + from->toString() + " to " +
                                  to->toString());
  }
}

void checkTypes(AstNode *node, Context &context) {
  switch (node->getNodeType()) {
  case AstNodeType::COMPILATION_UNIT: {
    auto compilationUnit = static_cast<CompilationUnitNode *>(node);
    for (auto &node : compilationUnit->getNodes()) {
      checkTypes(node.get(), context);
    }
    break;
  }
  case AstNodeType::FUNCTION: {
    auto function = static_cast<FunctionNode *>(node);
    std::map<std::string, Type *> parameters;
    for (auto &parameter : function->getParameters()) {
      parameters[parameter.name] = parameter.type.get();
    }
    context.symbolTable.push_back(std::move(parameters));
    for (auto &node : function->getBody()) {
      checkTypes(node.get(), context);
    }
    context.symbolTable.pop_back();
    std::vector<std::unique_ptr<Type>> parameterTypes;
    for (auto &parameter : function->getParameters()) {
      parameterTypes.push_back(parameter.type->clone());
    }
    function->type = std::make_unique<FunctionTypeNode>(
        std::move(parameterTypes),
        context.currentFunctionReturnType.value()->clone());
    break;
  }
  case AstNodeType::RETURN_STATEMENT: {
    auto returnNode = static_cast<ReturnStatementNode *>(node);
    checkTypes(returnNode->getExpression(), context);
    if (context.currentFunctionReturnType) {
      convert(returnNode->getExpression()->type->get(),
              *context.currentFunctionReturnType, returnNode->getLocation());
    } else {
      context.currentFunctionReturnType =
          returnNode->getExpression()->type->get();
    }
    break;
  }
  case AstNodeType::BINARY_EXPRESSION: {
    auto binaryExpression = static_cast<BinaryExpressionNode *>(node);
    checkTypes(binaryExpression->getLeft(), context);
    checkTypes(binaryExpression->getRight(), context);
    binaryExpression->type =
        executeBinaryExpression(binaryExpression->getOperator(),
                                binaryExpression->getLeft()->type->get(),
                                binaryExpression->getRight()->type->get(),
                                binaryExpression->getLocation())
            ->clone();
    break;
  }
  case AstNodeType::VARIABLE_REFERENCE: {
    auto variableReference = static_cast<VariableReferenceNode *>(node);
    for (auto symbolTable = context.symbolTable.rbegin();
         symbolTable != context.symbolTable.rend(); symbolTable++) {
          if (symbolTable->contains(variableReference->getName())) {
            variableReference->type =
                (*symbolTable)[variableReference->getName()]->clone();
            break;
          }
         }
         if (!variableReference->type.has_value()) {
            throw ZipsError(variableReference->getLocation(),
                            "Undefined identifier "s + variableReference->getName());
         }
      break;
  }
  }
}
} // namespace zips
