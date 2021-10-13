/* Implementation of Recursive-Descent Parser
 * parse.cpp
 * Programming Assignment 2
 * Spring 2021
*/

#include "parserInt.h"
#include "val.h"
#include "lex.h"

static int error_count = 0;
map<string, bool> defVar;
map<string, Token> SymTable;
map<string, Value> TempsResults;
queue<Value>* ValQue;

namespace Parser {
	bool pushed_back = false;
	LexItem	pushed_token;

	static LexItem GetNextToken(istream& in, int& line) {
		if( pushed_back ) {
			pushed_back = false;
			return pushed_token;
		}
		return getNextToken(in, line);
	}

	static void PushBackToken(LexItem & t) {
		if( pushed_back ) {
			abort();
		}
		pushed_back = true;
		pushed_token = t;	
	}

}

int ErrCount()
{
    return error_count;
}

void ParseError(int line, string msg)
{
	++error_count;
	cout << line << ": " << msg << endl;
    
}



//Program is: Prog = PROGRAM IDENT {Decl} {Stmt} END PROGRAM IDENT
bool Prog(istream& in, int& line)
{
	bool dl = false, sl = false;
	LexItem tok = Parser::GetNextToken(in, line);
	
	
	if (tok.GetToken() == PROGRAM) {
		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == IDENT) {
            string progname = tok.GetLexeme();
			dl = Decl(in, line);
			if( !dl  )
			{
				ParseError(line, "Incorrect Declaration in Program");
				return false;
			}
			sl = Stmt(in, line);
			if( !sl  )
			{
				ParseError(line, "Incorrect Statement in program");
				return false;
			}	
			tok = Parser::GetNextToken(in, line);
			
			if (tok.GetToken() == END) {
				tok = Parser::GetNextToken(in, line);
				
				if (tok.GetToken() == PROGRAM) {
					tok = Parser::GetNextToken(in, line);
					
					if (tok.GetToken() == IDENT && tok.GetLexeme() == progname) {
						return true;
					}
					else if(tok.GetToken() != IDENT)
					{
						ParseError(line, "Missing Program Name");
						return false;
					}
                    else{
                    
                        ParseError(line, "Incorrect Program Name");
						return false;
                    }
				}
				else
				{
					ParseError(line, "Missing PROGRAM at the End");
					return false;
				}	
			}
			else
			{
				ParseError(line, "Missing END of Program");
				return false;
			}	
		}
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	
	return false;
}

//Decl = Type : IdList 
//Type = INTEGER | REAL | CHAR
bool Decl(istream& in, int& line) {
	bool status = false;
	LexItem tok;
	
	LexItem t = Parser::GetNextToken(in, line);
	
	if(t.GetToken() == INTEGER || t.GetToken() == REAL || t.GetToken() == CHAR) {
		tok = t;
		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == COLON) {
			status = IdList(in, line, t);
			//cout<< tok.GetLexeme() << " " << (status? 1: 0) << endl;
			if (status)
			{
				status = Decl(in, line);
				return status;
			}
		}
		else{
			ParseError(line, "Missing Colon");
			return false;
		}
	}
		
	Parser::PushBackToken(t);
	return true;
}

//Stmt is either a PrintStmt, ReadStmt, IfStmt, or an AssigStmt
//Stmt = AssigStmt | IfStmt | PrintStmt | ReadStmt
bool Stmt(istream& in, int& line) {
	bool status;
	
	LexItem t = Parser::GetNextToken(in, line);
	
	switch( t.GetToken() ) {

	case PRINT:
		status = PrintStmt(in, line);
		
		if(status)
			status = Stmt(in, line);
		break;

	case IF:
		status = IfStmt(in, line);
		if(status)
			status = Stmt(in, line);
		break;

	case IDENT:
		Parser::PushBackToken(t);
        status = AssignStmt(in, line);
		if(status)
			status = Stmt(in, line);
		break;
		
	/*case READ:
		status = ReadStmt(in, line);
		
		if(status)
			status = Stmt(in, line);
		break;
*/
	default:
		Parser::PushBackToken(t);
		return true;
	}

	return status;
}

