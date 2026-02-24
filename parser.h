#ifndef PARSER_H
#define PARSER_H

#include "parserDef.h"

// Initialize hardcoded grammar, first, follow sets
void FirstAndFollowSets(grammar* G);
void createParseTable(FirstAndFollow F, table* T, grammar* G);
parseTree parseInputSourceCode(char *testcaseFile, table T, grammar G);
void printParseTree(parseTree PT, char *outfile);
void freeParseTree(parseTree PT);

#endif
