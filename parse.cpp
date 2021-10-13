#include <iostream>
#include "lex.h"
#include "parse.h"

using namespace std;
static int error_count = 0;
map<string, bool> defVar;
map<string, Token> SymTable;

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


bool Prog(istream& in, int& line){

	//cout << "In Prog()" << endl;

	bool status = false;

	LexItem tok = Parser::GetNextToken(in, line);

	if(tok.GetToken() != PROGRAM){
		ParseError(line, "Missing PROGRAM keyword");
		return false;
	}
	
	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() != IDENT){
		ParseError(line, "Missing Program Name");
		return false;
	}
	do{
		status = false;
		tok = Parser::GetNextToken(in, line);
		if(tok.GetToken() == INTEGER || tok.GetToken() == REAL || tok.GetToken() == CHAR){
			Parser::PushBackToken(tok);
			status = Decl(in, line);
			if(status == false){ //parsing of Decl failed
				return false;
			} 	
		}
		if(tok.GetToken() == IDENT || tok.GetToken() == IF || tok.GetToken() == PRINT || tok.GetToken() == READ){
			Parser::PushBackToken(tok);
			status = Stmt(in, line);
			if(status == false){ //parsing of Stmt failed
				return false;
			} 	
		}
		if(status == false && tok.GetToken() != END){ //Happens if token is not a decl or stmt
			//cout << "This one end" << endl;
			ParseError(line, "Missing END keyword");
			return false;
		}
	}while(status == true);

	//tok = Parser::GetNextToken(in, line); //Here previous token was a successful decl or stmt
	if(tok.GetToken() != END){
		//cout << "The other end" << endl;
		ParseError(line, "Missing END KeyWord");
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() != PROGRAM){
		ParseError(line, "Missing PROGRAM at the End");
		return false;
	}
	
	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() != IDENT){
		ParseError(line, "Missing Program Name");
		return false;
	}
	return true;
}

bool Decl(istream& in, int& line){
	
	//cout << "In Decl()" << endl;
	
	bool status = false;
	LexItem tok;
	LexItem t = Parser::GetNextToken(in, line);
	if(t.GetToken() == INTEGER || t.GetToken() == REAL || t.GetToken() == CHAR){
		tok = t;
		tok = Parser::GetNextToken(in, line);
		if(tok.GetToken() ==  COLON){
			status = IdList(in, line, t);
			if(status == true){
				status = Decl(in, line);
				return status;
			}
		}
		else{
			ParseError(line, "Incorrect Declaration in Program");
			return false;
		}
	}
	Parser::PushBackToken(t);
	return true;
}

bool IdList(istream& in, int& line, LexItem tok){
	
	//cout << "In IdList()" << endl;
	
	bool status = false;

	LexItem t = tok;
	tok = Parser::GetNextToken(in, line);

	if(tok.GetToken() != IDENT){
		ParseError(line, "Missing Variable");
		return false;
	}
	auto sym = SymTable.find(tok.GetLexeme());
	if(sym != SymTable.end()){ 
		ParseError(line, "Variable Redefinition");
		return false;
	}
	SymTable[tok.GetLexeme()] = t.GetToken();
	defVar[tok.GetLexeme()] = false;

	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == COMA){
		status = IdList(in, line, t);
		return status;
	}
	Parser::PushBackToken(tok);
	return true;
}

bool Stmt(istream& in, int& line){
	
	//cout << "In Stmt()" << endl;

	bool status;

	LexItem tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern\n(" + tok.GetLexeme()+ ")");
		return false;
	}
	switch(tok.GetToken()){
		case IDENT:
			Parser::PushBackToken(tok);
			status = Var(in, line);
			if(status == false){
				return false;
			}
			status = AssignStmt(in, line);
			if(status == true){
				defVar[tok.GetLexeme()] = true;
				status = Stmt(in, line);
			}
			break;
		case IF:
			status = IfStmt(in, line);
			if(status == true){
				status = Stmt(in, line);
			}
			break;
		case PRINT:
			status = PrintStmt(in, line);
			if(status == true){
				status = Stmt(in, line);
			}
			break;
		case READ:
			status = ReadStmt(in, line);
			if(status == true){
				status = Stmt(in, line);
			}
			break;
		default:
			Parser::PushBackToken(tok);
			return true;
	}
	return status;
}

bool PrintStmt(istream& in, int& line){

	//cout << "In PrintStmt()" << endl;

	bool status = false;

	LexItem tok = Parser::GetNextToken(in, line);

	if(tok.GetToken() != COMA){
		ParseError(line, "Missing a Comma");
		return false;
	}

	status = ExprList(in, line);
	if(status == false){
		ParseError(line, "Missing expression after PRINT");
		return false;
	}
	return status;
}

