#ifndef ZIPS_CODEGEN_H
#define ZIPS_CODEGEN_H

#include "ast.h"
#include "type.h"
#include <variant>

namespace zips {
// Allow us to add to vectors using +=.
template <typename T>
static inline std::vector<T> &operator+=(std::vector<T> &lhs, const T &rhs) {
  lhs.push_back(rhs);
  return lhs;
}
template<typename T>
static inline std::vector<T> &operator+=(std::vector<T> &lhs, const std::optional<T> &rhs) {
  if (rhs) {
    return lhs += *rhs;
  }else {
    return lhs;
  }
}
template <typename T>
static inline std::vector<T> &operator+=(std::vector<T> &lhs,
                                         const std::vector<T> &rhs) {
  lhs.insert(lhs.end(), rhs.begin(), rhs.end());
  return lhs;
}

enum class TargetArchitecture { X86_64, AARCH64 };
enum class TargetAbi { X86_64, MS_X64, AARCH64_EABI };

template <TargetArchitecture arch, TargetAbi abi>
class AssemblyInstructionGenerator {};
template <TargetAbi abi>
class AssemblyInstructionGenerator<TargetArchitecture::X86_64, abi> {
public:
  enum class Register {
    RAX,
    RCX,
    RDX,
    RBX,
    RBP,
    RSP,
    RSI,
    RDI,
    R8,
    R9,
    R10,
    R11,
    R12,
    R13,
    R14,
    R15
  };
  static constexpr size_t registerSize = 8;
  static constexpr std::vector<Register> callerSavedRegisters() {
    if constexpr (abi == TargetAbi::X86_64) {
      return {Register::RAX, Register::RCX, Register::RDX,
              Register::RSI, Register::RDI, Register::R8,
              Register::R9,  Register::R10, Register::R11};
    } else if constexpr (abi == TargetAbi::MS_X64) {
      return {Register::RAX, Register::RCX, Register::RDX, Register::R8,
              Register::R9,  Register::R10, Register::R11};
    }
  }
  static constexpr std::vector<Register> calleeSavedRegisters() {
    if constexpr (abi == TargetAbi::X86_64) {
      return {Register::RBX, Register::R12, Register::R13, Register::R14,
              Register::R15};
    } else if constexpr (abi == TargetAbi::MS_X64) {
      return {Register::RBX, Register::R12, Register::R13, Register::R14,
              Register::R15};
    }
  }
  static constexpr std::vector<Register> parameterPassingRegisters() {
    if constexpr (abi == TargetAbi::X86_64) {
      return {Register::RDI, Register::RSI, Register::RDX,
              Register::RCX, Register::R8,  Register::R9};
    } else if constexpr (abi == TargetAbi::MS_X64) {
      return {Register::RCX, Register::RDX, Register::R8, Register::R9};
    }
  }
  static constexpr size_t stackAlignmentOnCall = 16;
  static constexpr Register RETURN_VALUE_REGISTER = Register::RAX;

  enum class OperandSize { I8, I16, I32, I64 };

  static constexpr OperandSize operandSizeFromBits(size_t bits) {
    if (bits <= 8) {
      return OperandSize::I8;
    } else if (bits <= 16) {
      return OperandSize::I16;
    } else if (bits <= 32) {
      return OperandSize::I32;
    } else if (bits <= 64) {
      return OperandSize::I64;
    } else {
      throw std::runtime_error("Invalid operand size");
    }
  }

  static constexpr size_t getSize(OperandSize size) {
    switch (size) {
    case OperandSize::I8:
      return 1;
    case OperandSize::I16:
      return 2;
    case OperandSize::I32:
      return 4;
    case OperandSize::I64:
      return 8;
    }
    return 0;
  }

