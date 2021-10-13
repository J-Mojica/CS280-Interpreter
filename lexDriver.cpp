#include <fstream>
#include <stdlib.h>
#include <sstream>
#include "lex.h"

using namespace std;

int main(int argc, char* argv[]){
	
	map<string, bool> flags = {
		{"-v", false},
		{"-iconsts", false},
		{"-rconsts", false},
		{"-sconsts", false},
		{"-ids", false}
	};
	string filename = "";
	string argf;
	bool readFilename = false;

	for(int i = 1; i < argc; i++){
		if(argv[i][0] == '-'){
			argf = argv[i];
			auto it = flags.find(argf);
			if (it != flags.end()){
				it->second = true;
			}
			else{
				cerr << "UNRECOGNIZED FLAG " << argv[i] << endl;
				exit(1);
			}
		}
		else if(!readFilename){
			filename = argv[i];
			readFilename = true;
		}
		else{
			cerr << "ONLY ONE FILE NAME ALLOWED" << endl;
			exit(1);
		}
	}
	if(!readFilename){
		cerr << "No Specified Input File Name." << endl;
		exit(1);
	}
	
	ifstream in;
	
	in.open(filename);
	
	if(!in.good()){
		cerr << "CANNOT OPEN THE FILE " << filename << endl;
		exit(1);
	}

	map<string, int> idents;
	map<string, int> sconsts;
	map<int, int> iconsts;
	map<double, int> rconsts;
	
	LexItem currLexItem;
	Token currToken;
	string currLexeme;

	int lines = 0, tokens = 0;
	while(true){
		currLexItem = getNextToken(in, lines);
		currToken = currLexItem.GetToken();
		currLexeme = currLexItem.GetLexeme();
		if(currToken == ERR){
			cerr << "Error in line " << lines << " (" << currLexeme << ")" << endl;
			exit(1);	
		}
		if(currToken == DONE){
			cout << "Lines: " << lines << endl;
			if(lines == 0){
				return 0;
			}
			cout << "Tokens: " << tokens << endl;
			break;
		}
		tokens++;
		switch(currToken){
			case IDENT:{
				idents[currLexeme]++;
				break;
			}
			case SCONST:{
				sconsts[currLexeme]++;
				break;
			}
			case ICONST:{
				int i = stoi(currLexeme);
				iconsts[i]++;
				break;
			}
			case RCONST:{
				double r = atof(currLexeme.c_str());
				rconsts[r]++;
				break;
			}
			default:{
				break;		
			}
		}
		if(flags["-v"]){
			auto it = tokenPrint.find(currToken);
			string tokenString = it->second;
			switch(currToken){
				case IDENT:
				case ICONST:
				case RCONST:
				case SCONST:{
					cout << tokenString << "(" << currLexeme << ")"<< endl; 
					break;
				}
				default:{
					cout << tokenString << endl;
				}
			}
		}
	}
	ostringstream flagOut;
	if(flags["-sconsts"] && sconsts.size() > 0){
		flagOut << "STRINGS:" << endl; 
		for(auto it = sconsts.begin(); it != sconsts.end(); it++){
				flagOut << it->first << endl;
		}	
	}
	if(flags["-iconsts"] && iconsts.size() > 0){
		flagOut << "INTEGERS:" << endl; 
		for(auto it = iconsts.begin(); it != iconsts.end(); it++){
				flagOut << it->first << endl;
		}	
	}
	if(flags["-rconsts"] && rconsts.size() > 0){	
		flagOut << "REALS:" << endl; 
		for(auto it = rconsts.begin(); it != rconsts.end(); it++){
				flagOut << it->first << endl;
		}	
	}
	if(flags["-ids"] && idents.size() > 0){
		flagOut << "IDENTIFIERS:" << endl; 
		for(auto it = idents.begin(); it != idents.end(); it++){
			it++;
			if(it != idents.end()){
				it--;
				flagOut << it->first << ", ";
			}
			else{
				it--;
				flagOut << it->first << endl;
			}
		}
	}
	cout << flagOut.str();
	in.close();
	return 0;
}