//PrintStmt:= print, ExpreList 
bool PrintStmt(istream& in, int& line) {
    
	LexItem t;
    
    ValQue = new queue<Value>;
	
	if( (t=Parser::GetNextToken(in, line)) != COMA ) {
		
		ParseError(line, "Missing a Comma");
		return false;
	}
	
	bool ex = ExprList(in, line);
	
	if( !ex ) {
        
		ParseError(line, "Missing expression after print");
        while(!(*ValQue).empty()){
            ValQue->pop();
        }
        delete ValQue;
		return false;
	}
	
	while(!(*ValQue).empty()){
        Value nextVal = ValQue->front();
        cout << nextVal;
        ValQue->pop();
    }
    cout << endl;
	return ex;
}

//IfStmt:= if (Expr) then {Stmt} END IF
bool IfStmt(istream& in, int& line) {
	bool ex=false ; 
	LexItem t;
	
	if( (t=Parser::GetNextToken(in, line)) != LPAREN ) {
		
		ParseError(line, "Missing Left Parenthesis");
		return false;
	}
    Value val;
	
	ex = LogicExpr(in, line, val);
	if( !ex ) {
		ParseError(line, "Missing if statement Logic Expression");
		return false;
	}

	if((t=Parser::GetNextToken(in, line)) != RPAREN ) {
		
		ParseError(line, "Missing Right Parenthesis");
		return false;
	}
	
	if((t=Parser::GetNextToken(in, line)) != THEN ) {
		
		ParseError(line, "Missing THEN");
		return false;
	}
    if(val.GetBool() == true){
        bool st = Stmt(in, line);
        if( !st ) {
            ParseError(line, "Missing statement for IF");
            return false;
        }
    }
    else{
        while(t.GetToken() != END){
            t=Parser::GetNextToken(in, line);
        }
        Parser::PushBackToken(t);
    }
	
	
	if((t=Parser::GetNextToken(in, line)) != END ) {
		
		ParseError(line, "Missing END of IF");
		return false;
	}
	if((t=Parser::GetNextToken(in, line)) != IF ) {
		
		ParseError(line, "Missing IF at End of IF statement");
		return false;
	}
	
	return true;
}

