#ifndef ZIPS_CODEGEN_H
#define ZIPS_CODEGEN_H

#include "ast.h"
#include "type.h"
#include <variant>

namespace zips {
enum class TargetArchitecture { X86_64, AARCH64 };
enum class TargetAbi { X86_64, MS_X64, AARCH64_EABI };

template <TargetArchitecture arch, TargetAbi abi> class InstructionGenerator {
  static_assert(false, "Architecture or abi not implemented");
};
template <TargetAbi abi>
class InstructionGenerator<TargetArchitecture::X86_64, abi> {
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

  enum class OperandSize { I8, I16, I32, I64 };

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
  }

  static std::map < Register,
      std::string >>
          registerToString64 = {{Register::RAX, "rax"}, {Register::RCX, "rcx"},
                                {Register::RDX, "rdx"}, {Register::RBX, "rbx"},
                                {Register::RBP, "rbp"}, {Register::RSP, "rsp"},
                                {Register::RSI, "rsi"}, {Register::RDI, "rdi"},
                                {Register::R8, "r8"},   {Register::R9, "r9"},
                                {Register::R10, "r10"}, {Register::R11, "r11"},
                                {Register::R12, "r12"}, {Register::R13, "r13"},
                                {Register::R14, "r14"}, {Register::R15, "r15"}};
  static std::map < Register,
      std::string >> registerToString32 = {
          {Register::RAX, "eax"},  {Register::RCX, "ecx"},
          {Register::RDX, "edx"},  {Register::RBX, "ebx"},
          {Register::RBP, "ebp"},  {Register::RSP, "esp"},
          {Register::RSI, "esi"},  {Register::RDI, "edi"},
          {Register::R8, "r8d"},   {Register::R9, "r9d"},
          {Register::R10, "r10d"}, {Register::R11, "r11d"},
          {Register::R12, "r12d"}, {Register::R13, "r13d"},
          {Register::R14, "r14d"}, {Register::R15, "r15d"}};
  static std::map < Register,
      std::string >> registerToString16 = {
          {Register::RAX, "ax"},   {Register::RCX, "cx"},
          {Register::RDX, "dx"},   {Register::RBX, "bx"},
          {Register::RBP, "bp"},   {Register::RSP, "sp"},
          {Register::RSI, "si"},   {Register::RDI, "di"},
          {Register::R8, "r8w"},   {Register::R9, "r9w"},
          {Register::R10, "r10w"}, {Register::R11, "r11w"},
          {Register::R12, "r12w"}, {Register::R13, "r13w"},
          {Register::R14, "r14w"}, {Register::R15, "r15w"}};
  static std::map < Register,
      std::string >> registerToString8 = {
          {Register::RAX, "al"},   {Register::RCX, "cl"},
          {Register::RDX, "dl"},   {Register::RBX, "bl"},
          {Register::RBP, "bpl"},  {Register::RSP, "spl"},
          {Register::RSI, "sil"},  {Register::RDI, "dil"},
          {Register::R8, "r8b"},   {Register::R9, "r9b"},
          {Register::R10, "r10b"}, {Register::R11, "r11b"},
          {Register::R12, "r12b"}, {Register::R13, "r13b"},
          {Register::R14, "r14b"}, {Register::R15, "r15b"}};

  static constexpr std::string registerToString(OperandSize size,
                                                Register register) {
    switch (operandSize) {
    case OperandSize::I8:
      return registerToString8[register];
    case OperandSize::I16:
      return registerToString16[register];
    case OperandSize::I32:
      return registerToString32[register];
    case OperandSize::I64:
      return registerToString64[register];
    }
  }

  struct Operand {
    OperandSize size;
    std::variant<Register, size_t> value;
  };
  struct Instruction {
    std::string mnemonic;
    std::vector<Operand> operands;

    std::string toString() const {
      std::string result;
      result += mnemonic + " ";
      for (auto &operand : operands) {
        if (std::holds_alternative<Register>(operand.value)) {
          result += "%" + registerToString(operand.size,
                                           std::get<Register>(operand.value));
        } else {
          result += "$" + std::to_string(std::get<size_t>(operand.value));
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
    result.push_back(
        Instruction{"push", {Operand{OperandSize::I64, Register::RBP}}});
    result.push_back(Instruction{"mov",
                                 {Operand{OperandSize::I64, Register::RSP},
                                  Operand{OperandSize::I64, Register::RBP}}});
    result.push_back(
        Instruction{"sub",
                    {Operand{OperandSize::I64, stackAllocationSize},
                     Operand{OperandSize::I64, Register::RSP}}});
    return result;
  }
};

template <TargetArchitecture arch, TargetAbi abi> class CodeGenerator {
  using InstructionGenerator = InstructionGenerator<arch, abi>;
  using InstructionGenerator::Instruction;
  using InstructionGenerator::OperandSize;
  using InstructionGenerator::Register;

  InstructionGenerator instructionGenerator;

  struct Value {
    OperandSize size;
    std::variant<Register, size_t> position; // Register or stack offset.
  };

  struct Function {
    std::string name;
    std::vector<Instruction> instructions;
    std::vector<Register> savedRegisters;
    size_t stackAllocationSize = 0;
    std::vector<Register> availableRegisters =
        InstructionGenerator::callerSavedRegisters();
    std::vector<Register> remainingCalleeSavedRegisters =
        InstructionGenerator::calleeSavedRegisters();

    Value createValue(OperandSize size) {
      if (availableRegisters.size() > 0) {
        return Value{size, availableRegisters.pop_back()};
      } else if (remainingCalleeSavedRegisters.size() > 0) {
        Register chosenRegister = remainingCalleeSavedRegisters.pop_back();
        savedRegisters.push_back(chosenRegister);
        return Value{size, chosenRegister};
      } else {
        size_t sizeBytes = InstructionGenerator::getSize(size);
        // Naturally aligned.
        stackAllocationSize =
            (stackAllocationSize / sizeBytes) * sizeBytes + sizeBytes;
        return Value{size, stackAllocationSize - sizeBytes};
      }
    }
    void destroyValue(Value value) {
      if (std::holds_alternative<Register>(value)) {
        availableRegisters.push_back(std::get<Register>(value));
      }
    }
  };

public:
  std::string generate(CompilationUnitNode *node) {
    std::vector<Function> functions;
    // TODO.
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