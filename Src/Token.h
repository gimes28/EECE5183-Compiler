#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum TokenType {
    // Reserved words
    T_PROGRAM = 257,
    T_IS = 258,
    T_BEGIN = 259,
    T_END = 260,
    T_GLOBAL = 261,
    T_PROCEDURE = 262,
    T_VARIABLE = 263,
    T_INTEGER = 264,
    T_FLOAT = 265,
    T_STRING = 266,
    T_BOOL = 267,

    T_IF = 268,
    T_THEN = 269,
    T_ELSE = 270,
    T_FOR = 271,
    T_RETURN = 272,
    T_NOT = 273,
    T_TRUE = 274,
    T_FALSE = 275,

    // Single char tokens
    T_PERIOD = '.',
    T_SEMICOLON = ';',
    T_LPAREN = '(',
    T_RPAREN = ')',
    T_COMMA = ',',
    T_LBRACKET = '[',
    T_RBRACKET = ']',
    T_LBRACE = '{',
    T_RBRACE = '}',
    T_PLUS = '+',
    T_MINUS = '-',
    T_MULTIPLY = '*',
    T_DIVIDE = '/',
    T_LESS = '<',
    T_GREATER = '>',
    T_COLON = ':',
    T_AND = '&',
    T_OR = '|',

    // Operators
    T_LESS_EQ = 280,
    T_GREATER_EQ = 281,
    T_EQUAL = 282,
    T_NOT_EQUAL = 283,
    T_ASSIGNMENT = 286,

    T_IDENTIFIER = 300,

    // Constants
    T_INTEGER_CONST = 305,
    T_FLOAT_CONST = 306,
    T_STRING_CONST = 307,

    T_EOF = 349,
    T_UNK = 350
};

struct Token{
    TokenType tt;
    struct 
    {
        std::string stringVal;
        int intVal;
        double floatVal;
    } val;
};

const char* getTokenTypeName(Token tok);

#endif