/*bool ReadStmt(istream& in, int& line)
{
	
	LexItem t;
	
	if( (t=Parser::GetNextToken(in, line)) != COMA ) {
		
		ParseError(line, "Missing a Comma");
		return false;
	}
	
	bool ex = VarList(in, line);
	
	if( !ex ) {
		ParseError(line, "Missing Variable after Read Statement");
		return false;
	}
	
	

	return ex;
}*/
//IdList:= IDENT {,IDENT}
bool IdList(istream& in, int& line, LexItem& type) {
	bool status = false;
	string identstr;
	
	LexItem tok = Parser::GetNextToken(in, line);
	if(tok == IDENT)
	{
		//set IDENT lexeme to the type tok value
		identstr = tok.GetLexeme();
		if (!(defVar.find(identstr)->second))
		{
			defVar[identstr] = false;
            //cout << tokenPrint[type.GetToken()];
			SymTable[identstr] = type.GetToken();
		}	
		else
		{
			ParseError(line, "Variable Redefinition");
			return false;
		}
		
	}
	else
	{
		ParseError(line, "Missing Variable");
		return false;
	}
	
	tok = Parser::GetNextToken(in, line);
	
	if (tok == COMA) {
		status = IdList(in, line, type);
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else{
		Parser::PushBackToken(tok);
		return true;
	}
	return status;
}

//VarList
/*bool VarList(istream& in, int& line)
{
	bool status = false;
	string identstr;
	
	status = Var(in, line);
	
	if(!status)
	{
		ParseError(line, "Missing Variable");
		return false;
	}
	
	LexItem tok = Parser::GetNextToken(in, line);
	
	if (tok == COMA) {
		status = VarList(in, line);
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else{
		Parser::PushBackToken(tok);
		return true;
	}
	return status;
}*/

//Var:= ident
bool Var(istream& in, int& line, LexItem& tok)
{
	//called only from the AssignStmt function
	string identstr;
	
	tok = Parser::GetNextToken(in, line);
	
	if (tok == IDENT){
		identstr = tok.GetLexeme();
		if (!(SymTable.find(identstr)->second))
		{
			ParseError(line, "Undeclared Variable");
			return false;
			
		}	
		return true;
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	return false;
}

//AssignStmt:= Var = Expr
bool AssignStmt(istream& in, int& line) {
	
	bool varstatus = false, status = false;
	LexItem t;
	
	varstatus = Var( in, line, t);
	
	if (varstatus){
        
        Value rightVal;
        
        string idstring = t.GetLexeme();
        Token leftType = SymTable[idstring];
        //cout << idstring << " is a " << tokenPrint[leftType] << endl;
        
		t = Parser::GetNextToken(in, line);
		
		if (t == ASSOP){
			status = Expr(in, line, rightVal);
			if(!status) {
				ParseError(line, "Missing Expression in Assignment Statment");
				return status;
			}
            //switch(leftType){
                if(leftType == INTEGER && rightVal.GetType() == VREAL){
                    Value temp = rightVal;
                    rightVal = new Value();
                    rightVal.SetType(VINT);
                    rightVal.SetInt((int)temp.GetReal());
                    //cout << rightVal << endl;
                    TempsResults[idstring] = rightVal;
                    
                    //break
                }
                else{
                    TempsResults[idstring] = rightVal;
                    //break;
                }
            defVar[idstring] = true;
            //}
		}
		else if(t.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << t.GetLexeme() << ")" << endl;
			return false;
		}
		else {
			ParseError(line, "Missing Assignment Operator =");
			return false;
		}
	}
	else {
		ParseError(line, "Missing Left-Hand Side Variable in Assignment statement");
		return false;
	}
	return status;	
}

//ExprList:= Expr {,Expr}
bool ExprList(istream& in, int& line) {
	bool status = false;
    
    Value val;

	status = Expr(in, line, val);
	if(!status){
		ParseError(line, "Missing Expression");
		return false;
	}
    
    ValQue->push(val);
	
	LexItem tok = Parser::GetNextToken(in, line);
	
	if (tok == COMA) {
		status = ExprList(in, line);
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else{
		Parser::PushBackToken(tok);
		return true;
	}
	return status;
}

//Expr:= Term {(+|-) Term}
bool Expr(istream& in, int& line, Value& retVal) {
	
    Value val1, val2;
	bool t1 = Term(in, line, val1);
	
    //cout << "in Expr()" << endl;
    LexItem tok;
	if( !t1 ) {
		return false;
	}
	retVal = val1;
	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
    
	while ( tok.GetToken() == PLUS || tok.GetToken() == MINUS ){
		t1 = Term(in, line, val2);
		if( !t1 ) 
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}
        		
        if(val1.GetType() == VCHAR || val2.GetType() == VCHAR){
            ParseError(line, "Run-Time Error-Illegal Mixed Type Operands");
            return false;
        }
        
        if(tok.GetToken() == PLUS){
            retVal = retVal + val2;
        }
        else if(tok.GetToken() == MINUS){
            retVal = retVal - val2;
        }
		
		tok = Parser::GetNextToken(in, line);
		if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}		
	}
	Parser::PushBackToken(tok);
	return true;
}

//Term:= SFactor {(*|/) SFactor}
bool Term(istream& in, int& line, Value& retVal) {
    
    Value val1, val2;
	
	bool t1 = SFactor(in, line, val1);
	LexItem tok;
	
	if( !t1 ) {
		return false;
	}
    retVal = val1;
	
	tok	= Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
	}
	while ( tok.GetToken() == MULT || tok.GetToken() == DIV  )
	{
		
        t1 = SFactor(in, line, val2);
		
		if( !t1 ) {
			ParseError(line, "Missing operand after operator");
			return false;
		}
        
        if(retVal.GetType() == VCHAR || val2.GetType() == VCHAR){
            ParseError(line, "Run-Time Error-Illegal Mixed Type Operands");
            return false;
        }
        if(tok.GetToken() == MULT){
            retVal = retVal * val2;
        }
        else if(tok.GetToken() == DIV){
            if(val2.IsInt()){
                int itemp = val2.GetInt();
                if(itemp == 0){
                    ParseError(line, "Run-Time Error-Illegal Division by Zero");
                    return false;
                }
                retVal = retVal / val2;
            }
            else if(val2.IsReal()){
                float rtemp = val2.GetReal();
                if(rtemp == 0.0){
                    ParseError(line, "Run-Time Error-Illegal Division by Zero");
                    return false;
                }
                retVal = retVal / val2;
            }           
        }
		
		tok	= Parser::GetNextToken(in, line);
		if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}
		
	}
	Parser::PushBackToken(tok);
	return true;
}

