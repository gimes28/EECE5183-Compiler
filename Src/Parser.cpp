#include "Error.h"
#include "Parser.h"
#include "Lexer.h"
#include "ScopeHandler.h"
#include "Symbol.h"

#include "llvm/IR/Constants.h"
#include "llvm/IR/IRPrintingPasses.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Verifier.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"

#include <iostream>
#include <vector>

Parser::Parser(Lexer* lexerptr, ScopeHandler* scoperPtr){
    lexer = lexerptr;
    scoper = scoperPtr;

    llvmContext = new llvm::LLVMContext();
    llvmBuilder = new llvm::IRBuilder<>(*llvmContext);
}

Parser::~Parser(){
    delete llvmBuilder;
    delete llvmModule;
    delete llvmContext;
}

bool Parser::IsTokenType(TokenType token){
    if(tok.tt == token){
        tok = lexer->InitScan();
        return true;
    }
    return false;
}

void Parser::SetDebugOptionParser(bool debug){
    debugOptionParser = debug;
}

void Parser::SetDebugOptionCodeGen(bool debug){
    debugOptionCodeGen = debug;
}

void Parser::DebugParseTrace(std::string message){
    if(debugOptionParser)
        std::cout << "Parse: " << message << std::endl;
}

bool Parser::OutputAssembly(){

    bool errMod = llvm::verifyModule(*llvmModule, &llvm::errs());
    if (errMod){
        errTable.ReportError(ERROR_LLVM_INVALID_MODULE, lexer->GetFileName(), lexer->GetLineNumber());
        if(debugOptionCodeGen)
            llvmModule->print(llvm::outs(), nullptr);
        return false;
    }  

    // Initlalize target registry
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();

    auto targetTriple = llvm::sys::getDefaultTargetTriple();
    llvmModule->setTargetTriple(targetTriple);

    std::string err;
    auto target = llvm::TargetRegistry::lookupTarget(targetTriple, err);

    // If no targets are found
    if(!target){
        llvm::errs() << err;
        return false;
    }

    auto cpu = "generic";
    auto features = "";

    llvm::TargetOptions opt;
    auto rm = llvm::Optional<llvm::Reloc::Model>();
    auto targetMachine = target->createTargetMachine(targetTriple, cpu, features, opt, rm);

    llvmModule->setDataLayout(targetMachine->createDataLayout());

    std::string filename = "out.s";
    std::error_code errCode;
    llvm::raw_fd_ostream dest(filename, errCode, llvm::sys::fs::OF_None);

    if(errCode){
        llvm::errs() << "Could not open output file: " << errCode.message();
        return false;
    }

    llvm::legacy::PassManager pm;
    auto fileType = llvm::CGFT_AssemblyFile;

    if (targetMachine->addPassesToEmitFile(pm, dest, nullptr, fileType)) {
        llvm::errs() << "TargetMachine cannot emit a file of this type.";
        return false;
    }

    if(debugOptionCodeGen){
        std::string filename2 = "out.ll";
        std::error_code errCode2;
        llvm::raw_fd_ostream dest2(filename2, errCode2, llvm::sys::fs::OF_None);

        if(errCode2){
            llvm::errs() << "Could not open output file: " << errCode2.message();
            return false;
        }

        pm.add(llvm::createPrintModulePass(dest2));

        pm.run(*llvmModule);
        dest.flush();
        dest2.flush();

        errTable.ReportDebug(lexer->GetFileName(), true, "Codegen succeeded");
    }
    else{
        pm.run(*llvmModule);
        dest.flush();
    }

    return true;
}

bool Parser::Parse(){
    tok = lexer->InitScan();

    bool success = Program();
    if(debugOptionParser){
        if(success)
            errTable.ReportDebug(lexer->GetFileName(), success, "Parse succeeded");
        else
            errTable.ReportDebug(lexer->GetFileName(), success, "Parse failed");
    }

    return success;
}

bool Parser::Program(){
    //Global scope is defined in scopeHandler
    if (!ProgramHeader())
        return false;

    // Code gen: main function 
    std::vector<llvm::Type*> params;
    llvm::FunctionType *funcType = llvm::FunctionType::get(llvmBuilder->getInt32Ty(), params, false);
    llvm::Function *func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, "main", *llvmModule);

    Symbol sym(T_IDENTIFIER, "main", ST_PROCEDURE, TYPE_INT);
    sym.llvmFunction = func;
    scoper->SetCurrentProcedure(sym);

    if (!ProgramBody())
        return false;

    if (!IsTokenType(T_PERIOD)){
        errTable.ReportError(ERROR_MISSING_PERIOD, lexer->GetFileName(), lexer->GetLineNumber());
        return false;
    }
    if(!IsTokenType(T_EOF))
        return false;
    scoper->ExitScope();
    return true;
}

bool Parser::ProgramHeader(){
    DebugParseTrace("Program Header");

    if (!IsTokenType(T_PROGRAM))
        return false;
    Symbol id;
    if (!Identifier(id)){
        errTable.ReportError(ERROR_INVALID_IDENTIFIER, lexer->GetFileName(), lexer->GetLineNumber(), "\'" + id.id + "\'");
        return false;
    }

    llvmModule = new llvm::Module(id.id, *llvmContext);

    // Insert runtime functions
    scoper->InitRuntimeFunctions(llvmModule, llvmBuilder);

    if(!IsTokenType(T_IS)){
        errTable.ReportError(ERROR_INVALID_HEADER, lexer->GetFileName(), lexer->GetLineNumber(), "Missing \'is\' in program header");
        return false;
    }
    return true;    
}

bool Parser::ProgramBody(){
    DebugParseTrace("Program Body");

    if (!DeclarationBlock())
        return false;

    if (!IsTokenType(T_BEGIN)){
        errTable.ReportError(ERROR_INVALID_BODY, lexer->GetFileName(), lexer->GetLineNumber(), "Missing \'begin\' in program body");
        return false;
    }

    llvm::Function *func = scoper->GetCurrentProcedure().llvmFunction;

    // Code gen: main entry point 
    llvm::BasicBlock *entry = llvm::BasicBlock::Create(*llvmContext, "entry", func);
    llvmBuilder->SetInsertPoint(entry);

    if (!StatementBlock())
        return false;
    
    if (!IsTokenType(T_END)){
        errTable.ReportError(ERROR_INVALID_BODY, lexer->GetFileName(), lexer->GetLineNumber(), "Missing \'end\' in program body");
        return false;
    }

    if (!IsTokenType(T_PROGRAM)){
        errTable.ReportError(ERROR_INVALID_BODY, lexer->GetFileName(), lexer->GetLineNumber(), "Missing \'program\' in program body");
        return false;
    }

    // Code Gen: Return Main function, return 0
    llvm::Value *retVal = llvm::ConstantInt::get(*llvmContext, llvm::APInt(32, 0, true));
    llvmBuilder->CreateRet(retVal);

    return true;
}

