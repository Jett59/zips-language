#include "ast.h"
#include "lexer.h"
#include "parser.hh"
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
  Lexer lexer(input);
  Parser parser(lexer, fileName, &ast);
  int result = parser();
  input.close();
  if (result == 0) {
    std::cout << "Success!" << std::endl;
  }else {
    std::cerr << "Error!" << std::endl;
  }
}
