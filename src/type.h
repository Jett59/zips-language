#ifndef ZIPS_TYPE_H
#define ZIPS_TYPE_H

#include <string>
#include <memory>

namespace zips {
enum class TypeType { PRIMITIVE };
class Type {
  TypeType type;

public:
  Type(TypeType type) : type(type) {}
  virtual ~Type() {}
  TypeType getType() { return type; }
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
class PrimitiveTypeNode : public Type {
    PrimitiveTypeType primitiveType;

    public:
    PrimitiveTypeNode(PrimitiveTypeType primitiveType) : Type(TypeType::PRIMITIVE), primitiveType(primitiveType) {}
    PrimitiveTypeType getPrimitiveType() { return primitiveType; }
};
} // namespace zips

#endif