bool Parser::Declaration(){
    DebugParseTrace("Declaration");

    Symbol decl;
    decl.isGlobal = IsTokenType(T_GLOBAL) || scoper->IsGlobalScope(); // Outermost scope is global, regardless if keyword is used or not

    if (ProcedureDeclaration(decl)) {} 
    else if (VariableDeclaration(decl)) {}
    else
        return false;
    return true;
}

bool Parser::ProcedureDeclaration(Symbol &decl){
    DebugParseTrace("Procedure Declaration");

    if(!ProcedureHeader(decl))
        return false;

    decl.st = ST_PROCEDURE;

    if (scoper->HasSymbol(decl.id, decl.isGlobal)){
        errTable.ReportError(ERROR_DUPLICATE_IDENTIFIER, lexer->GetFileName(), lexer->GetLineNumber(), "\'" + decl.id + "\'");
        return false;
    }

    // Code gen: function 
    std::vector<llvm::Type*> params;
    for (auto &parm : decl.params){
        params.push_back(GetLLVMType(parm.type));
    }
    llvm::FunctionType *funcType = llvm::FunctionType::get(GetLLVMType(decl.type), params, false);
    llvm::Function *func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, decl.id, *llvmModule);

    int i = 0;
    for (auto &parm : func->args()){
        parm.setName(decl.params[i++].id);
    }
    decl.llvmFunction = func;

    scoper->SetSymbol(decl.id, decl, decl.isGlobal);

    // Set inside function so procedure symbol can be easily found for return type checking
    scoper->SetCurrentProcedure(decl);
    
    if(!ProcedureBody())
        return false;

    scoper->ExitScope();

    if (!decl.isGlobal){
        if (scoper->HasSymbol(decl.id, decl.isGlobal)){
            errTable.ReportError(ERROR_DUPLICATE_PROCEDURE, lexer->GetFileName(), lexer->GetLineNumber(), "\'" + decl.id + "\'");
            return false;
        }
        scoper->SetSymbol(decl.id, decl, decl.isGlobal);
    }

    return true;
}

bool Parser::ProcedureHeader(Symbol &decl){
    DebugParseTrace("Procedure Header");
    if(!IsTokenType(T_PROCEDURE))
        return false;

    scoper->NewScope();

    if (!Identifier(decl)){
        errTable.ReportError(ERROR_INVALID_IDENTIFIER, lexer->GetFileName(), lexer->GetLineNumber(), "\'" + decl.id + "\'");
        return false;
    }

    if(!IsTokenType(T_COLON)){
        errTable.ReportError(ERROR_MISSING_COLON, lexer->GetFileName(), lexer->GetLineNumber(), "Missing \':\' in procedure header");
        return false;
    }

    if(!TypeMark(decl)){
        errTable.ReportError(ERROR_INVALID_TYPE_MARK, lexer->GetFileName(), lexer->GetLineNumber());
        return false;
    }
    
    if(!IsTokenType(T_LPAREN)){
        errTable.ReportError(ERROR_MISSING_PAREN, lexer->GetFileName(), lexer->GetLineNumber(), "Missing \'(\' in procedure header");
        return false;
    }

    ParameterList(decl);
    
    if(listError)
        return false;

    if(!IsTokenType(T_RPAREN)){
        errTable.ReportError(ERROR_MISSING_PAREN, lexer->GetFileName(), lexer->GetLineNumber(), "Missing \')\' in procedure header");
        return false;
    }
    return true;
}

bool Parser::ParameterList(Symbol &decl){
    DebugParseTrace("Parameter List");

    Symbol param;
    if(!Parameter(param))
        return false;

    decl.params.push_back(param);

    while(IsTokenType(T_COMMA)){
        if (!Parameter(param)) {
            errTable.ReportError(ERROR_INVALID_PARAMETER, lexer->GetFileName(), lexer->GetLineNumber());
            listError = true;
            return false;
        }

        decl.params.push_back(param);
    }
    return true;
}

bool Parser::Parameter(Symbol &param){
    DebugParseTrace("Parameter");

    return VariableDeclaration(param);
}

bool Parser::ProcedureBody(){
    DebugParseTrace("Procedure Body");

    if (!DeclarationBlock())
        return false;

    if (!IsTokenType(T_BEGIN)){
        errTable.ReportError(ERROR_INVALID_BODY, lexer->GetFileName(), lexer->GetLineNumber(), "Missing \'begin\' in procedure body");
        return false;
    }
    
    Symbol procedure = scoper->GetCurrentProcedure();
    llvm::Function *func = procedure.llvmFunction;

    // Code gen: main entry point 
    llvm::BasicBlock *entry = llvm::BasicBlock::Create(*llvmContext, "entry", func);
    llvmBuilder->SetInsertPoint(entry);

    // Code gen: Parms and declared variables
    for(SymbolTable::iterator it = scoper->GetScopeBegin(); it != scoper->GetScopeEnd(); ++it){
        if (it->second.st != ST_VARIABLE)
            continue;

        // Allocate space
        llvm::Type *t = GetLLVMType(it->second.type);       
        llvm::Value *addr = llvmBuilder->CreateAlloca(t, nullptr, it->second.id);

        // Todo Arrays

        it->second.llvmAddress = addr;
    }

    // Code gen: Store argument values in allocated addresses
    auto arg = func->arg_begin();
    for(auto &pmtr : procedure.params){
        // Get symbol in params vector
        Symbol parameter = scoper->GetSymbol(pmtr.id);

        // Get arg value and go to next arg
        llvm::Value *argVal = arg++;

        // Store arg value in address
        llvmBuilder->CreateStore(argVal, parameter.llvmAddress);

        // Update symbol
        parameter.llvmValue = argVal;
        scoper->SetSymbol(parameter.id, parameter, parameter.isGlobal);

        // Todo Arrays
    }

    if (!StatementBlock())
        return false;
    
    if (!IsTokenType(T_END)){
        errTable.ReportError(ERROR_INVALID_BODY, lexer->GetFileName(), lexer->GetLineNumber(), "Missing \'end\' in procedure body");
        return false;
    }

    if (!IsTokenType(T_PROCEDURE)){
        errTable.ReportError(ERROR_INVALID_BODY, lexer->GetFileName(), lexer->GetLineNumber(), "Missing \'procedure\' in procedure body");
        return false;
    }

    // Verify function
    bool errFunc = llvm::verifyFunction(*func, &llvm::errs());
    if (errFunc){
        errTable.ReportError(ERROR_LLVM_INVALID_FUNCTION, lexer->GetFileName(), lexer->GetLineNumber(), "Error found within procedure");
        if(debugOptionCodeGen)
            llvmModule->print(llvm::outs(), nullptr);
        return false;
    }   

    return true;
}

