#ifndef LEXER_H
#define LEXER_H

#include "lexerDef.h"

// Lexer function prototypes
twinBuffer initBuffer(FILE *fp);
FILE *getStream(FILE *fp); // Original signature requested, we will manage twinBuffer within.
void loadBuffer(twinBuffer B, int buf_index);
char nextChar(twinBuffer B);
void retract(twinBuffer B, int steps);
tokenInfo getNextToken(twinBuffer B);
void removeComments(char *testcaseFile, char *cleanFile);
const char* getTokenName(TokenType type);
void closeBuffer(twinBuffer B);

#endif
