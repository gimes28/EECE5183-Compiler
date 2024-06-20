#include "Error.h"
#include "Lexer.h"
#include "ScopeHandler.h"
#include "Token.h"

#include <iomanip>
#include <iostream>
#include <string>

Error errTable;

Lexer::Lexer(ScopeHandler* scoperPtr){
    scoper = scoperPtr;

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
    this->fileName = fileName;
    file.open(fileName);
    if(file.is_open()){
        lineCount = 1;
        return true;
    }
    errTable.ReportError(ERROR_FAIL_TO_OPEN, fileName, lineCount);
    return false;
}

int Lexer::GetLineNumber(){
    return lineCount;
}

std::string Lexer::GetFileName(){
    return fileName;
}

void Lexer::SetDebugOption(bool opt){
    debugOption = opt;
}

bool Lexer::GetDebugOption(){
    return debugOption;
}

void Lexer::Debug(Token tok){
    if(debugOption){
        std::cout << "Token: " << std::left;
        std::cout << std::setw(20) << GetTokenTypeName(tok.tt);
        switch(tok.tt) {
        case(T_INTEGER_CONST):
            std::cout << tok.val.intVal << std::endl;
            break;
        case(T_FLOAT_CONST):
            std::cout << tok.val.floatVal << std::endl;
            break;
        default:
            std::cout << tok.val.stringVal << std::endl;
            break;
        }
    }   
}

int Lexer::GetCharClass(char t){
    if(t == EOF)
        return EOF;
    else if(charClass.find(t) == charClass.end())
        return INVALID;
    return charClass[t];
}

Token Lexer::InitScan(){
    Token tok = ScanToken();
    Debug(tok);
    return tok;
}

Token Lexer::ScanToken(){
    Token tok = Token();
    if(!file.is_open()){
        errTable.ReportError(ERROR_FAIL_TO_OPEN, fileName, lineCount);
        tok.tt = T_UNK;
        return tok;
    }
    
    char nextCh, ch;
    int chClass;
    do {
        do{
            ch = file.get();
            chClass = GetCharClass(ch);
            //Check for LR
            if(ch == '\n')
                lineCount++;
            //Check for CRLF
            else if(ch == '\r'){
                ch = file.get();
                if(ch == '\n')
                    lineCount++;
                else{
                    file.unget();
                    std::string errorChar;
                    errorChar += ch;
                    errTable.ReportError(ERROR_INVALID_INPUT, fileName, lineCount, "\'" + errorChar + "\'");
                }
            }
            else if (chClass == INVALID){
                std::string errorChar;
                errorChar += ch;
                errTable.ReportError(ERROR_INVALID_CHARACTER, fileName, lineCount, "\'" + errorChar + "\'");
            }

        } while(chClass == SPACE || chClass == INVALID);

        //Grab comments
        if(ch == '/'){
            nextCh = file.get();
            //Check for single line comment
            if(nextCh == '/'){
                do{
                    nextCh = file.get();
                }
                while(nextCh != '\n' && nextCh != EOF);
                file.unget();
            }
            //Check for multiline comments and nested comments
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

    std::string val = "";
    switch(chClass){
    case(SPECIAL):{
        switch (ch)
        {   
            //Check Special Chars
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
            //Check Operators
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
                else{
                    file.unget();
                    tok.tt = T_COLON;  
                }                                   
                break;
            case '\"': 
                tok.tt = T_STRING_CONST;

                ch = file.get();
                while(ch != '\"'){
                    if(ch == '\n')
                        lineCount++;
                    else if (ch == EOF){
                        tok.tt = T_UNK;
                        errTable.ReportError(ERROR_MISSING_STRING_CLOSING, fileName, lineCount);
                        break;
                    } 
                    tok.val.stringVal += ch;
                    ch = file.get();
                }
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
            chClass = GetCharClass(ch);
        } while(chClass == NUM || ch == '_');

        //Check for float value
        if (ch == '.'){
            do{
                if(ch != '_'){
                    val += ch;
                }
                ch = file.get();
                chClass = GetCharClass(ch);
            } while(chClass == NUM || ch == '_');
            tok.val.floatVal += std::stod(val);
            tok.tt = T_FLOAT_CONST;
            file.unget();
        }
        //Integer value
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
            //Keywords and Identifiers stored lower
            if (chClass == UPPER_ALPHA){
                ch += ('a' - 'A');
            }
            tok.val.stringVal += ch;
            ch = file.get();
            chClass = GetCharClass(ch);
        } while (chClass == UPPER_ALPHA || chClass == LOWER_ALPHA ||  
                chClass == NUM || ch == '_');
        file.unget();
        //push to symbolTable if in
        if(scoper->HasSymbol(tok.val.stringVal, true)){
            tok.tt = scoper->GetSymbol(tok.val.stringVal, true).tt;
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
    return tok;
}