bool IfStmt(istream& in, int& line){

	//cout << "In IfStmt()" << endl;

	bool status = false;

	LexItem tok = Parser::GetNextToken(in, line);

	if(tok.GetToken() != LPAREN){
		ParseError(line, "Missing Left Parenthesis after IF");
		return false;
	}

	status = LogicExpr(in, line);	

	tok = Parser::GetNextToken(in, line);	
	if(tok.GetToken() != RPAREN){
		ParseError(line, "Missing Right Parenthesis");
		return false;
	}
	if(status == false){
		ParseError(line, "Missing or incorrect Logic Expression");
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() != THEN){
		ParseError(line, "Missing THEN in IF statement");
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() != END){
		Parser::PushBackToken(tok);
		status = Stmt(in, line);
		if(status == false){
			ParseError(line, "Incorrect statement inside IF block");
			return false;
		}
	}

	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() != END){
		ParseError(line, "Missing END keyword");
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() != IF){
		ParseError(line, "Missing IF at End of IF statement");
		return false;
	}
	return true;
}

bool ReadStmt(istream& in, int& line){
	
	//cout << "In ReadStmt()" << endl;
	
	bool status = false;

	LexItem tok = Parser::GetNextToken(in, line);

	if(tok.GetToken() != COMA){
		ParseError(line, "Missing coma after READ");
		return false;
	}
	status = VarList(in, line);
	return status;
}


bool VarList(istream& in, int& line){

	//cout << "In VarList()" << endl;

	bool status = false;
	status = Var(in, line);
	if(status == false){
		return false;
	}

	LexItem tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == COMA){
		status = VarList(in, line);
		return status;
	}
	Parser::PushBackToken(tok);
	return true;
}

bool Var(istream& in, int& line){

	//cout << "In Var()" << endl;

	LexItem tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() != IDENT){
		ParseError(line, "Missing Variable");
		return false;
	}

	//auto defv = defVar.find(tok.GetLexeme());
	//if(defv->second == false || defv == defVar.end()){
	//	ParseError(line, "Undefined Variable");
	//	return false;
	//}

	auto sym = SymTable.find(tok.GetLexeme());
	if(sym == SymTable.end()){
		ParseError(line, "Undeclared Variable");
		return false;
	}

	return true;
}

bool AssignStmt(istream& in, int& line){

	//cout << "In AssignStmt()" << endl;
	
	LexItem tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() != ASSOP){
		ParseError(line, "Missing Assignment Operator =");
		return false;
	}
	
	bool status = Expr(in, line);

	return status;
}

bool ExprList(istream& in, int& line){

	//cout << "In ExprList()" << endl;

	bool status = Expr(in, line);
	if(status == false){
		return false;
	}

	LexItem tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == COMA){
		status = ExprList(in, line);
		return status;
	}

	Parser::PushBackToken(tok);
	return true;
}

bool LogicExpr(istream& in, int& line){
	
	//cout << "In LogicExpr()" << endl;
	
	bool status = Expr(in, line);
	if(status == false){
		return false;
	}
	
	LexItem tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() != EQUAL && tok.GetToken() != LTHAN){
		return false;
	}

	status = Expr(in, line);
	return status;
}

bool Expr(istream& in, int& line){
	
	//cout << "In Expr()" << endl;

	bool status = Term(in, line);
	if(status == false){
		ParseError(line, "Missing Expression");
		return false;
	}
	
	LexItem tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == PLUS || tok.GetToken() == MINUS){
		status = Expr(in, line);
		return status;
	}

	Parser::PushBackToken(tok);
	return true;
}

bool Term(istream& in, int& line){

	//cout << "In Term()" << endl;

	bool status = SFactor(in, line);
	if(status == false){
		return false;
	}

	LexItem tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == MULT || tok.GetToken() == DIV){
		status = Term(in, line);
		return status;
	}

	Parser::PushBackToken(tok);
	return true;
}
bool SFactor(istream& in, int& line){
	
	//cout << "In SFactor()" << endl;
	
	bool status = false;

	LexItem tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == PLUS || tok.GetToken() == MINUS){
		status = Factor(in, line, tok.GetToken());
		return status;
	}
	Parser::PushBackToken(tok);
	status = Factor(in, line, PLUS); //Default sign is plus;
	return status;
}

bool Factor(istream& in, int& line, int sign){
	
	//cout << "In Factor(): "; 

	LexItem tok = Parser::GetNextToken(in, line);
	
	bool status = false;

	if(tok.GetToken() == IDENT){
		//cout << tok.GetLexeme() << endl;
		Parser::PushBackToken(tok);
		status = Var(in, line);
		return status;
	}
	if(tok.GetToken() == ICONST || tok.GetToken() == RCONST || tok.GetToken() == SCONST){
		//cout << tok.GetLexeme() << endl;
		return true;
	}
	if(tok.GetToken() == LPAREN){
		status = Expr(in, line);
		if(status == false){
			return false;
		}
		tok = Parser::GetNextToken(in, line);
		if(tok.GetToken() != RPAREN){
			return false;
		}
		return true;
	}

	return false;
}
