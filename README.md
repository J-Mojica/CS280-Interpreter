# CS280-Interpreter
__Implementation of an interpreter for a Fortran-like programming language__

The interpreter consists of two programs: the [Lexical Analyzer](/lex.cpp) and the [Parser](/parseInt.cpp).

## Lexical Analyzer

The Lexical Analyzer's job is to get tokens for the Parser to utilize. Each token can correspond to a keyword, an operator, a terminal of the language, or an error. The Lexical Analyzer reads one character at a time from an input stream provided by the Parser, ignoring white space until it either recognizes a valid token or it encounters an error. The tokens sent to the Parser are instances of a wrapper class called LexItem, which allows one to store the line number where the token was generated and the string corresponding to it's lexeme. 

\[ [Lexical Analyzer](/lex.cpp) | [Lexical Analyzer Tester Program](/lexTester.cpp) | [Lexical Analyzer Test Files](/lexTests/) \]

## Parser

The Parser acts as both the parser and the interpreter for the language. It Parses the source code from the input file statement by statement and for each statement it does the following:
- It stops if there is a lexical/syntactic error (detected by the lexical analyzer).
- It checks if there are any run-time errors.
- Stops the parsing process if a run-time error is found.
- Excutes the statement if no run-time errors were found.

The Parser uses the lexical analyzer to get tokens through a function called `getNextToken()`. It then follows the grammar rules of the language to determine if a statement is valid or not. The Parser is implemented as a recursive descend Parser:

- The Parser includes one function per rule or non-terminal.
- Each of these functions recognizes the right hand side of the rule.
- If the rule requires another non-terminal then the function of this non-terminal is called.

The parsing status is dependent on the return value of the last non-terminal function called during the parsing process. If any of the non-terminals' function returns `false` then the parsing process is stopped. However all statements that were successfully parsed up until that point are executed.

\[ [Parser/Interpreter](/parserInt.cpp) | [Parser/Interpreter Tester Program](/parserTester.cpp) | [Parser/Interpreter Test Files](/parserTests/) \]
## Grammar of the language

```
Prog = PROGRAM IDENT {Decl} {Stmt} END PROGRAM IDENT 
Decl = (INTEGER | REAL | CHAR) : IdList
IdList = IDENT {,IDENT}
Stmt = AssigStmt | IfStmt | PrintStmt
PrintStmt = PRINT , ExprList
IfStmt = IF (LogicExpr) THEN {Stmt} END IF
AssignStmt = Var = Expr
ExprList = Expr {, Expr}
Expr = Term {(+|-) Term}
Term = SFactor {(\*|/) SFactor}
SFactor = Sign Factor | Factor
LogicExpr = Expr (== | <) Expr
Var = IDENT
Sign = + | -
Factor = IDENT | ICONST | RCONST | SCONST | (Expr)
```

## Example Program ([progtest2](/parserTests/progtest2))
The following is an example of a program that follows the grammar rules above and is successfully parsed and interpreted.
```
PROGRAM circle
    !Program with no errors
	!Variable declarations
	INTEGER : r, a, p, b 	
	CHAR : str1, str2
	!Read data
	PRINT , "Please the two radius values circle"
	READ, r, b
	!Display the results
	PRINT , "The output results are " , a+4, p*2, -b
END PROGRAM circle
```
---
This was a project for the CS 280 Programming Language Concepts course at the New Jersey Institute of Technology during the Spring 2021 Semester. The instructor was Dr. Bassel Arafeh. The grammar of the language, all header files (`.h` files), test files, and [`val.cpp`](/val.cpp) were provided by the instructor.
