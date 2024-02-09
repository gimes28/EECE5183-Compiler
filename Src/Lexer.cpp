#include "Lexer.h"
#include "Token.h"
#include "Error.h"
#include "SymbolTable.h"

#include <iostream>
#include <iomanip>
#include <string>

SymbolTable symTable;

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

void Lexer::SetDebugOption(bool opt){
    debugOption = opt;
}

bool Lexer::GetDebugOption(){
    return debugOption;
}

void Lexer::Debug(Token tok){
    if(debugOption){
        std::cout << "Token: ";
        std::cout << std::setw(4) << getTokenTypeName(tok) << " ";
        switch(tok.tt) {
        case(T_INTEGER_CONST):
            std::cout << tok.val.intVal;
            break;
        case(T_DOUBLE_CONST):
            std::cout << tok.val.doubleVal;
            break;
        case(T_STRING_CONST || T_IDENTIFIER):
            std::cout << tok.val.stringVal;
        }
        std::cout << std::endl;
    }
}

int Lexer::getCharClass(char t){
    if(charClass.find(t) == charClass.end()){
        Error::ReportError(ERROR_INVALID_TYPE, "Invalid Type: " + t);
        return EOF;
    }
    return charClass[t];
}

void Lexer::PopSymbolTable(Token tok){
    std::string val = "";
    switch(tok.tt) {
    case(T_INTEGER_CONST):
        val = std::to_string(tok.val.intVal);
        break;
    case(T_DOUBLE_CONST):
        val = std::to_string(tok.val.doubleVal);
        break;
    case(T_STRING_CONST || T_IDENTIFIER):
        val = tok.val.stringVal;
    }
    symTable.setSymbol(val, tok);
}

Token Lexer::ScanToken(){
    Token tok = Token();
    if(!file.is_open()){
        Error::ReportError(ERROR_FAIL_TO_OPEN, "File failed to open");
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
            if (chClass == INVALID){
                Error::ReportError(ERROR_INVALID_TYPE, "Invalid Charactor");
            }
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
                    // Can break with no closing comment
                    if(nextCh == '\n')
                        lineCount++;
                    else if (nextCh == EOF)
                        break;
                }
            }
            else{
                file.unget();
                break;
            }
        }
    } while (ch == '/');

    int size = 0;
    std::string val = "";
    switch(chClass){
    case(SPECIAL):{
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
            case '.': tok.tt = T_PERIOD; 
            break;
            case '<':
                ch = file.get();
                if (ch == '=')
                    tok.tt = T_LESS_EQ;
                else{
                    tok.tt = T_LESS;
                    file.unget();
                }
                break;
            case '>':
                ch = file.get();
                if (ch == '=')
                    tok.tt = T_GREATER_EQ;
                else{
                    tok.tt = T_GREATER;
                    file.unget();
                }
                break;
            case '=':
                ch = file.get();
                if (ch == '=')
                    tok.tt = T_EQUAL;
                else {
                    tok.tt = T_UNK;
                    file.unget();
                }
                break;
            case '!': 
                ch = file.get();
                if (ch == '=')
                    tok.tt = T_NOT_EQUAL;
                else{
                    tok.tt = T_NOT;
                    file.unget(); 
                }
                break;
            case ':': 
                ch = file.get();
                if(ch == '=')
                    tok.tt = T_ASSIGNMENT;
                else
                    tok.tt = T_UNK;                 
                break;
            case '\"': 
                tok.tt = T_STRING_CONST;

                ch = file.get();
                while(ch != '\"'){
                    if(ch == '\n')
                        lineCount++;
                    else if (ch == EOF){
                        tok.tt = T_UNK;
                        Error::ReportError(ERROR_UNKNOWN, "String missing ending quote");
                        break;
                    } 
                    else if (size >= 255){
                        tok.tt = T_UNK;
                        Error::ReportError(ERROR_UNKNOWN, "String is too large");
                        break;
                    }
                    tok.val.stringVal[size++] = ch;
                    ch = file.get();
                }
                tok.val.stringVal[size] = '0';
                break;
            default:
                tok.tt = T_UNK;
                break;
        }
        break;
    }
    case(NUM):{
        do{            
            if(ch != '_'){
                val += ch;
            }
            ch = file.get();
        } while(getCharClass(ch) == NUM || ch == '_');

        //check for float value
        if (ch == '.'){
            do{
                if(ch != '_'){
                    val += ch;
                }
            } while(getCharClass(ch) == NUM || ch == '_');
            tok.val.doubleVal += std::stod(val);
            tok.tt = T_DOUBLE_CONST;
            file.unget();
        }
        //integer value
        else{
            tok.val.intVal += std::stoi(val);
            tok.tt = T_INTEGER_CONST;
            file.unget();
        }
        break;
    }
    case(LOWER_ALPHA):
    case(UPPER_ALPHA):
        do {
            //keywords and identifiers are stored lower
            size = 0;
            if (chClass == UPPER_ALPHA){
                ch += ('a' - 'A');
            }
            tok.val.stringVal[size++] += ch;
            std::cout << ch << std::endl; 
            ch = file.get();
            chClass = getCharClass(ch);
        } while (chClass == UPPER_ALPHA || chClass == LOWER_ALPHA ||  
                chClass == NUM || ch == '_');
        file.unget();

        if(symTable.hasSymbol(tok.val.stringVal)){
            tok.tt = symTable.getSymbol(tok.val.stringVal).tt;
        }
        else{
            tok.tt = T_IDENTIFIER;
        }
        break;
    default: {
        if(ch == EOF)
            tok.tt = T_EOF;
        else 
            tok.tt = T_UNK;
        break;
    }
    }

    Debug(tok);
    PopSymbolTable(tok);

    return tok;
}