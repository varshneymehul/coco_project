#ifndef PARSER_DEF_H
#define PARSER_DEF_H

#include "lexer.h"

#define NUM_NON_TERMINALS 54
#define NUM_TERMINALS 59 // Excluding EPS, DOLLAR, TK_ERROR, TK_EOF
#define NUM_GRAMMAR_RULES 95 // approximate maximum
#define MAX_RHS 10

// NonTerminal Enums
typedef enum {
    NT_PROGRAM, NT_MAINFUNCTION, NT_OTHERFUNCTIONS, NT_FUNCTION, NT_INPUT_PAR, 
    NT_OUTPUT_PAR, NT_PARAMETER_LIST, NT_DATATYPE, NT_PRIMITIVEDATATYPE, 
    NT_CONSTRUCTEDDATATYPE, NT_REMAINING_LIST, NT_STMTS, NT_TYPEDEFINITIONS, 
    NT_ACTUALORREDEFINED, NT_TYPEDEFINITION, NT_FIELDDEFINITIONS, 
    NT_FIELDDEFINITION, NT_FIELDTYPE, NT_MOREFIELDS, NT_DECLARATIONS, 
    NT_DECLARATION, NT_GLOBAL_OR_NOT, NT_OTHERSTMTS, NT_STMT, NT_ASSIGNMENTSTMT, 
    NT_SINGLEORRECID, NT_OPTION_SINGLE_CONSTRUCTED, NT_ONEEXPANSION, 
    NT_MOREEXPANSIONS, NT_FUNCALLSTMT, NT_OUTPUTPARAMETERS, NT_INPUTPARAMETERS, 
    NT_ITERATIVESTMT, NT_CONDITIONALSTMT, NT_ELSEPART, NT_IOSTMT, 
    NT_ARITHMETICEXPRESSION, NT_EXPPRIME, NT_TERM, NT_TERMPRIME, NT_FACTOR, 
    NT_HIGHPRECEDENCEOPERATORS, NT_LOWPRECEDENCEOPERATORS, NT_BOOLEANEXPRESSION, 
    NT_VAR, NT_LOGICALOP, NT_RELATIONALOP, NT_RETURNSTMT, NT_OPTIONALRETURN, 
    NT_IDLIST, NT_MORE_IDS, NT_DEFINETYPESTMT, NT_A
} NonTerminal;

typedef struct {
    int lhs; // NonTerminal
    int rhsSize;
    int rhs[MAX_RHS]; // Negative for terminals (e.g. -TK_WHILE), Positive for NonTerminals
} GrammarRule;

typedef struct {
    GrammarRule rules[NUM_GRAMMAR_RULES];
    int numRules;
} grammar;

struct ParseTreeNode {
    char lexeme[MAX_LEXEME_SIZE];
    int lineNo;
    TokenType token;
    TokenValue value; // only relevant if numeric
    
    int symbol; // > 0 NonTerminal, < 0 Terminal (Negative matching enum)
    const char* nonTerminalName;
    const char* tokenName;

    struct ParseTreeNode* parent;
    struct ParseTreeNode* children[MAX_RHS];
    int numChildren;
    
    bool isLeaf;
};

typedef struct ParseTreeNode* parseTree;

typedef struct {
    // We will dynamically allocate or hardcode 2D arrays 
    // Here we can store sizes/indices in case we need them
} FirstAndFollow;

typedef struct {
    int table[NUM_NON_TERMINALS][NUM_TERMINALS + 2]; // +2 for EPS and DOLLAR
} table;

#endif
