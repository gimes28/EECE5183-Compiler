#ifndef TOKEN_H
#define TOKEN_H

#include <string>

#define T_LPAREN '('
#define T_RPAREN ')'
#define T_LCURLBRACKT '{'
#define T_RCURLBRACKT '}'
#define T_LSQBRACKET '['
#define T_RSQBRACKET ']'
#define T_SEMICOLON ';'
#define T_COLON ':'
#define T_ASSIGN '='
#define T_ADD '+'
#define T_SUBTRACT '-'
#define T_MULTIPLY
#define T_DIVIDE'
#define T_COMMA ','
#define T_AND '&'
#define T_OR '|'

#define T_LESSTHAN 257
#define T_LESSEQUAL 258
#define T_GREATERTHAN 259
#define T_GREATEREQUAL 260
#define T_EQUALS 261
#define T_NOTEQUALS 262

struct Token{
    int tt;
    union 
    {
        std::string stringVal;
        int intVal;
        double doubleVal;
    } val;

    int lineNum;
    int startChar;
};

#endif