bool Parser::VariableDeclaration(Symbol &decl){
    DebugParseTrace("Variable Declaration");

    if (!IsTokenType(T_VARIABLE))
        return false;

    decl.st = ST_VARIABLE;

    if (!Identifier(decl)){
        errTable.ReportError(ERROR_INVALID_IDENTIFIER, lexer->GetFileName(), lexer->GetLineNumber(), "\'" + decl.id + "\'");
        return false;
    }

    // Check duplicate variable
    if(scoper->HasSymbol(decl.id, decl.isGlobal)){
        errTable.ReportError(ERROR_DUPLICATE_VARIABLE, lexer->GetFileName(), lexer->GetLineNumber(), "\'" + decl.id + "\'");
        return false;
    }

    if(!IsTokenType(T_COLON)){
        errTable.ReportError(ERROR_MISSING_COLON, lexer->GetFileName(), lexer->GetLineNumber(), "Missing \':\' in variable declaration");
        return false;
    }

    if(!TypeMark(decl)){
        errTable.ReportError(ERROR_INVALID_TYPE_MARK, lexer->GetFileName(), lexer->GetLineNumber());
        return false;
    }
    
    if(IsTokenType(T_LBRACKET)){
        if(!Bound(decl)){
            errTable.ReportError(ERROR_INVALID_BOUND, lexer->GetFileName(), lexer->GetLineNumber());
            return false;
        }

        decl.isArr = true;

        if(!IsTokenType(T_RBRACKET)){
            errTable.ReportError(ERROR_MISSING_BRACKET, lexer->GetFileName(), lexer->GetLineNumber(), "Missing \']\' in variable bound");
            return false;
        }
    }

    // Code gen: Global declared variables
    if(decl.isGlobal){
        // Moved global declared variables to variable decleration from program body 
        // since global variables don't have llvmAddress when accessed inside procedures.
        llvm::Type *t = GetLLVMType(decl.type);

        //Outerscope == global
        llvm::Constant *val = llvm::Constant::getNullValue(t);        
        llvm::Value *addr = new llvm::GlobalVariable(*llvmModule, t, false, llvm::GlobalValue::ExternalLinkage, val, decl.id);

        // Todo Arrays

        decl.llvmAddress = addr;
    }

    scoper->SetSymbol(decl.id, decl, decl.isGlobal);

    return true;
}

bool Parser::TypeMark(Symbol &id){
    DebugParseTrace("Type Mark");

    if (IsTokenType(T_INTEGER))
        id.type = TYPE_INT;
    else if (IsTokenType(T_FLOAT))
        id.type = TYPE_FLOAT;
    else if (IsTokenType(T_STRING))
        id.type = TYPE_STRING;
    else if (IsTokenType(T_BOOL))
        id.type = TYPE_BOOL;
    else
        return false;
    return true;
}

bool Parser::Bound(Symbol &id){
    DebugParseTrace("Bound");

    Symbol num;

    int val = tok.val.intVal;

    if(Number(num) && num.type == TYPE_INT){
        id.arrSize = val;
        return true;
    }
    else{
        errTable.ReportError(ERROR_INVALID_BOUND, lexer->GetFileName(), lexer->GetLineNumber(), "Bound must be an integer");
        return false;
    }
}

bool Parser::Statement(){
    DebugParseTrace("Statement");

    if (AssignmentStatement()){}
    else if (IfStatement()){}
    else if (LoopStatement()){}
    else if (ReturnStatement()){}
    else 
        return false;
    return true;
}

bool Parser::AssignmentStatement(){
    DebugParseTrace("Assignment Statement");

    Symbol dest, exp;
    if(!Destination(dest))
        return false;

    if (!IsTokenType(T_ASSIGNMENT))
        return false;

    if (!Expression(exp))
        return false;
    
    if(!CompatibleTypeCheck(dest, exp))
        return false;
    
    // Code gen: Assignment statement
    llvmBuilder->CreateStore(exp.llvmValue, dest.llvmAddress);

    // Update symbol
    dest.llvmValue = exp.llvmValue;
    scoper->SetSymbol(dest.id, dest, dest.isGlobal);

    return true;    
}

bool Parser::Destination(Symbol &id){
    DebugParseTrace("Destination");

    if(!Identifier(id))
        return false;
    
    if(!scoper->HasSymbol(id.id)){
        errTable.ReportError(ERROR_SCOPE_DECLERATION, lexer->GetFileName(), lexer->GetLineNumber(), "Destination: \'" + id.id + "\'");
        return false;
    }

    id = scoper->GetSymbol(id.id);

    // Confirm id is a name
    if(id.st != ST_VARIABLE){
        errTable.ReportError(ERROR_INVALID_DESTINATION, lexer->GetFileName(), lexer->GetLineNumber(), "\'" + id.id + "\' is not a valid destination");
        return false;
    }

    if(!ArrayIndexAssist(id))
        return false;
    return true; 
}

bool Parser::IfStatement(){
    DebugParseTrace("If");

    if (!IsTokenType(T_IF))
        return false;

    if(!IsTokenType(T_LPAREN)){
        errTable.ReportError(ERROR_MISSING_PAREN, lexer->GetFileName(), lexer->GetLineNumber());
        return false;
    }

    Symbol exp;   
    if(!Expression(exp))
        return false;

    if(!IsTokenType(T_RPAREN)){
        errTable.ReportError(ERROR_MISSING_PAREN, lexer->GetFileName(), lexer->GetLineNumber(), "Missing \')\' in if statement");
        return false;
    }

    // Check and convert to bool
    if(exp.type == TYPE_INT){
        exp.type = TYPE_BOOL;
        exp.llvmValue = llvmBuilder->CreateICmpNE(exp.llvmValue, llvm::ConstantInt::get(*llvmContext, llvm::APInt(32, 0, true)));
    }
    else if (exp.type != TYPE_BOOL){
        errTable.ReportError(ERROR_MISSING_PAREN, lexer->GetFileName(), lexer->GetLineNumber(), "If statement expression must evaluate to bool");
        return false;
    }

    // Code gen: If statement
    llvm::Function *func = scoper->GetCurrentProcedure().llvmFunction;

    //Set condition value 
    llvm::Value *ifCond = llvmBuilder->CreateICmpNE(exp.llvmValue, llvm::ConstantInt::get(*llvmContext, llvm::APInt(1, 0, true)));
    exp.llvmValue = ifCond;

    // Create basic blocks for if and else then merge
    llvm::BasicBlock *ifThenBlock = llvm::BasicBlock::Create(*llvmContext, "ifThen", func);
    llvm::BasicBlock *ifElseBlock = llvm::BasicBlock::Create(*llvmContext, "ifElse", func);
    llvm::BasicBlock *ifMergeBlock = llvm::BasicBlock::Create(*llvmContext, "ifMerge", func);

    llvmBuilder->CreateCondBr(ifCond, ifThenBlock, ifElseBlock);
    llvmBuilder->SetInsertPoint(ifThenBlock);

    if(!IsTokenType(T_THEN)){
        errTable.ReportError(ERROR_INVALID_IF, lexer->GetFileName(), lexer->GetLineNumber(), "Missing \'then\' in if statement");
        return false;
    }

    if(!StatementBlock())
        return false;
    
    // Merge if block into merge block if no return
    if(llvmBuilder->GetInsertBlock()->getTerminator() == nullptr)
        llvmBuilder->CreateBr(ifMergeBlock);
        
    llvmBuilder->SetInsertPoint(ifElseBlock);

    if(IsTokenType(T_ELSE)){
        if(!StatementBlock())
            return false;
    }

    // Merge else block into merge block if no return
    if(llvmBuilder->GetInsertBlock()->getTerminator() == nullptr)
        llvmBuilder->CreateBr(ifMergeBlock);
        
    llvmBuilder->SetInsertPoint(ifMergeBlock);

    if(!IsTokenType(T_END)){
        errTable.ReportError(ERROR_INVALID_IF, lexer->GetFileName(), lexer->GetLineNumber(), "Missing \'end\' in if statement");
        return false;
    }

    if(!IsTokenType(T_IF)){
        errTable.ReportError(ERROR_INVALID_IF, lexer->GetFileName(), lexer->GetLineNumber(), "Missing closing \'if\'");
        return false;
    }

    return true; 
}