  static constexpr std::string registerToString64(Register reg) {
    switch (reg) {
    case Register::RAX:
      return "rax";
    case Register::RCX:
      return "rcx";
    case Register::RDX:
      return "rdx";
    case Register::RBX:
      return "rbx";
    case Register::RBP:
      return "rbp";
    case Register::RSP:
      return "rsp";
    case Register::RSI:
      return "rsi";
    case Register::RDI:
      return "rdi";
    case Register::R8:
      return "r8";
    case Register::R9:
      return "r9";
    case Register::R10:
      return "r10";
    case Register::R11:
      return "r11";
    case Register::R12:
      return "r12";
    case Register::R13:
      return "r13";
    case Register::R14:
      return "r14";
    case Register::R15:
      return "r15";
    }
    return "Unknown";
  }
  static constexpr std::string registerToString32(Register reg) {
    switch (reg) {
    case Register::RAX:
      return "eax";
    case Register::RCX:
      return "ecx";
    case Register::RDX:
      return "edx";
    case Register::RBX:
      return "ebx";
    case Register::RBP:
      return "ebp";
    case Register::RSP:
      return "esp";
    case Register::RSI:
      return "esi";
    case Register::RDI:
      return "edi";
    case Register::R8:
      return "r8d";
    case Register::R9:
      return "r9d";
    case Register::R10:
      return "r10d";
    case Register::R11:
      return "r11d";
    case Register::R12:
      return "r12d";
    case Register::R13:
      return "r13d";
    case Register::R14:
      return "r14d";
    case Register::R15:
      return "r15d";
    }
    return "Unknown";
  }
  static constexpr std::string registerToString16(Register reg) {
    switch (reg) {
    case Register::RAX:
      return "ax";
    case Register::RCX:
      return "cx";
    case Register::RDX:
      return "dx";
    case Register::RBX:
      return "bx";
    case Register::RBP:
      return "bp";
    case Register::RSP:
      return "sp";
    case Register::RSI:
      return "si";
    case Register::RDI:
      return "di";
    case Register::R8:
      return "r8w";
    case Register::R9:
      return "r9w";
    case Register::R10:
      return "r10w";
    case Register::R11:
      return "r11w";
    case Register::R12:
      return "r12w";
    case Register::R13:
      return "r13w";
    case Register::R14:
      return "r14w";
    case Register::R15:
      return "r15w";
    }
    return "Unknown";
  }
  static constexpr std::string registerToString8(Register reg) {
    switch (reg) {
    case Register::RAX:
      return "al";
    case Register::RCX:
      return "cl";
    case Register::RDX:
      return "dl";
    case Register::RBX:
      return "bl";
    case Register::RBP:
      return "bpl";
    case Register::RSP:
      return "spl";
    case Register::RSI:
      return "sil";
    case Register::RDI:
      return "dil";
    case Register::R8:
      return "r8b";
    case Register::R9:
      return "r9b";
    case Register::R10:
      return "r10b";
    case Register::R11:
      return "r11b";
    case Register::R12:
      return "r12b";
    case Register::R13:
      return "r13b";
    case Register::R14:
      return "r14b";
    case Register::R15:
      return "r15b";
    }
    return "Unknown";
  }

  static constexpr std::string registerToString(OperandSize size,
                                                Register reg) {
    switch (size) {
    case OperandSize::I8:
      return registerToString8(reg);
    case OperandSize::I16:
      return registerToString16(reg);
    case OperandSize::I32:
      return registerToString32(reg);
    case OperandSize::I64:
      return registerToString64(reg);
    }
    return "Unknown";
  }

  static constexpr std::string operandSizeSuffix(OperandSize size) {
    switch (size) {
    case OperandSize::I8:
      return "b";
    case OperandSize::I16:
      return "w";
    case OperandSize::I32:
      return "l";
    case OperandSize::I64:
      return "q";
    }
    return "Unknown";
  }

  struct Operand {
    struct MemoryOperand {
      Register base;
      ptrdiff_t offset;
    };
    std::variant<Register, size_t, std::string, MemoryOperand> value;
  };
  struct Instruction {
    std::string mnemonic;
    OperandSize size;
    std::vector<Operand> operands;
    bool needsOperandSizeSuffix = true;

