#include "Lexer.h"
#include "Token.h"
#include "Error.h"
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
        lineCount = 1;
        return true;
    }
    Error::ReportError(ERROR_FAIL_TO_OPEN, "Cannot open file: " + fileName);
    return false;
}

int Lexer::getLineNumber(){
    return lineCount;
}

int Lexer::getCharClass(char t){
    if(charClass.find(t) == charClass.end()){
        // Return error for invalid token or eof
        return -1;
    }
    return charClass[t];
}

Token Lexer::ScanToken(){
    Token tok = Token();
    if(!file.is_open()){
        // report error
        tok.tt = T_UNK;
        return tok;
    }
    
    char nextCh, ch;
    int chClass;

    // remove whitespace   
    do {
        do{
            ch = file.get();
            if(ch == '\n')
                lineCount++;
            chClass = getCharClass(ch);
        } while(chClass == SPACE);
    
        if(ch == '/'){
            nextCh = file.get();
            //check for single line comment
            if(nextCh == '/'){
                do{
                    nextCh = file.get();
                }
                while(nextCh != '\n' && nextCh != EOF);
                file.unget();
            }
            //check for multiline comment
            else if(nextCh == '*'){
                int layer = 1;
                while(layer > 0){
                    nextCh = file.get();
                    if(nextCh == '*'){
                        nextCh = file.get();
                        if(nextCh == '/'){
                            layer--;
                        }
                    }
                    else if(nextCh == '/'){
                        nextCh = file.get();
                        if(nextCh == '*')
                            layer++;
                    }
                }
            }
            else{
                file.unget();
                break;
            }
        }
    } while (chClass == SPACE || ch == '/');

    int size = 0;
    switch(chClass){
    case(NUM):
        tok.tt = T_INTEGER_CONST; break;
    case(SPECIAL):
        switch (ch)
        {
        case '&': tok.tt = T_AND; break;
        case '(': tok.tt = T_LPAREN; break;
        case ')': tok.tt = T_RPAREN; break;
        case '-': tok.tt = T_MINUS; break;
        case '+': tok.tt = T_PLUS; break;
        case '*': tok.tt = T_MULTIPLY; break;
        case '/': tok.tt = T_DIVIDE; break;
        case '[': tok.tt = T_LBRACKET; break;
        case ']': tok.tt = T_RBRACKET; break;
        case '{': tok.tt = T_LBRACE; break;
        case '}': tok.tt = T_RBRACE; break;
        case '|': tok.tt = T_OR; break;
        case ';': tok.tt = T_SEMICOLON; break;
        case ',': tok.tt = T_COMMA; break;
        case '.': tok.tt =  T_PERIOD; break;
        case '<':
            nextCh = file.get();
            if (nextCh == '=')
                tok.tt = T_LESS_EQ;
            else
                tok.tt = T_LESS;
            break;
        case '>':
            nextCh = file.get();
            if (nextCh == '=')
                tok.tt = T_GREATER_EQ;
            else
                tok.tt = T_GREATER;
            break;
        case '=':
            nextCh = file.get();
            if (nextCh == '=')
                tok.tt = T_EQUAL;
            else 
                tok.tt = T_UNK;
            break;
        case '!': 
            nextCh = file.get();
            if (nextCh == '=')
                tok.tt = T_NOT_EQUAL;
            else
                tok.tt = T_NOT; 
            break;
        case ':': 
            nextCh = file.get();
            if(nextCh == '=')
                tok.tt = T_ASSIGNMENT;
            else
                tok.tt = T_UNK; 
            break;
        case '\"': 
            tok.tt = T_STRING_CONST;

            nextCh = file.get();
            while(nextCh != '\"'){
                if(nextCh == '\n' || nextCh == EOF) {
                    tok.tt = T_UNK;
                    // Report Error
                    break;
                } else if (size >= 255){
                    tok.tt = T_UNK;
                    // Report Error
                    break;
                }

                tok.val.stringVal[size++] = '\0';
                break;
            }
        default:
            tok.tt = T_UNK;
            //errorStatus = true;
            break;
        }
    case(LOWER_ALPHA):
        tok.tt = T_IDENTIFIER; break;
    case(UPPER_ALPHA):
        tok.tt = T_IDENTIFIER; break;
    default: 
        if(ch == EOF)
            tok.tt = T_EOF;
        else 
            tok.tt = T_UNK;
        break;
    }
    return tok;
}