#ifndef ZIPS_TYPE_H
#define ZIPS_TYPE_H

#include <memory>
#include <string>
#include <vector>
#include <map>

namespace zips {
enum class TypeType { PRIMITIVE, FUNCTION };
class Type {
  TypeType type;

public:
  Type(TypeType type) : type(type) {}
  virtual ~Type() {}
  TypeType getType() { return type; }

  virtual std::string toString() = 0;
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

  std::string toString() override { return primitiveTypeTypeToString[primitiveType]; }
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
    std::string result = "FunctionTypeNode {\n";
    result += "Parameters: [\n";
    for (auto &parameterType : parameterTypes) {
      result += parameterType->toString() + "\n";
    }
    result += "]\n";
    result += "returns: " + returnType->toString() + "\n" + "\n";
    result += "}";
    return result;
  }
};
} // namespace zips

#endif