    std::string toString() const {
      std::string result;
      result += mnemonic;
      if (needsOperandSizeSuffix) {
        result += operandSizeSuffix(size);
      }
      result += " ";
      for (auto &operand : operands) {
        if (std::holds_alternative<Register>(operand.value)) {
          result +=
              "%" + registerToString(size, std::get<Register>(operand.value));
        } else if (std::holds_alternative<size_t>(operand.value)) {
          result += "$" + std::to_string(std::get<size_t>(operand.value));
        } else if (std::holds_alternative<std::string>(operand.value)) {
          result += std::get<std::string>(operand.value);
        } else {
          auto &mem = std::get<typename Operand::MemoryOperand>(operand.value);
          result += std::to_string(mem.offset) + "(%" +
                    registerToString64(mem.base) + ")";
        }
        result += ", ";
      }
      if (operands.size() > 0) {
        result = result.substr(0, result.length() - 2);
      }
      return result;
    }
  };

  std::string generateFileHeader(const std::string &fileName) {
    std::string result;
    result += ".file \"" + fileName + "\"\n";
    result += ".text";
    return result;
  }

  std::string generateFileFooter() {
    std::string result;
    result += ".ident \"Compiled by the Zips compiler\"\n";
    result += ".section .note.GNU-stack,\"\",@progbits";
    return result;
  }

  std::string generateFunctionHeader(const std::string &name) {
    std::string result;
    result += ".globl " + name + "\n";
    result += ".type " + name + ", @function\n";
    result += name + ":";
    return result;
  }

  std::string generateFunctionFooter(const std::string &functionName) {
    std::string result;
    result += ".size " + functionName + ", .-" + functionName;
    return result;
  }

  std::vector<Instruction> generateProlog(size_t stackAllocationSize) {
    std::vector<Instruction> result;
    result += Instruction{"push", OperandSize::I64, {Operand{Register::RBP}}};
    result += Instruction{"mov",
                          OperandSize::I64,
                          {Operand{Register::RSP}, Operand{Register::RBP}}};
    if (stackAllocationSize > 0) {
      result +=
          Instruction{"sub",
                      OperandSize::I64,
                      {Operand{stackAllocationSize}, Operand{Register::RSP}}};
    }
    return result;
  }
  std::vector<Instruction> generateEpilog(size_t stackAllocationSize) {
    std::vector<Instruction> result;
    if (stackAllocationSize > 0) {
      result +=
          Instruction{"add",
                      OperandSize::I64,
                      {Operand{stackAllocationSize}, Operand{Register::RSP}}};
    }
    result += Instruction{"pop", OperandSize::I64, {Operand{Register::RBP}}};
    result += Instruction{"ret", OperandSize::I64, {}};
    return result;
  }

  Instruction generateLabel(const std::string &name) {
    return Instruction{name + ":", OperandSize::I64, {}, false};
  }

  Instruction generateSaveRegister(Register reg) {
    return Instruction{"push", OperandSize::I64, {Operand{reg}}};
  }
  Instruction generateRestoreRegister(Register reg) {
    return Instruction{"pop", OperandSize::I64, {Operand{reg}}};
  }

  std::optional<Instruction> move(OperandSize size, Register from,
                                  Register to) {
    if (from != to) {
      return Instruction{"mov", size, {Operand{from}, Operand{to}}};
    } else {
      return std::nullopt;
    }
  }

  Instruction jump(const std::string &label) {
    return Instruction{"jmp", OperandSize::I64, {Operand{label}}, false};
  }

  std::vector<Instruction> add(OperandSize size, Register a, Register b,
                               Register dest) {
    std::vector<Instruction> result;
    result += move(size, a, dest);
    result += Instruction{"add", size, {Operand{b}, Operand{dest}}};
    return result;
  }

