#ifndef PARSER_H
#define PARSER_H

#include "parserDef.h"

FirstFollowSets* ComputeFirstAndFollowSets(const char *filename);
void createParseTable(int parse_table[MAX_SYMBOLS][MAX_SYMBOLS], const FirstFollowSets *ff_sets);
ParseTree parseInputSourceCode(char *testcaseFile, int parse_table[MAX_SYMBOLS][MAX_SYMBOLS]);
void printParseTree(struct ParseTree PT, char *outfile);

#endif