bool Parser::LoopStatement(){
    DebugParseTrace("Loop");

    if(!IsTokenType(T_FOR))
        return false;
     
    if (!IsTokenType(T_LPAREN)){
        errTable.ReportError(ERROR_MISSING_PAREN, lexer->GetFileName(), lexer->GetLineNumber(), "Missing \'(\' in loop");
        return false;   
    }

    if(!AssignmentStatement())
        return false;

    if(!IsTokenType(T_SEMICOLON)){
        errTable.ReportError(ERROR_MISSING_SEMICOLON, lexer->GetFileName(), lexer->GetLineNumber(), "Missing \':\' in loop");
        return false;
    }

    // Code gen: Loop statement
    llvm::Function *func = scoper->GetCurrentProcedure().llvmFunction;

    // Create basic blocks for loop header and body then merge
    llvm::BasicBlock *loopHeaderBlock = llvm::BasicBlock::Create(*llvmContext, "loopHead", func);
    llvm::BasicBlock *loopBodyBlock = llvm::BasicBlock::Create(*llvmContext, "loopBody", func);
    llvm::BasicBlock *loopMergeBlock = llvm::BasicBlock::Create(*llvmContext, "loopMerge", func);

    llvmBuilder->CreateBr(loopHeaderBlock);
    llvmBuilder->SetInsertPoint(loopHeaderBlock);

    Symbol exp;
    if(!Expression(exp))
        return false;    

    if(!IsTokenType(T_RPAREN)){
        errTable.ReportError(ERROR_MISSING_PAREN, lexer->GetFileName(), lexer->GetLineNumber(), "Missing \')\' in loop");
        return false;
    }

    // Check and convert to bool
    if(exp.type == TYPE_INT){
        exp.type = TYPE_BOOL;
        exp.llvmValue = llvmBuilder->CreateICmpNE(exp.llvmValue, llvm::ConstantInt::get(*llvmContext, llvm::APInt(32, 0, true)));
    }
    else if (exp.type != TYPE_BOOL){
        errTable.ReportError(ERROR_INVALID_EXPRESSION, lexer->GetFileName(), lexer->GetLineNumber(), "Loop statement expression must evaluate to bool");
        return false;
    }

    // Code gen: loop condition
    llvm::Value *loopCond = llvmBuilder->CreateICmpNE(exp.llvmValue, llvm::ConstantInt::get(*llvmContext, llvm::APInt(1, 0, true)));
    exp.llvmValue = loopCond;

    llvmBuilder->CreateCondBr(loopCond, loopBodyBlock, loopMergeBlock);

    // Loop body
    llvmBuilder->SetInsertPoint(loopBodyBlock);

    if(!StatementBlock())
        return false;

    // Return to header and recheck condition
    // Merge else block into merge block if no return
    if(llvmBuilder->GetInsertBlock()->getTerminator() == nullptr)
        llvmBuilder->CreateBr(loopHeaderBlock);
        
    llvmBuilder->SetInsertPoint(loopMergeBlock);
    
    if(!IsTokenType(T_END)){
        errTable.ReportError(ERROR_INVALID_LOOP, lexer->GetFileName(), lexer->GetLineNumber(), "Missing \'end\' in loop");
        return false;
    }

    if(!IsTokenType(T_FOR)){
        errTable.ReportError(ERROR_INVALID_LOOP, lexer->GetFileName(), lexer->GetLineNumber(), "Missing closing \'for\'");
        return false;
    }
    return true;
}

bool Parser::ReturnStatement(){
    DebugParseTrace("Return");

    if (!IsTokenType(T_RETURN))
        return false;

    Symbol exp;
    if(!Expression(exp))
        return false;
    
    // Type check match to procedure return type
    Symbol proc = scoper->GetCurrentProcedure();
    if(proc.type == TYPE_UNK){
        errTable.ReportError(ERROR_INVALID_RETURN, lexer->GetFileName(), lexer->GetLineNumber(), "Return statement must be within procedure");
        return false;
    }
    else if(!CompatibleTypeCheck(proc, exp)){
        return false;
    }

    // Code gen: Return
    llvmBuilder->CreateRet(exp.llvmValue);

    return true;
}

bool Parser::Identifier(Symbol &id){
    DebugParseTrace("Identifier");

    if (tok.tt == T_IDENTIFIER){
        id.id = tok.val.stringVal;
        id.tt = tok.tt;
    }
    return IsTokenType(T_IDENTIFIER);
}

bool Parser::Expression(Symbol &exp){
    DebugParseTrace("Expression");

    bool notToken = IsTokenType(T_NOT);

    if(!ArithOp(exp))
        return false;

    if(notToken){
        if(exp.type != TYPE_BOOL || exp.type != TYPE_INT){
            exp.llvmValue = llvmBuilder->CreateNot(exp.llvmValue);
        }
        else{
            errTable.ReportError(ERROR_INVALID_EXPRESSION, lexer->GetFileName(), lexer->GetLineNumber(), "\'not\' operator only defined for int and bool");
            return false;
        }
    }

    if(!ExpressionPrime(exp))
        return false;

    return true;
}

bool Parser::ExpressionPrime(Symbol &exp){
    DebugParseTrace("Expression Prime");

    Token op = tok;
    if (IsTokenType(T_AND) || IsTokenType(T_OR)){
        Symbol rhs;
        if(!ArithOp(rhs)){
            errTable.ReportError(ERROR_MISSING_OPERAND, lexer->GetFileName(), lexer->GetLineNumber());
            return false;
        }
        
        ExpressionTypeCheck(exp, rhs, op);

        if(!ExpressionPrime(exp))
            return false;
    }
    return true;
}