  Instruction stackStore(OperandSize size, Register reg, ptrdiff_t offset) {
    ptrdiff_t rbpOffset = -offset - 8;
    return Instruction{"mov",
                       size,
                       {Operand{reg}, Operand{typename Operand::MemoryOperand{
                                          Register::RBP, rbpOffset}}}};
  }
};

template <TargetArchitecture arch, TargetAbi abi> class CodeGenerator {
  using InstructionGenerator = AssemblyInstructionGenerator<arch, abi>;
  using Instruction = InstructionGenerator::Instruction;
  using OperandSize = InstructionGenerator::OperandSize;
  using Register = InstructionGenerator::Register;

  InstructionGenerator instructionGenerator;

  struct Value {
    OperandSize size;
    std::variant<Register, size_t> position; // Register or stack offset.
    bool variable = false;
  };

  struct Function {
    std::string name;
    std::vector<std::map<std::string, Value>> variables;
    std::string labelPrefix;
    std::vector<Instruction> instructions;
    std::vector<Register> savedRegisters;
    size_t stackAllocationSize = 0;
    std::vector<Register> availableRegisters =
        InstructionGenerator::callerSavedRegisters();
    std::vector<Register> remainingCalleeSavedRegisters =
        InstructionGenerator::calleeSavedRegisters();

    Value createValue(OperandSize size, bool isVariable = false) {
      if (availableRegisters.size() > 0) {
        Register chosenRegister = availableRegisters.back();
        availableRegisters.pop_back();
        return Value{size, chosenRegister, isVariable};
      } else if (remainingCalleeSavedRegisters.size() > 0) {
        Register chosenRegister = remainingCalleeSavedRegisters.back();
        remainingCalleeSavedRegisters.pop_back();
        savedRegisters += chosenRegister;
        return Value{size, chosenRegister, isVariable};
      } else {
        size_t sizeBytes = InstructionGenerator::getSize(size);
        // Naturally aligned.
        stackAllocationSize =
            (stackAllocationSize / sizeBytes) * sizeBytes + sizeBytes;
        return Value{size, stackAllocationSize - sizeBytes, isVariable};
      }
    }
    void destroyValue(Value value) {
      if (!value.variable && std::holds_alternative<Register>(value.position)) {
        availableRegisters += std::get<Register>(value.position);
      }
    }
    void destroyVariable(Value value) {
      if (value.variable && std::holds_alternative<Register>(value.position)) {
        availableRegisters += std::get<Register>(value.position);
      }
    }
    Value createValue(OperandSize size, bool isVariable, Register position) {
      return Value{size, position, isVariable};
    }
  };

  Register getIntoRegister(Function &function, const Value &value) {
    if (std::holds_alternative<Register>(value.position)) {
      return std::get<Register>(value.position);
    } else {
      throw std::runtime_error("Not implemented - out of registers");
    }
  }
  void getBackToValue(Function &function, Register allocatedRegister,
                      const Value &value) {
    if (std::holds_alternative<Register>(value.position)) {
      Register targetRegister = std::get<Register>(value.position);
      if (allocatedRegister != targetRegister) {
        function.instructions += instructionGenerator.move(
            value.size, allocatedRegister, targetRegister);
      }
    } else {
      function.instructions += instructionGenerator.stackStore(
          value.size, allocatedRegister, std::get<size_t>(value.position));
    }
  }

  Value add(Function &function, const Value &a, const Value &b,
            bool destroyValues) {
    Register registerA = getIntoRegister(function, a);
    Register registerB = getIntoRegister(function, b);
    Value result = function.createValue(a.size);
    Register registerResult = getIntoRegister(function, result);
    function.instructions += instructionGenerator.add(
        result.size, registerA, registerB, registerResult);
    getBackToValue(function, registerResult, result);
    return result;
  }

  void returnValue(Function &function, const Value &value) {
    Register valueRegister = getIntoRegister(function, value);
    function.instructions += instructionGenerator.move(
        value.size, valueRegister, InstructionGenerator::RETURN_VALUE_REGISTER);
    function.instructions +=
        instructionGenerator.jump(function.labelPrefix + "_end");
  }

  Value generateExpression(Function &function, AstNode *node) {
    switch (node->getNodeType()) {
    case AstNodeType::VARIABLE_REFERENCE: {
      std::string variableName =
          static_cast<VariableReferenceNode *>(node)->getName();
      for (auto variableScope = function.variables.rbegin();
           variableScope != function.variables.rend(); variableScope++) {
        if (variableScope->contains(variableName)) {
          return (*variableScope)[variableName];
        }
      }
      throw std::runtime_error("Variable not found");
    }
    case AstNodeType::BINARY_EXPRESSION: {
      auto binaryExpression = static_cast<BinaryExpressionNode *>(node);
      Value left = generateExpression(function, binaryExpression->getLeft());
      Value right = generateExpression(function, binaryExpression->getRight());
      // Destroy the left and right operands so they can be used as the result
      // value.
      function.destroyValue(left);
      function.destroyValue(right);
      Value result;
      switch (binaryExpression->getOperator()) {
      case BinaryOperator::ADD: {
        result = add(function, left, right, true);
        break;
      }
      default:
        throw std::runtime_error("Not implemented - binary expression");
      }
      function.destroyValue(left);
      function.destroyValue(right);
      return result;
      break;
    }
    default:
      throw std::runtime_error("Unimplemented expression type");
    }
  }

  void generateStatement(Function &function, AstNode *node) {
    switch (node->getNodeType()) {
    case AstNodeType::RETURN_STATEMENT: {
      returnValue(
          function,
          generateExpression(
              function,
              static_cast<ReturnStatementNode *>(node)->getExpression()));
      break;
    }
    default:
      function.destroyValue(generateExpression(function, node));
    }
  }

  Function generateFunction(FunctionNode *node, size_t functionIndex) {
    Function function;
    function.name = node->getName();
    function.labelPrefix = "l" + std::to_string(functionIndex);
    std::map<std::string, Value> parameters;
    size_t i = 0;
    for (auto &parameter : node->getParameters()) {
      if (parameter.type->getType() != TypeType::PRIMITIVE) {
        throw std::runtime_error("Not implemented - non-primitive parameters");
      }
      parameters[parameter.name] = function.createValue(
          InstructionGenerator::operandSizeFromBits(
              getBits(static_cast<PrimitiveTypeNode *>(parameter.type.get())
                          ->getPrimitiveType())),
          true, InstructionGenerator::parameterPassingRegisters()[i]);
      i++;
    }
    function.variables += std::move(parameters);
    for (auto &statement : node->getBody()) {
      generateStatement(function, statement.get());
    }
    std::vector<Instruction> actualInstructions =
        instructionGenerator.generateProlog(function.stackAllocationSize);
    for (auto &savedRegister : function.savedRegisters) {
      actualInstructions +=
          instructionGenerator.generateSaveRegister(savedRegister);
    }
    actualInstructions += std::move(function.instructions);
    actualInstructions +=
        instructionGenerator.generateLabel(function.labelPrefix + "_end");
    for (auto &savedRegister : function.savedRegisters) {
      actualInstructions +=
          instructionGenerator.generateRestoreRegister(savedRegister);
    }
    actualInstructions +=
        instructionGenerator.generateEpilog(function.stackAllocationSize);
    function.instructions = std::move(actualInstructions);
    return function;
  }

public:
  std::string generate(CompilationUnitNode *node) {
    std::vector<Function> functions;
    size_t functionIndex = 0;
    for (auto &function : node->getNodes()) {
      functions.push_back(generateFunction(
          static_cast<FunctionNode *>(function.get()), functionIndex++));
    }
    std::string result =
        instructionGenerator.generateFileHeader(node->getLocation().file) +
        "\n";
    for (auto &function : functions) {
      result +=
          instructionGenerator.generateFunctionHeader(function.name) + "\n";
      for (auto &instruction : function.instructions) {
        result += "\t" + instruction.toString() + "\n";
      }
      result +=
          instructionGenerator.generateFunctionFooter(function.name) + "\n";
    }
    result += instructionGenerator.generateFileFooter();
    return result;
  }
};
} // namespace zips

#endif