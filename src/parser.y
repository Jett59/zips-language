%require "3.8"
%language "c++"
%define api.token.constructor
%define api.value.type variant
%define api.value.automove
%define parse.lac "full"

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
#include <vector>
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

%token LET "let"

%token I8 "i8" I16 "i16" I32 "i32" I64 "i64"
%token U8 "u8" U16 "u16" U32 "u32" U64 "u64"
%token ISIZE "isize" USIZE "usize"

%token EQUALS "="
%token LEFT_PAREN "(" RIGHT_PAREN ")" LEFT_BRACKET "[" RIGHT_BRACKET "]" LEFT_BRACE "{" RIGHT_BRACE "}"
%token COMMA "," COLON ":"

%token END 0 "EOF"

%type <std::unique_ptr<AstNode>> definition function
%type <std::vector<std::unique_ptr<AstNode>>> definitions statement-list
%type <std::unique_ptr<Type>> type primitive-type
%type <NamedType> named-type
%type <std::vector<NamedType>> parameter-list

%start compilation_unit

%%

compilation_unit: definitions {
    *ast = make_unique<CompilationUnitNode>($1);
}

definitions: definitions definition {
    auto definitions = $1;
    definitions.push_back($2);
    $$ = std::move(definitions);
}
| {
    $$ = std::vector<std::unique_ptr<AstNode>>{};
}

definition: function

function: "let" IDENTIFIER "(" parameter-list ")" "=" "{" statement-list "}" {
    $$ = make_unique<FunctionNode>($2, $4, $8);
}

parameter-list: parameter-list "," named-type {
    auto parameterList = $1;
    parameterList.push_back($3);
    $$ = std::move(parameterList);
}
| named-type {
    std::vector<NamedType> parameterList;
    parameterList.push_back($1);
    $$ = std::move(parameterList);
}
| {
    $$ = std::vector<NamedType>{};
}

named-type: IDENTIFIER ":" type {
    $$ = {$1, $3};
}

type: primitive-type

primitive-type: 
"i8" {
    $$ = make_unique<PrimitiveTypeNode>(PrimitiveTypeType::I8);
}
| "i16" {
    $$ = make_unique<PrimitiveTypeNode>(PrimitiveTypeType::I16);
}
| "i32" {
    $$ = make_unique<PrimitiveTypeNode>(PrimitiveTypeType::I32);
}
| "i64" {
    $$ = make_unique<PrimitiveTypeNode>(PrimitiveTypeType::I64);
}
| "u8" {
    $$ = make_unique<PrimitiveTypeNode>(PrimitiveTypeType::U8);
}
| "u16" {
    $$ = make_unique<PrimitiveTypeNode>(PrimitiveTypeType::U16);
}
| "u32" {
    $$ = make_unique<PrimitiveTypeNode>(PrimitiveTypeType::U32);
}
| "u64" {
    $$ = make_unique<PrimitiveTypeNode>(PrimitiveTypeType::U64);
}
| "isize" {
    $$ = make_unique<PrimitiveTypeNode>(PrimitiveTypeType::ISIZE);
}
| "usize" {
    $$ = make_unique<PrimitiveTypeNode>(PrimitiveTypeType::USIZE);
}

statement-list: {
    $$ = std::vector<std::unique_ptr<AstNode>>{};
}

%%

void zips::Parser::error(const zips::location& location, const std::string& message) {
    zips::error(fileName, location.begin.line, location.begin.column, message);
}