bool Parser::ArithOp(Symbol &aro){
    DebugParseTrace("Arithmetic Op");

    if(!Relation(aro))
        return false;

    if(!ArithOpPrime(aro))
        return false;
    
    return true;
}

bool Parser::ArithOpPrime(Symbol &aro){
    DebugParseTrace("Arithmetic Op Prime");

    Token op = tok;

    if (IsTokenType(T_PLUS) || IsTokenType(T_MINUS)){
        Symbol rhs;
        if(!Relation(rhs)){
            errTable.ReportError(ERROR_MISSING_OPERAND, lexer->GetFileName(), lexer->GetLineNumber());
            return false;
        }
        
        if(!ArithmeticTypeCheck(aro, rhs, op))
            return false;

        if(!ArithOpPrime(aro))
            return false;
    }
    return true;
}

bool Parser::Relation(Symbol &rel){
    DebugParseTrace("Relation");

    if(!Term(rel))
        return false;
    
    // Check and convert type for relation ops

    if(!RelationPrime(rel))
        return false;

    return true;
}

bool Parser::RelationPrime(Symbol &rel){
    DebugParseTrace("Relation Prime");

    Token op = tok;

    if(IsTokenType(T_LESS) || IsTokenType(T_LESS_EQ) || 
        IsTokenType(T_GREATER) || IsTokenType(T_GREATER_EQ) || 
        IsTokenType(T_EQUAL) || IsTokenType(T_NOT_EQUAL)){

        Symbol rhs;    
        if(!Term(rhs)){
            errTable.ReportError(ERROR_MISSING_OPERAND, lexer->GetFileName(), lexer->GetLineNumber());
            return false;
        }

        // Check and convert type for relation ops
        if(!RelationTypeCheck(rel, rhs, op))
            return false;

        rel.type = TYPE_BOOL;

        if(!RelationPrime(rel))
            return false;
    }
    return true;
}

bool Parser::Term(Symbol &trm){
    DebugParseTrace("Term");

    if(!Factor(trm))
        return false;
    
    if(!TermPrime(trm))
        return false;

    return true;
}

bool Parser::TermPrime(Symbol &trm){
    DebugParseTrace("Term Prime");

    Token op = tok;

    if(IsTokenType(T_MULTIPLY) || IsTokenType(T_DIVIDE)){
        Symbol rhs;
        if(!Factor(rhs)){
            errTable.ReportError(ERROR_MISSING_OPERAND, lexer->GetFileName(), lexer->GetLineNumber());
            return false;
        }
        // Check and convert for *

        if(!ArithmeticTypeCheck(trm, rhs, op))
            return false;

        if(!TermPrime(trm))
            return false;
    }
    return true;
}

bool Parser::Factor(Symbol &fac){
    DebugParseTrace("Factor");

    if(IsTokenType(T_LPAREN)){
        if(!Expression(fac))
            return false;
        
        if(!IsTokenType(T_RPAREN)){
            errTable.ReportError(ERROR_MISSING_PAREN, lexer->GetFileName(), lexer->GetLineNumber(), "Missing \')\' in expression factor");
            return false;
        }
    }
    else if(ProcedureCallOrName(fac)){}
    else if(IsTokenType(T_MINUS)){
        if (Name(fac) || Number(fac)){
            // Code Gen: Negative
            if(fac.type == TYPE_INT)
                fac.llvmValue = llvmBuilder->CreateNeg(fac.llvmValue);
            else if(fac.type == TYPE_FLOAT)
                fac.llvmValue = llvmBuilder->CreateFNeg(fac.llvmValue);
            else{
                errTable.ReportError(ERROR_INVALID_RELATION, lexer->GetFileName(), lexer->GetLineNumber(), "Minus operator only valid on integers or floats");
                return false;
            }            
        }
        else{
            errTable.ReportError(ERROR_INVALID_CHARACTER, lexer->GetFileName(), lexer->GetLineNumber(), "Invalid use of \'-\'");
            return false;
        }
    }
    else if(Number(fac)){}
    else if(String(fac)){}
    else if(IsTokenType(T_TRUE)){
        fac.tt = T_TRUE;
        fac.type = TYPE_BOOL;
        // Code Gen: True
        fac.llvmValue = llvm::ConstantInt::getTrue(*llvmContext);
    }
    else if(IsTokenType(T_FALSE)){
        fac.tt = T_FALSE;
        fac.type = TYPE_BOOL;
        // Code Gen: False
        fac.llvmValue = llvm::ConstantInt::getFalse(*llvmContext);
    }
    else
        return false;
    return true;
}

bool Parser::Name(Symbol &id){
    DebugParseTrace("Name");

    if(!Identifier(id))
        return false;
        
    if(!scoper->HasSymbol(id.id)){
        errTable.ReportError(ERROR_SCOPE_DECLERATION, lexer->GetFileName(), lexer->GetLineNumber(), "Name: \'" + id.id + "\'");
        return false;
    }

    id = scoper->GetSymbol(id.id);

    //Confirm id is a name
    if (id.st != ST_VARIABLE){
        errTable.ReportError(ERROR_INVALID_VARIABLE, lexer->GetFileName(), lexer->GetLineNumber(), "\'" + id.id + "\' is not a variable");
        return false;
    }

    if (!ArrayIndexAssist(id)){
        return false;
    }

    // Code gen: Name
    id.llvmValue = llvmBuilder->CreateLoad(GetLLVMType(id.type), id.llvmAddress);

    return true;
}

bool Parser::ArgumentList(Symbol &id, std::vector<llvm::Value*> &argList){
    DebugParseTrace("Argument List");

    Symbol arg;
    int ind = 0;

    do
    {   
        if(ind > 0)
            arg = Symbol();

        if(!Expression(arg)){
            if(ind != id.params.size())
                errTable.ReportError(ERROR_INVALID_ASSIGNMENT, lexer->GetFileName(), lexer->GetLineNumber(), "Too few arguments provided to \'" + id.id + "\'");
            return false;
        }

        // Check number of params
        if(ind >= id.params.size()){
            errTable.ReportError(ERROR_INVALID_ASSIGNMENT, lexer->GetFileName(), lexer->GetLineNumber(), "Too many arguments provided to \'" + id.id + "\'");
            return false;
        }
        // Check type match to param
        else if(!CompatibleTypeCheck(id.params[ind], arg)){
            return false;
        }

        argList.push_back(arg.llvmValue);
        ind++;

    } while (IsTokenType(T_COMMA));

    if(ind != id.params.size()){
        errTable.ReportError(ERROR_INVALID_ASSIGNMENT, lexer->GetFileName(), lexer->GetLineNumber(), "Too few arguments provided to \'" + id.id + "\'");
        return false;
    }
    return true;
}

