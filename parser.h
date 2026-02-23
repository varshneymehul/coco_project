// Contains function prototype declarations of functions in parser.c
#include <stdint.h>

#define MAX_RHS_LENGTH 32
#define MAX_SYMBOL_LENGTH 40
#define MAX_GRAMMAR_LENGTH 64
#define MAX_SYMBOLS 128
#define EPSILON 65535

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
