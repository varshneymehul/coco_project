#ifndef PARSER_DEF_H
#define PARSER_DEF_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_RHS_LENGTH 32
#define MAX_SYMBOL_LENGTH 40
#define MAX_GRAMMAR_LENGTH 64
#define MAX_SYMBOLS 256
#define MAX_PRODUCTIONS 256
#define MAX_NON_TERMINALS 100
#define MAX_TERMINALS 256
#define STACK_SIZE 2000

typedef struct
{
    uint16_t lhs;
    uint16_t rhs[MAX_RHS_LENGTH];
    uint16_t rhs_length;

} Grammar;

typedef struct {
    uint16_t symbols[MAX_SYMBOLS];
    uint16_t count;
} SymbolSet;

typedef struct {
    SymbolSet *first_sets;  
    SymbolSet *follow_sets; 
    uint16_t num_symbols;
} FirstFollowSets;

typedef struct ParseTreeNode {
    uint16_t symbol;
    char lexeme[MAX_SYMBOL_LENGTH]; 
    int line_no;

    struct ParseTreeNode *parent;
    struct ParseTreeNode *children[MAX_RHS_LENGTH];
    int child_count;

} ParseTreeNode;

typedef struct ParseTree {
    ParseTreeNode *root;
} ParseTree;

typedef struct {
    ParseTreeNode *data[STACK_SIZE];
    int top;
} NodeStack;



#endif