bool Parser::Number(Symbol &num){
    DebugParseTrace("Number");

    if(tok.tt == T_INTEGER_CONST){
        num.type = TYPE_INT;
        num.tt = T_INTEGER_CONST;
        // Code Gen: Integer
        num.llvmValue = llvm::ConstantInt::get(*llvmContext, llvm::APInt(32, tok.val.intVal, true));

        return IsTokenType(T_INTEGER_CONST);
    }
    else if(tok.tt == T_FLOAT_CONST){
        num.type = TYPE_FLOAT;
        num.tt = T_FLOAT_CONST;
        // Code Gen: Float
        num.llvmValue = llvm::ConstantFP::get(*llvmContext, llvm::APFloat(tok.val.floatVal));

        return IsTokenType(T_FLOAT_CONST);
    }
    else
        return false;
}

bool Parser::String(Symbol &str){
    DebugParseTrace("String");

    if(tok.tt == T_STRING_CONST){
        str.id = tok.val.stringVal;
        str.tt = tok.tt;
        str.type = TYPE_STRING;
        // Code Gen: String
        str.llvmValue = llvmBuilder->CreateGlobalStringPtr(tok.val.stringVal);
    }

    return IsTokenType(T_STRING_CONST);
}

bool Parser::DeclarationBlock(){
    while(Declaration()){
        if(!IsTokenType(T_SEMICOLON)){
            errTable.ReportError(ERROR_MISSING_SEMICOLON, lexer->GetFileName(), lexer->GetLineNumber(), "Missing \';\' after declaration");
            return false;
        }
    }
    return true;
}

bool Parser::StatementBlock(){
    while(Statement()){
        if(!IsTokenType(T_SEMICOLON)){
            errTable.ReportError(ERROR_MISSING_SEMICOLON, lexer->GetFileName(), lexer->GetLineNumber(), "Missing \';\' after statement");
            return false;
        }
    }
    return true;
}


bool Parser::ProcedureCallOrName(Symbol &id){
    DebugParseTrace("Procedure Call or Name");
    if (!Identifier(id))
        return false;

    if(!scoper->HasSymbol(id.id)){
        errTable.ReportError(ERROR_SCOPE_DECLERATION, lexer->GetFileName(), lexer->GetLineNumber(), "Procedure Call: \'" + id.id + "\'");
        return false;
    }

    id = scoper->GetSymbol(id.id);

    if(IsTokenType(T_LPAREN)){
        DebugParseTrace("Procedure Call");

        //Confirm id is a procedure
        if(id.st != ST_PROCEDURE){
            errTable.ReportError(ERROR_INVALID_PROCEDURE, lexer->GetFileName(), lexer->GetLineNumber(), "\'" + id.id + "\' is not a procedure and cannot be called");
            return false;
        }
        std::vector<llvm::Value*> argList;
        ArgumentList(id, argList);

        if(listError)
            return false; 

        if (!IsTokenType(T_RPAREN)){
            errTable.ReportError(ERROR_MISSING_PAREN, lexer->GetFileName(), lexer->GetLineNumber(), "Missing \')\' in procedure call");
            return false;
        }

        // Code gen: Procedure call
        id.llvmValue = llvmBuilder->CreateCall(id.llvmFunction, argList);

        return true;  
    }
    else{
        DebugParseTrace("Name");

        //Confirm id is a name/variable
        if(id.st != ST_VARIABLE){
            errTable.ReportError(ERROR_INVALID_VARIABLE, lexer->GetFileName(), lexer->GetLineNumber(), "\'" + id.id + "\' is not a variable");
            return false;
        }

        // Check array access
        if(!ArrayIndexAssist(id))
            return false;

        // Code gen: Name
        id.llvmValue = llvmBuilder->CreateLoad(GetLLVMType(id.type), id.llvmAddress);
    }
    return true;
}

// Handle array index access
bool Parser::ArrayIndexAssist(Symbol &id){
    DebugParseTrace("Index");
    if (IsTokenType(T_LBRACKET)){
        Symbol exp;
        if(!Expression(exp))
            return false;
        
        if(!id.isArr){
            errTable.ReportError(ERROR_INVALID_ARRAY_INDEX, lexer->GetFileName(), lexer->GetLineNumber(), "/'" +  id.id + "/' is not an array");
            return false;
        }
        else if(exp.type != TYPE_INT){
            errTable.ReportError(ERROR_INVALID_ARRAY_INDEX, lexer->GetFileName(), lexer->GetLineNumber(), "Array index must be an integer");
            return false;
        }

        // check exp value < id.arrSize

        // Array is indexed
        id.isIndexed = true;

        if(!IsTokenType(T_RBRACKET)){
            errTable.ReportError(ERROR_MISSING_BRACKET, lexer->GetFileName(), lexer->GetLineNumber(), "Missing \']\' in name");
            return false;
        }
    }
    return true;
}

bool Parser::ArithmeticTypeCheck(Symbol &lhs, Symbol &rhs, Token &op){
    if((lhs.type != TYPE_INT && lhs.type != TYPE_FLOAT) || (rhs.type != TYPE_INT && rhs.type != TYPE_FLOAT)){
        errTable.ReportError(ERROR_INVALID_TYPE_CHECK, lexer->GetFileName(), lexer->GetLineNumber(), "Arithmetic only defined for int and float");
        return false;
    }

    if(lhs.type == TYPE_INT){
        if(rhs.type == TYPE_FLOAT){
            // convert lhs to float 
            lhs.type = TYPE_FLOAT;
            lhs.llvmValue = llvmBuilder->CreateSIToFP(lhs.llvmValue, llvmBuilder->getFloatTy());
        }
        // else both are ints
    }
    else{
        if(rhs.type == TYPE_INT){
            // convert rhs to float 
            rhs.type = TYPE_FLOAT;
            rhs.llvmValue = llvmBuilder->CreateSIToFP(rhs.llvmValue, llvmBuilder->getFloatTy());
        }
        // else both are floats
    }
    
    // Code gen: Arithmetic
    switch(op.tt){
        case (T_PLUS):
            if(lhs.type == TYPE_INT)
                lhs.llvmValue = llvmBuilder->CreateAdd(lhs.llvmValue, rhs.llvmValue);
            else
                lhs.llvmValue = llvmBuilder->CreateFAdd(lhs.llvmValue, rhs.llvmValue);
            break;
        case (T_MINUS):
            if(lhs.type == TYPE_INT)
                lhs.llvmValue = llvmBuilder->CreateSub(lhs.llvmValue, rhs.llvmValue);
            else
                lhs.llvmValue = llvmBuilder->CreateFSub(lhs.llvmValue, rhs.llvmValue);
            break;
        case (T_MULTIPLY):
            if(lhs.type == TYPE_INT)
                lhs.llvmValue = llvmBuilder->CreateMul(lhs.llvmValue, rhs.llvmValue);
            else
                lhs.llvmValue = llvmBuilder->CreateFMul(lhs.llvmValue, rhs.llvmValue);
            break;
        case (T_DIVIDE):
            if(lhs.type == TYPE_INT)
                lhs.llvmValue = llvmBuilder->CreateSDiv(lhs.llvmValue, rhs.llvmValue);
            else
                lhs.llvmValue = llvmBuilder->CreateFDiv(lhs.llvmValue, rhs.llvmValue);
            break;
        default:
            errTable.ReportError(ERROR_INVALID_TYPE_CHECK, lexer->GetFileName(), lexer->GetLineNumber(), "Invalid arithmetic operation");
            return false;
    }
    return true;
}

