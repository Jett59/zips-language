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

.*                { return MAKE_PARAMS(IDENTIFIER, yytext); }

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
