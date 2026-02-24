#ifndef LEXER_DEF_H
#define LEXER_DEF_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LEXEME_SIZE 64
#define BUFFER_SIZE 512

// Token definitions
typedef enum {
    TK_WITH, TK_PARAMETERS, TK_END, TK_WHILE, TK_UNION, TK_ENDUNION, 
    TK_DEFINETYPE, TK_AS, TK_TYPE, TK_MAIN, TK_GLOBAL, TK_PARAMETER, 
    TK_LIST, TK_INPUT, TK_OUTPUT, TK_INT, TK_REAL, TK_ENDWHILE, TK_IF, 
    TK_THEN, TK_ENDIF, TK_READ, TK_WRITE, TK_RETURN, TK_CALL, TK_RECORD, 
    TK_ENDRECORD, TK_ELSE,
    TK_ASSIGNOP, TK_COMMENT, TK_SQL, TK_SQR, TK_COMMA, TK_SEM, TK_COLON, 
    TK_DOT, TK_OP, TK_CL, TK_PLUS, TK_MINUS, TK_MUL, TK_DIV, TK_AND, 
    TK_OR, TK_NOT, TK_LT, TK_LE, TK_EQ, TK_GT, TK_GE, TK_NE,
    TK_ID, TK_FUNID, TK_FIELDID, TK_RUID, TK_NUM, TK_RNUM,
    EPS, DOLLAR, TK_ERROR, TK_EOF
} TokenType;

typedef union {
    int numValue;
    double rnumValue;
} TokenValue;

typedef struct {
    TokenType type;
    char lexeme[MAX_LEXEME_SIZE];
    int lineNo;
    TokenValue value; // only relevant if token is TK_NUM or TK_RNUM
} tokenInfo;

typedef struct {
    char buffer[2][BUFFER_SIZE];
    int current_buf;
    int ptr;
    int num_bytes_read[2];
    FILE *fp;
    bool eof_reached;
} twinBufferStruct;

typedef twinBufferStruct* twinBuffer;

#endif
