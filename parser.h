// Contains function prototype declarations of functions in parser.c
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_RHS_LENGTH 32
#define MAX_SYMBOL_LENGTH 40
#define MAX_GRAMMAR_LENGTH 64
#define MAX_SYMBOLS 256
#define EPSILON 0
#define MAX_PRODUCTIONS 256
#define END_OF_INPUT 254
#define SYM_S 1  
#define MAX_NON_TERMINALS 100
#define MAX_TERMINALS 256

char symbol_table[MAX_SYMBOLS][MAX_SYMBOL_LENGTH];
uint16_t symbol_count = 0;

Grammar grammar[MAX_GRAMMAR_LENGTH];
uint16_t grammar_count = 0;

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
