#include "lex.h"

using namespace std;

LexItem getNextToken(istream& in, int& linenumber){
	

	Token token = ERR;
	string lexeme = "";
	LexItem lexitem;
	
	string line = "";

	char c;
	while(true){
		in.get(c);
		if(in.eof()){
			token = DONE;
			break;
		}
		if(c == '!'){
			getline(in, line);
			linenumber++;
			continue;
		}
		if(c == '\n'){
			linenumber++;
			continue;
		}
		if(isspace(c)){
			continue;
		}
		if(isalpha(c)){
			token = IDENT;
			lexeme += toupper(c);
			in.get(c);
			while(isalnum(c)){
				lexeme += toupper(c);
				in.get(c);
			}
			in.unget();
			auto it = kwmap.find(lexeme);
			if(it != kwmap.end()){
				token = it->second;
			}
			break;
		}
		if(isdigit(c)){
			lexeme += c;
			in.get(c);
			while(isdigit(c)){
				lexeme += c;
				in.get(c);
			}
			if(c == '.'){
				char x = in.peek();
				if(!isdigit(x)){
					token = ERR;
					break;
				}
				lexeme +=  c;
				in.get(c);
				while(isdigit(c)){
					lexeme += c;
					in.get(c);
				}
				in.unget();
				token = RCONST;
				break;
			}
			else{
				in.unget();
				token = ICONST;
				break;
			}
		}
		if(c == '.'){	
			lexeme += c;
			in.get(c);
			if(!isdigit(c)){
				lexeme += c;
				token = ERR;
				break;
			}
			while(isdigit(c)){
				lexeme += c;
				in.get(c);
			}
			in.unget();
			token = RCONST;
			break;
		}
		if(c == '"' || c == '\''){
			lexeme += c;
			string str;
			char quoteType = c;
			while(true){
				in.get(c);
				if(in.eof() || c == '\n'){
					token = ERR;
					break;
				}
				lexeme += c;
				if(c == quoteType){
					token = SCONST;
					lexeme = str;
					break;
				}
				str += c;
			}
			break;
		}
		if(c == '+'){
			token = PLUS;
			lexeme += c;
			break;
		}
		if(c == '-'){
			token = MINUS;
			lexeme += c;
			break;
		}
		if(c == '*'){
			token = MULT;
			lexeme += c;
			break;
		}
		if(c == '/'){
			lexeme += c;
			in.get(c);
			if(c == '/'){
				lexeme += c;
				token = CONCAT;
			}
			else{
				in.unget();
				token = DIV;
			}
			break;
		}
		if(c == '='){
			lexeme += c;
			in.get(c);
			if(c == '='){
				lexeme += c;
				token = EQUAL;
			}
			else{
				in.unget();
				token = ASSOP;
			}
			break;
		}
		if(c == '('){
			token = LPAREN;
			lexeme += c;
			break;
		}
		if(c == ')'){
			token = RPAREN;
			lexeme += c;
			break;
		}
		if(c == '<'){
			token = LTHAN;
			lexeme += c;
			break;
		}
		if(c == ','){
			token = COMA;
			lexeme += c;
			break;
		}
		if(c == ':'){
			token = COLON;
			lexeme += c;
			break;
		}
		if(token == ERR){
			lexeme += c;
			break;
		}
	}
    if(token == ERR){
        linenumber++;
    }

	lexitem = LexItem(token, lexeme, linenumber); 
	return lexitem;
}