//SFactor = Sign Factor | Factor
bool SFactor(istream& in, int& line, Value& retVal)
{
	LexItem t = Parser::GetNextToken(in, line);
	bool status;
	int sign = 0;
	if(t == MINUS )
	{
		sign = -1;
	}
	else if(t == PLUS)
	{
		sign = 1;
	}
	else{
		Parser::PushBackToken(t);
        sign = 1;
    }
	status = Factor(in, line, sign, retVal);
	return status;
}
//LogicExpr = Expr (== | <) Expr
bool LogicExpr(istream& in, int& line, Value& retVal){
    
	Value val1, val2;
	bool t1 = Expr(in, line, val1);
	LexItem tok;
	
	if( !t1 ) {
		return false;
	}
	retVal = val1;
	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	if ( tok.GetToken() == LTHAN  || tok.GetToken() == EQUAL) 
	{
		t1 = Expr(in, line, val2);
		if( !t1 ) 
		{
			ParseError(line, "Missing expression after relational operator");
			return false;
		}
        if(retVal.GetType() != val2.GetType()){
            ParseError(line, "Run-Time Error-Illegal Mixed Type Operands");
			return false;
        }
        
        if(tok.GetToken() == LTHAN){
            retVal = val1 < val2;
        }
        else if(tok.GetToken() == EQUAL){
            retVal = val1 == val2;
        }
        
		return true;
	}
	Parser::PushBackToken(tok);
	return true;
}

//Factor := ident | iconst | rconst | sconst | (Expr)
bool Factor(istream& in, int& line, int sign, Value& retVal) {
	
	LexItem tok = Parser::GetNextToken(in, line);
	
    string lexeme = tok.GetLexeme();
	if( tok == IDENT ) {
		//check if the var is defined 
		if (!(defVar.find(lexeme)->second))
		{
			ParseError(line, "Undefined Variable");
			return false;	
		}
        retVal = TempsResults[lexeme];
        if(retVal.GetType() == VINT){
            retVal.SetInt(sign*retVal.GetInt());
        }
        else if(retVal.GetType() == VREAL){
            retVal.SetReal(sign*retVal.GetReal());
        }
        //cout << "IN FACTOR IDENT " << retVal << lexeme << endl;
		return true;
	}
	else if( tok == ICONST ) {
		retVal = new Value(sign*stoi(lexeme));
        retVal.SetType(VINT);
        retVal.SetInt(sign*stoi(lexeme));
        //cout << "IN FACTOR ICONST " << retVal << " - " << lexeme << endl;
		return true;
	}
	else if( tok == SCONST ) {
		retVal = new Value();
        retVal.SetType(VCHAR);
        retVal.SetChar(lexeme);
        //cout << "IN FACTOR SCONST " << retVal << " - " << lexeme << endl;
		return true;
	}
	else if( tok == RCONST ) {
		retVal = new Value();
        retVal.SetType(VREAL);
        retVal.SetReal(sign*stof(lexeme));
        //cout << "IN FACTOR RCONST " << retVal << " - "<< lexeme << endl;
		return true;
	}
	else if( tok == LPAREN ) {
		bool ex = Expr(in, line, retVal);
		if( !ex ) {
			ParseError(line, "Missing expression after (");
			return false;
		}
		if( Parser::GetNextToken(in, line) == RPAREN )
			return ex;

		ParseError(line, "Missing ) after expression");
		return false;
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}

	ParseError(line, "Unrecognized input");
	return 0;
}