bool Parser::RelationTypeCheck(Symbol &lhs, Symbol &rhs, Token &op){
    // if int is present with float or bool, convert int to respective type

    bool comp = false;

    if(lhs.type == TYPE_INT) {
        if(rhs.type == TYPE_BOOL) {
            comp = true;
            lhs.type = TYPE_BOOL;
            // Convert lhs to bool, all non zeros are true
            lhs.llvmValue = llvmBuilder->CreateICmpNE(lhs.llvmValue, llvm::ConstantInt::get(*llvmContext, llvm::APInt(32, 0, true)));
        }
        else if(rhs.type == TYPE_FLOAT) {
            comp = true;
            lhs.type = TYPE_FLOAT;
            // Convert lhs to float
            lhs.llvmValue = llvmBuilder->CreateSIToFP(lhs.llvmValue, llvmBuilder->getFloatTy());
        }
        else if (rhs.type == TYPE_INT)
            comp = true;
    }
    else if(lhs.type == TYPE_FLOAT) {
        if(rhs.type == TYPE_FLOAT) 
            comp = true;
        else if(rhs.type == TYPE_INT) {
            comp = true;
            rhs.type = TYPE_FLOAT;
            // Convert rhs to float
            rhs.llvmValue = llvmBuilder->CreateSIToFP(rhs.llvmValue, llvmBuilder->getFloatTy());
        }
    }
    else if(lhs.type == TYPE_BOOL) {
        if(rhs.type == TYPE_BOOL)
            comp = true;
        else if(rhs.type == TYPE_INT) {
            comp = true;
            rhs.type = TYPE_BOOL;
            // Convert rhs to bool, all non zeros are true
            rhs.llvmValue = llvmBuilder->CreateICmpNE(rhs.llvmValue, llvm::ConstantInt::get(*llvmContext, llvm::APInt(32, 0, true)));
        }
    }
    else if(lhs.type == TYPE_STRING) {
        if(rhs.type == TYPE_STRING && (op.tt == T_EQUAL || op.tt == T_NOT_EQUAL))
            comp = true;
    }

    if(!comp)
        errTable.ReportError(ERROR_INVALID_RELATION, lexer->GetFileName(), lexer->GetLineNumber());
        
    // Code gen: Relation
    switch(op.tt){
        case (T_LESS):
            if(lhs.type == TYPE_INT)
                lhs.llvmValue = llvmBuilder->CreateICmpSLT(lhs.llvmValue, rhs.llvmValue);
            else if(lhs.type == TYPE_BOOL)
                lhs.llvmValue = llvmBuilder->CreateICmpULT(lhs.llvmValue, rhs.llvmValue);
            else   // float
                lhs.llvmValue = llvmBuilder->CreateFCmpOLT(lhs.llvmValue, rhs.llvmValue);
            break;
        case (T_LESS_EQ):
            if(lhs.type == TYPE_INT)
                lhs.llvmValue = llvmBuilder->CreateICmpSLE(lhs.llvmValue, rhs.llvmValue);
            else if(lhs.type == TYPE_BOOL)
                lhs.llvmValue = llvmBuilder->CreateICmpULE(lhs.llvmValue, rhs.llvmValue);
            else   // float
                lhs.llvmValue = llvmBuilder->CreateFCmpOLE(lhs.llvmValue, rhs.llvmValue);
            break;
        case (T_GREATER):
            if(lhs.type == TYPE_INT)
                lhs.llvmValue = llvmBuilder->CreateICmpSGT(lhs.llvmValue, rhs.llvmValue);
            else if(lhs.type == TYPE_BOOL)
                lhs.llvmValue = llvmBuilder->CreateICmpUGT(lhs.llvmValue, rhs.llvmValue);
            else   // float
                lhs.llvmValue = llvmBuilder->CreateFCmpOGT(lhs.llvmValue, rhs.llvmValue);
            break;
        case (T_GREATER_EQ):
            if(lhs.type == TYPE_INT)
                lhs.llvmValue = llvmBuilder->CreateICmpSGE(lhs.llvmValue, rhs.llvmValue);
            else if(lhs.type == TYPE_BOOL)
                lhs.llvmValue = llvmBuilder->CreateICmpUGE(lhs.llvmValue, rhs.llvmValue);
            else   // float
                lhs.llvmValue = llvmBuilder->CreateFCmpOGE(lhs.llvmValue, rhs.llvmValue);
            break;
        case (T_EQUAL):
            if(lhs.type == TYPE_INT || lhs.type == TYPE_BOOL)
                lhs.llvmValue = llvmBuilder->CreateICmpEQ(lhs.llvmValue, rhs.llvmValue);
            else if(lhs.type == TYPE_STRING){}
                //lhs.llvmValue = StringCompare(lhs, rhs);
            else   // float
                lhs.llvmValue = llvmBuilder->CreateFCmpOEQ(lhs.llvmValue, rhs.llvmValue);
            break;
        case (T_NOT_EQUAL):
            if(lhs.type == TYPE_INT || lhs.type == TYPE_BOOL)
                lhs.llvmValue = llvmBuilder->CreateICmpNE(lhs.llvmValue, rhs.llvmValue);
            else if(lhs.type == TYPE_STRING){}
                //lhs.llvmValue = llvmBuilder->CreateNot(StringCompare(lhs, rhs));
            else   // float
                lhs.llvmValue = llvmBuilder->CreateFCmpONE(lhs.llvmValue, rhs.llvmValue);
            break;
        default:
            errTable.ReportError(ERROR_INVALID_RELATION, lexer->GetFileName(), lexer->GetLineNumber());
            return false;
    }
    
    return comp;
}

