#include "ast.h"
#include "codegen/codegen.h"
#include "error.h"
#include "lexer.h"
#include "parser.hh"
#include "typeCheck.h"
#include <fstream>

void usage(const char *program) {
  std::cerr << "Usage: " << program << " file" << std::endl;
}

using namespace zips;

int main(int argc, char **argv) {
  if (argc < 2) {
    usage(argv[0]);
    return 1;
  }
  std::string fileName = argv[1];
  std::ifstream input(fileName);
  if (!input) {
    perror(fileName.c_str());
    return 1;
  }
  std::unique_ptr<AstNode> ast;
  Lexer lexer(input, fileName);
  Parser parser(lexer, fileName, &ast);
  int result = parser();
  input.close();
  if (result == 0) {
    try {
      checkTypes(ast.get());
      CodeGenerator<TargetArchitecture::X86_64, TargetAbi::X86_64>
          codeGenerator;
      std::cout << codeGenerator.generate(
                       static_cast<CompilationUnitNode *>(ast.get()))
                << std::endl;
    } catch (const ZipsError &e) {
      error(e);
    } catch (std::runtime_error &e) {
      std::cerr << e.what() << std::endl;
    }
  } else {
    std::cerr << "Error!" << std::endl;
  }
}
