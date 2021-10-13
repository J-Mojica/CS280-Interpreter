/*
 * parse.h
 */

#ifndef PARSE_H_
#define PARSE_H_

#include <iostream>

#include "lex.h"

extern bool Prog(istream& in, int& line);
extern bool Stmt(istream& in, int& line);
extern bool Decl(istream& in, int& line);
extern bool PrintStmt(istream& in, int& line);
extern bool IfStmt(istream& in, int& line);
extern bool ReadStmt(istream& in, int& line);
extern bool IdList(istream& in, int& line, LexItem tok);
extern bool VarList(istream& in, int& line);
extern bool Var(istream& in, int& line);
extern bool AssignStmt(istream& in, int& line);
extern bool ExprList(istream& in, int& line);
extern bool LogicExpr(istream& in, int& line);
extern bool Expr(istream& in, int& line);
extern bool Term(istream& in, int& line);
extern bool SFactor(istream& in, int& line);
extern bool Factor(istream& in, int& line, int sign);
extern int ErrCount();

#endif /* PARSE_H_ */