bool Parser::ExpressionTypeCheck(Symbol &lhs, Symbol &rhs, Token &op){
    bool comp = false;

    if(lhs.type == TYPE_BOOL && rhs.type == TYPE_BOOL)
        comp = true;
    else if(lhs.type == TYPE_INT && rhs.type == TYPE_INT)
        comp = true;
    
    if(!comp)
        errTable.ReportError(ERROR_INVALID_EXPRESSION, lexer->GetFileName(), lexer->GetLineNumber(), "Expression operations only defined for int and bool");

    switch(op.tt){
        case T_AND:
            lhs.llvmValue = llvmBuilder->CreateAnd(lhs.llvmValue, rhs.llvmValue);
            break;
        case T_OR:
            lhs.llvmValue = llvmBuilder->CreateOr(lhs.llvmValue, rhs.llvmValue);
            break;
        default:
            errTable.ReportError(ERROR_INVALID_EXPRESSION, lexer->GetFileName(), lexer->GetLineNumber());
            return false;
    }
    
    return comp;
}

bool Parser::CompatibleTypeCheck(Symbol &dest, Symbol &exp){
    bool comp = false;

    // int == bool
    // int == float 
    if(dest.type == exp.type)
        comp = true;
    else if(dest.type == TYPE_INT){
        if(exp.type == TYPE_BOOL){
            comp = true;
            exp.type = TYPE_INT;
            exp.llvmValue = llvmBuilder->CreateIntCast(exp.llvmValue, llvmBuilder->getInt32Ty(), false);
        }
        else if(exp.type == TYPE_FLOAT){
            comp = true;
            exp.type = TYPE_INT;
            exp.llvmValue = llvmBuilder->CreateFPToSI(exp.llvmValue, llvmBuilder->getInt32Ty());
        }
    }
    else if(dest.type == TYPE_FLOAT){
        if(exp.type == TYPE_INT){
            comp = true;
            exp.type = TYPE_FLOAT;
            exp.llvmValue = llvmBuilder->CreateSIToFP(exp.llvmValue, llvmBuilder->getFloatTy());
        }
    }
    else if(dest.type == TYPE_BOOL){
        if(exp.type == TYPE_INT){
            comp = true;
            exp.type = TYPE_BOOL;
            exp.llvmValue = llvmBuilder->CreateICmpNE(exp.llvmValue, llvm::ConstantInt::get(*llvmContext, llvm::APInt(32, 0, true)));
        }
    }

    if(!comp)
        errTable.ReportError(ERROR_INVALID_ASSIGNMENT, lexer->GetFileName(), lexer->GetLineNumber(), "Incompatible types for " + GetTypeName(dest.type) + " and " + GetTypeName(exp.type));
    
    /*
    Check for valid matching with isArr and isIndexed
    - var = var
    - arr = arr
    - arr[i] = arr[i]
    - arr[i] = var
    - var = arr[i]    
    */
    
    if(dest.isArr || exp.isArr){
        // Both arrays
        if(dest.isArr && exp.isArr){
            // Array indexes must match
            if(dest.isIndexed != exp.isIndexed){
                errTable.ReportError(ERROR_INVALID_ARRAY_INDEX, lexer->GetFileName(), lexer->GetLineNumber(), "Incompatible matching index arrays");
                comp = false;
            }
            else if(!dest.isIndexed){
                if(dest.arrSize != exp.arrSize){
                    errTable.ReportError(ERROR_INVALID_ARRAY_INDEX, lexer->GetFileName(), lexer->GetLineNumber(), "Array lengths must match");
                    comp = false;
                }
            }
        }
        else{
            // One side is array or array must be indexed
            if((dest.isArr && !dest.isIndexed) || (exp.isArr && !exp.isIndexed)){
                errTable.ReportError(ERROR_INVALID_ARRAY_INDEX, lexer->GetFileName(), lexer->GetLineNumber(), "Array is not indexed");
                comp = false;
            }
        } 
    }
    // Both are not arrays
    return comp;
}

llvm::Type* Parser::GetLLVMType(Type t){
    switch(t){
        case TYPE_INT:
            return llvmBuilder->getInt32Ty();
        case TYPE_FLOAT:
            return llvmBuilder->getFloatTy();
        case TYPE_BOOL:
            return llvmBuilder->getInt1Ty();
        case TYPE_STRING:
            return llvmBuilder->getInt8PtrTy();
        default:
            errTable.ReportError(ERROR_INVALID_TYPE, lexer->GetFileName(), lexer->GetLineNumber());
            return nullptr;
    }
}

/*
llvm::Value* Parser::StringCompare(Symbol& lhs, Symbol& rhs){
    llvm::Function *func = scoper->GetCurrentProcedure().llvmFunction;
    llvm::BasicBlock *strCompEntry = llvm::BasicBlock::Create(*llvmContext, "StrComp", func);
    llvm::BasicBlock *strCompEntryMerge = llvm::BasicBlock::Create(*llvmContext, "StrCompMerge", func);

    // Set initial value index to 0
    llvm::Value* indAddr = llvmBuilder->CreateAlloca(llvmBuilder->getInt32Ty(), nullptr, "strCompInd");
    llvm::Value* ind = llvm::ConstantInt::get(*llvmContext, llvm::APInt(32, 0, true));
    llvmBuilder->CreateStore(ind, indAddr);

    llvmBuilder->CreateBr(strCompEntry);
    llvmBuilder->SetInsertPoint(strCompEntry);

    ind = llvmBuilder->CreateLoad(llvmBuilder->getInt32Ty(), indAddr);
    //llvm::Value* arr[2] = {llvm::ConstantInt::get(*llvmContext, llvm::APInt(32, 0, true)), ind};

    // Get pointer to string, then load character
    llvm::Value* lhsAddr = llvmBuilder->CreateInBoundsGEP(llvmBuilder->getInt32Ty(), lhs.llvmValue, ind);
    llvm::Value* rhsAddr = llvmBuilder->CreateInBoundsGEP(llvmBuilder->getInt32Ty(), rhs.llvmValue, ind);

    llvm::Value* lhsVal = llvmBuilder->CreateLoad(llvmBuilder->getInt8Ty(), lhsAddr);
    llvm::Value* rhsVal = llvmBuilder->CreateLoad(llvmBuilder->getInt8Ty(), rhsAddr);

    llvm::Value* comp = llvmBuilder->CreateICmpEQ(lhsVal, rhsVal);

    // Create terminating /0 value
    llvm::Value* termVal = llvm::ConstantInt::get(*llvmContext, llvm::APInt(8, 0, true));

    // Check for /0 terminator in 1 string, if not equal then we can return
    llvm::Value* notEndTerm = llvmBuilder->CreateICmpNE(lhsVal, termVal);

    // Increment ind
    llvm::Value *i = llvm::ConstantInt::get(*llvmContext, llvm::APInt(32, 1, true));
    ind = llvmBuilder->CreateAdd(ind, i);
    llvmBuilder->CreateStore(ind, indAddr);

    // Continue checking if not end and if lhs == rhs
    llvm::Value* cond = llvmBuilder->CreateAnd(comp, notEndTerm);
    llvmBuilder->CreateCondBr(cond, strCompEntry, strCompEntryMerge);
    llvmBuilder->SetInsertPoint(strCompEntryMerge);

    return comp;
}
*/