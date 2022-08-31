#ifndef ZIPS_TYPE_H
#define ZIPS_TYPE_H

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace zips {
enum class TypeType { PRIMITIVE, FUNCTION };
class Type {
  TypeType type;

public:
  Type(TypeType type) : type(type) {}
  virtual ~Type() {}
  TypeType getType() { return type; }

  virtual std::string toString() = 0;
  virtual std::unique_ptr<Type> clone() = 0;
};

struct NamedType {
  std::string name;
  std::unique_ptr<Type> type;
};

enum class PrimitiveTypeType {
  I8,
  I16,
  I32,
  I64,
  U8,
  U16,
  U32,
  U64,
  ISIZE,
  USIZE
};
static inline bool isSigned(PrimitiveTypeType type) {
  switch (type) {
  case PrimitiveTypeType::I8:
  case PrimitiveTypeType::I16:
  case PrimitiveTypeType::I32:
  case PrimitiveTypeType::I64:
  case PrimitiveTypeType::ISIZE:
    return true;
  default:
    return false;
  }
}
static inline size_t getBits(PrimitiveTypeType type) {
  switch (type) {
  case PrimitiveTypeType::I8:
    return 8;
  case PrimitiveTypeType::I16:
    return 16;
  case PrimitiveTypeType::I32:
    return 32;
  case PrimitiveTypeType::I64:
    return 64;
  case PrimitiveTypeType::ISIZE:
    return sizeof(size_t) * 8;
  case PrimitiveTypeType::U8:
    return 8;
  case PrimitiveTypeType::U16:
    return 16;
  case PrimitiveTypeType::U32:
    return 32;
  case PrimitiveTypeType::U64:
    return 64;
  case PrimitiveTypeType::USIZE:
    return sizeof(size_t) * 8;
  }
  return 0;
}
static std::map<PrimitiveTypeType, std::string> primitiveTypeTypeToString = {
    {PrimitiveTypeType::I8, "i8"},       {PrimitiveTypeType::I16, "i16"},
    {PrimitiveTypeType::I32, "i32"},     {PrimitiveTypeType::I64, "i64"},
    {PrimitiveTypeType::U8, "u8"},       {PrimitiveTypeType::U16, "u16"},
    {PrimitiveTypeType::U32, "u32"},     {PrimitiveTypeType::U64, "u64"},
    {PrimitiveTypeType::ISIZE, "isize"}, {PrimitiveTypeType::USIZE, "usize"}};
class PrimitiveTypeNode : public Type {
  PrimitiveTypeType primitiveType;

public:
  PrimitiveTypeNode(PrimitiveTypeType primitiveType)
      : Type(TypeType::PRIMITIVE), primitiveType(primitiveType) {}
  PrimitiveTypeType getPrimitiveType() { return primitiveType; }

  std::string toString() override {
    return primitiveTypeTypeToString[primitiveType];
  }

  std::unique_ptr<Type> clone() override {
    return std::make_unique<PrimitiveTypeNode>(primitiveType);
  }
};
class FunctionTypeNode : public Type {
  std::vector<std::unique_ptr<Type>> parameterTypes;
  std::unique_ptr<Type> returnType;

public:
  FunctionTypeNode(std::vector<std::unique_ptr<Type>> parameterTypes,
                   std::unique_ptr<Type> returnType)
      : Type(TypeType::FUNCTION), parameterTypes(std::move(parameterTypes)),
        returnType(std::move(returnType)) {}
  const std::vector<std::unique_ptr<Type>> &getParameterTypes() {
    return parameterTypes;
  }
  const std::unique_ptr<Type> &getReturnType() { return returnType; }

  std::string toString() override {
    std::string result;
    result += returnType->toString() + "(";
    for (auto &parameterType : parameterTypes) {
      result += parameterType->toString() + ", ";
    }
    if (parameterTypes.size() > 0) {
      result = result.substr(0, result.length() - 2);
    }
    result += ")";
    return result;
  }

  std::unique_ptr<Type> clone() override {
    std::vector<std::unique_ptr<Type>> parameterTypes;
    for (auto &parameterType : this->parameterTypes) {
      parameterTypes.push_back(parameterType->clone());
    }
    return std::make_unique<FunctionTypeNode>(
        std::move(parameterTypes), returnType->clone());
  }
};
} // namespace zips

#endif