%option nodefault
%option noyywrap
%option nounistd
%option never-interactive

%{
    #include <cstdint>
#include "parser.hh"
#include "location.hh"
#include "lexer.h"

#define YY_USER_ACTION updateLocation(yytext);

#define MAKE(TYPE)  Parser::make_ ## TYPE (currentLocation)
#define MAKE_PARAMS(TYPE, ...) Parser::make_ ## TYPE (__VA_ARGS__, currentLocation)

#define yyterminate() MAKE(END)

%}

%option c++
%option yyclass="Lexer"

%%

"\n"|" "|"\t"|"\r"|"\v"|"\f" ;

"let" return MAKE(LET);

"i8" return MAKE(I8);
"i16" return MAKE(I16);
"i32" return MAKE(I32);
"i64" return MAKE(I64);
"u8" return MAKE(U8);
"u16" return MAKE(U16);
"u32" return MAKE(U32);
"u64" return MAKE(U64);
"isize" return MAKE(ISIZE);
"usize" return MAKE(USIZE);

[a-zA-Z_][a-zA-Z0-9_]*                return MAKE_PARAMS(IDENTIFIER, yytext);

"=" return MAKE(EQUALS);

"(" return MAKE(LEFT_PAREN);
")" return MAKE(RIGHT_PAREN);
"[" return MAKE(LEFT_BRACKET);
"]" return MAKE(RIGHT_BRACKET);
"{" return MAKE(LEFT_BRACE);
"}" return MAKE(RIGHT_BRACE);

"," return MAKE(COMMA);
":" return MAKE(COLON);

"+" return MAKE(PLUS);
"-" return MAKE(MINUS);
"*" return MAKE(STAR);
"/" return MAKE(SLASH);

<<EOF>> return yyterminate();

. {
    std::cerr << "Unknown character " << yytext << std::endl;
    return yyterminate();
}

%%

void zips::Lexer::updateLocation(const char* token) {
    currentLocation.begin = currentLocation.end;
    int tokenLength = strlen(token);
    for (int i = 0; i < tokenLength; i ++) {
        char c = token[i];
        if (c == '\n') {
            currentLocation.end.line++;
            currentLocation.end.column = 1;
        }else {
            currentLocation.end.column++;
        }
    }
}
