#ifndef ZIPS_LEXER_H
#define ZIPS_LEXER_H

#include <fstream>

#include "location.hh"

#if !defined(yyFlexLexerOnce)
#include "FlexLexer.h"
#endif

#undef YY_DECL
#define YY_DECL zips::Parser::symbol_type zips::Lexer::next()

#include "parser.hh"

namespace zips {
class Lexer : public yyFlexLexer {
private:
  location currentLocation;

public:
  Lexer(std::istream &input)
      : yyFlexLexer(&input) {
  }
  Parser::symbol_type next();
  location getLocation() { return currentLocation; }

private:
  void updateLocation(const char *token);
};
} // namespace zips

#endif