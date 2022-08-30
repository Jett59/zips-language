%require "3.8"
%language "c++"
%define api.token.constructor
%define api.value.type variant
%define api.value.automove

%define api.namespace { zips }

%define api.parser.class  { Parser }

%code requires {
    #include "ast.h"

    namespace zips {
        class Lexer;
    }
}

%debug

%locations

%define parse.error verbose

%{

#include <cstdint>
#include <iostream>
#include <memory>
#include "error.h"
#include "ast.h"
#include "lexer.h"

zips::Parser::symbol_type yylex(zips::Lexer& lexer) {
    return lexer.next();
}

using std::make_unique;
using std::move;

%}

%lex-param { zips::Lexer& lexer }
%parse-param { zips::Lexer& lexer }
%parse-param { std::string fileName }
%parse-param {std::unique_ptr<zips::AstNode> *ast}

%token <std::string> IDENTIFIER "identifier"

%token END 0 "EOF"

%start compilation_unit

%%

compilation_unit: IDENTIFIER {
    std::cout << $1 << std::endl;
}

%%

void zips::Parser::error(const zips::location& location, const std::string& message) {
    zips::error(fileName, location.begin.line, location.begin.column, message);
}
