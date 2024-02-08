#include "Lexer.h"
#include "Token.h"
#include <iostream>

Lexer::Lexer(){
    charClass['0'] = NUM;
    charClass['1'] = NUM;
    charClass['2'] = NUM;
    charClass['3'] = NUM;
    charClass['4'] = NUM;
    charClass['5'] = NUM;
    charClass['6'] = NUM;
    charClass['7'] = NUM;
    charClass['8'] = NUM;
    charClass['9'] = NUM;
    charClass['!'] = SPECIAL;
    charClass['&'] = SPECIAL;
    charClass['*'] = SPECIAL;
    charClass['('] = SPECIAL;
    charClass[')'] = SPECIAL;
    charClass['-'] = SPECIAL;
    charClass['='] = SPECIAL;
    charClass['+'] = SPECIAL;
    charClass['['] = SPECIAL;
    charClass[']'] = SPECIAL;
    charClass['{'] = SPECIAL;
    charClass['}'] = SPECIAL;
    charClass['|'] = SPECIAL;
    charClass[':'] = SPECIAL;
    charClass[';'] = SPECIAL;
    charClass['\"'] = SPECIAL;
    charClass['|'] = SPECIAL;
    charClass[','] = SPECIAL;
    charClass['.'] = SPECIAL;
    charClass['<'] = SPECIAL;
    charClass['>'] = SPECIAL;
    charClass['/'] = SPECIAL;
    charClass['a'] = LOWER_ALPHA;
    charClass['b'] = LOWER_ALPHA;
    charClass['c'] = LOWER_ALPHA;
    charClass['d'] = LOWER_ALPHA;
    charClass['e'] = LOWER_ALPHA;
    charClass['f'] = LOWER_ALPHA;
    charClass['g'] = LOWER_ALPHA;
    charClass['h'] = LOWER_ALPHA;
    charClass['i'] = LOWER_ALPHA;
    charClass['j'] = LOWER_ALPHA;
    charClass['k'] = LOWER_ALPHA;
    charClass['l'] = LOWER_ALPHA;
    charClass['m'] = LOWER_ALPHA;
    charClass['n'] = LOWER_ALPHA;
    charClass['o'] = LOWER_ALPHA;
    charClass['p'] = LOWER_ALPHA;
    charClass['q'] = LOWER_ALPHA;
    charClass['r'] = LOWER_ALPHA;
    charClass['s'] = LOWER_ALPHA;
    charClass['t'] = LOWER_ALPHA;
    charClass['u'] = LOWER_ALPHA;
    charClass['v'] = LOWER_ALPHA;
    charClass['w'] = LOWER_ALPHA;
    charClass['x'] = LOWER_ALPHA;
    charClass['y'] = LOWER_ALPHA;
    charClass['z'] = LOWER_ALPHA;
    charClass['A'] = UPPER_ALPHA;
    charClass['B'] = UPPER_ALPHA;
    charClass['C'] = UPPER_ALPHA;
    charClass['D'] = UPPER_ALPHA;
    charClass['E'] = UPPER_ALPHA;
    charClass['F'] = UPPER_ALPHA;
    charClass['G'] = UPPER_ALPHA;
    charClass['H'] = UPPER_ALPHA;
    charClass['I'] = UPPER_ALPHA;
    charClass['J'] = UPPER_ALPHA;
    charClass['K'] = UPPER_ALPHA;
    charClass['L'] = UPPER_ALPHA;
    charClass['M'] = UPPER_ALPHA;
    charClass['N'] = UPPER_ALPHA;
    charClass['O'] = UPPER_ALPHA;
    charClass['P'] = UPPER_ALPHA;
    charClass['Q'] = UPPER_ALPHA;
    charClass['R'] = UPPER_ALPHA;
    charClass['S'] = UPPER_ALPHA;
    charClass['T'] = UPPER_ALPHA;
    charClass['U'] = UPPER_ALPHA;
    charClass['V'] = UPPER_ALPHA;
    charClass['W'] = UPPER_ALPHA;
    charClass['X'] = UPPER_ALPHA;
    charClass['Y'] = UPPER_ALPHA;
    charClass['Z'] = UPPER_ALPHA;
    charClass[' '] = SPACE;
    charClass['\t'] = SPACE;
    charClass['\n'] = SPACE;
}

Lexer::~Lexer(){
    file.close();
}

bool Lexer::LoadFile(std::string fileName){
    file.open(fileName);
    if(file.is_open()){
        return true;
    }
    return false;
}

Token Lexer::ScanToken(){
    Token token = Token();
    token.tt = scanToken();
    return token;
}

int Lexer::scanToken(){
    
    if(!file.is_open()){
        // report error
        return T_EOF;
    }

    char ch = file.get();
    int chClass = getCharClass(ch);

    // remove whitespace

    while (chClass == SPACE){
        ch = file.get();
        chClass = getCharClass(ch);
    }

    switch(ch){
    case(NUM):

    return T_FLOAT;
    case(SPECIAL):
    return T_SEMICOLON;
    case(LOWER_ALPHA):
    return T_IDENTIFIER;
    case(UPPER_ALPHA):
    return T_IDENTIFIER;
    default:
    if(ch == EOF) return T_EOF;
    return T_UNK;
    }


    while(!file.eof()){
        std::cout << (char)file.get() << std::endl;
    }
    return T_EOF;
}

int Lexer::getCharClass(char t){
    if(charClass.find(t) == charClass.end()){
        // Return error for invalid token or eof
        return -1;
    }
    return charClass[t];
}