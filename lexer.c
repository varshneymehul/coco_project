#include "lexer.h"
#include <ctype.h>

// Global variable tracking line number for the tokens
int currentLineNo = 1;

twinBuffer initBuffer(FILE *fp) {
    twinBuffer B = (twinBuffer)malloc(sizeof(twinBufferStruct));
    B->fp = fp;
    B->current_buf = 0;
    B->ptr = 0;
    B->eof_reached = false;
    B->num_bytes_read[0] = 0;
    B->num_bytes_read[1] = 0;
    loadBuffer(B, 0);
    return B;
}

void loadBuffer(twinBuffer B, int buf_index) {
    if (B->eof_reached) return;
    size_t count = fread(B->buffer[buf_index], 1, BUFFER_SIZE, B->fp);
    B->num_bytes_read[buf_index] = count;
    if (count < BUFFER_SIZE) {
        B->buffer[buf_index][count] = '\0'; // Mark EOF in buffer
        B->eof_reached = true;
    }
}

// Wrapper to meet the prompt requirement: FILE *getStream(FILE *fp)
FILE *getStream(FILE *fp) {
    // According to instructions, getStream populates twin buffer.
    // Since we handle twin buffer stateably, we return fp to keep signature, 
    // but the actual buffer loading happens as needed in nextChar().
    return fp;
}

char nextChar(twinBuffer B) {
    if (B->ptr >= B->num_bytes_read[B->current_buf]) {
        // End of current buffer
        if (B->eof_reached && B->num_bytes_read[B->current_buf] < BUFFER_SIZE) {
            return '\0'; // true EOF
        }
        // Switch buffer
        int next_buf = 1 - B->current_buf;
        loadBuffer(B, next_buf);
        B->current_buf = next_buf;
        B->ptr = 0;
    }
    if (B->num_bytes_read[B->current_buf] == 0) return '\0';
    char c = B->buffer[B->current_buf][B->ptr++];
    return c;
}

void retract(twinBuffer B, int steps) {
    for (int i = 0; i < steps; i++) {
        if (B->ptr > 0) {
            B->ptr--;
        } else {
            B->current_buf = 1 - B->current_buf;
            B->ptr = BUFFER_SIZE - 1;
        }
    }
}

void closeBuffer(twinBuffer B) {
    if (B) free(B);
}

// String names for output
const char* tokenNames[] = {
    "TK_WITH", "TK_PARAMETERS", "TK_END", "TK_WHILE", "TK_UNION", "TK_ENDUNION", 
    "TK_DEFINETYPE", "TK_AS", "TK_TYPE", "TK_MAIN", "TK_GLOBAL", "TK_PARAMETER", 
    "TK_LIST", "TK_INPUT", "TK_OUTPUT", "TK_INT", "TK_REAL", "TK_ENDWHILE", "TK_IF", 
    "TK_THEN", "TK_ENDIF", "TK_READ", "TK_WRITE", "TK_RETURN", "TK_CALL", "TK_RECORD", 
    "TK_ENDRECORD", "TK_ELSE",
    "TK_ASSIGNOP", "TK_COMMENT", "TK_SQL", "TK_SQR", "TK_COMMA", "TK_SEM", "TK_COLON", 
    "TK_DOT", "TK_OP", "TK_CL", "TK_PLUS", "TK_MINUS", "TK_MUL", "TK_DIV", "TK_AND", 
    "TK_OR", "TK_NOT", "TK_LT", "TK_LE", "TK_EQ", "TK_GT", "TK_GE", "TK_NE",
    "TK_ID", "TK_FUNID", "TK_FIELDID", "TK_RUID", "TK_NUM", "TK_RNUM",
    "EPS", "DOLLAR", "TK_ERROR", "TK_EOF"
};

const char* getTokenName(TokenType type) {
    return tokenNames[type];
}

TokenType checkKeywordOrFieldId(char* lexeme) {
    if (strcmp(lexeme, "with") == 0) return TK_WITH;
    if (strcmp(lexeme, "parameters") == 0) return TK_PARAMETERS;
    if (strcmp(lexeme, "end") == 0) return TK_END;
    if (strcmp(lexeme, "while") == 0) return TK_WHILE;
    if (strcmp(lexeme, "union") == 0) return TK_UNION;
    if (strcmp(lexeme, "endunion") == 0) return TK_ENDUNION;
    if (strcmp(lexeme, "definetype") == 0) return TK_DEFINETYPE;
    if (strcmp(lexeme, "as") == 0) return TK_AS;
    if (strcmp(lexeme, "type") == 0) return TK_TYPE;
    if (strcmp(lexeme, "global") == 0) return TK_GLOBAL;
    if (strcmp(lexeme, "parameter") == 0) return TK_PARAMETER;
    if (strcmp(lexeme, "list") == 0) return TK_LIST;
    if (strcmp(lexeme, "input") == 0) return TK_INPUT;
    if (strcmp(lexeme, "output") == 0) return TK_OUTPUT;
    if (strcmp(lexeme, "int") == 0) return TK_INT;
    if (strcmp(lexeme, "real") == 0) return TK_REAL;
    if (strcmp(lexeme, "endwhile") == 0) return TK_ENDWHILE;
    if (strcmp(lexeme, "if") == 0) return TK_IF;
    if (strcmp(lexeme, "then") == 0) return TK_THEN;
    if (strcmp(lexeme, "endif") == 0) return TK_ENDIF;
    if (strcmp(lexeme, "read") == 0) return TK_READ;
    if (strcmp(lexeme, "write") == 0) return TK_WRITE;
    if (strcmp(lexeme, "return") == 0) return TK_RETURN;
    if (strcmp(lexeme, "call") == 0) return TK_CALL;
    if (strcmp(lexeme, "record") == 0) return TK_RECORD;
    if (strcmp(lexeme, "endrecord") == 0) return TK_ENDRECORD;
    if (strcmp(lexeme, "else") == 0) return TK_ELSE;
    return TK_FIELDID;
}

bool isSmallAlpha(char c) {
    return (c >= 'a' && c <= 'z');
}
bool isSplAlpha(char c) {
    return (c >= 'b' && c <= 'd');
}
bool isSplNum(char c) {
    return (c >= '2' && c <= '7');
}
bool isAlphabet(char c) {
    return isalpha(c);
}
bool isDigit(char c) {
    return isdigit(c);
}

tokenInfo getNextToken(twinBuffer B) {
    tokenInfo token;
    token.type = TK_ERROR;
    memset(token.lexeme, 0, MAX_LEXEME_SIZE);
    
    int state = 1;
    char c;
    int lex_len = 0;

    while (1) {
        c = nextChar(B);
        
        switch (state) {
            case 1:
                if (c == '\0') {
                    token.type = TK_EOF;
                    token.lineNo = currentLineNo;
                    strcpy(token.lexeme, "EOF");
                    return token;
                }
                else if (c == ' ' || c == '\t' || c == '\r') {
                    state = 1; // ignore whitespaces
                    lex_len = 0;
                }
                else if (c == '\n') {
                    currentLineNo++;
                    state = 1;
                    lex_len = 0;
                }
                else if (c == '%') {
                    // Comment
                    state = 2;
                }
                else if (c == '~') { token.type = TK_NOT; token.lexeme[lex_len++] = c; token.lineNo = currentLineNo; return token; }
                else if (c == '(') { token.type = TK_OP; token.lexeme[lex_len++] = c; token.lineNo = currentLineNo; return token; }
                else if (c == ')') { token.type = TK_CL; token.lexeme[lex_len++] = c; token.lineNo = currentLineNo; return token; }
                else if (c == '[') { token.type = TK_SQL; token.lexeme[lex_len++] = c; token.lineNo = currentLineNo; return token; }
                else if (c == ']') { token.type = TK_SQR; token.lexeme[lex_len++] = c; token.lineNo = currentLineNo; return token; }
                else if (c == ',') { token.type = TK_COMMA; token.lexeme[lex_len++] = c; token.lineNo = currentLineNo; return token; }
                else if (c == ';') { token.type = TK_SEM; token.lexeme[lex_len++] = c; token.lineNo = currentLineNo; return token; }
                else if (c == ':') { token.type = TK_COLON; token.lexeme[lex_len++] = c; token.lineNo = currentLineNo; return token; }
                else if (c == '.') { token.type = TK_DOT; token.lexeme[lex_len++] = c; token.lineNo = currentLineNo; return token; }
                else if (c == '+') { token.type = TK_PLUS; token.lexeme[lex_len++] = c; token.lineNo = currentLineNo; return token; }
                else if (c == '-') { token.type = TK_MINUS; token.lexeme[lex_len++] = c; token.lineNo = currentLineNo; return token; }
                else if (c == '*') { token.type = TK_MUL; token.lexeme[lex_len++] = c; token.lineNo = currentLineNo; return token; }
                else if (c == '/') { token.type = TK_DIV; token.lexeme[lex_len++] = c; token.lineNo = currentLineNo; return token; }
                else if (c == '&') { token.lexeme[lex_len++] = c; state = 3; }
                else if (c == '@') { token.lexeme[lex_len++] = c; state = 6; }
                else if (c == '=') { token.lexeme[lex_len++] = c; state = 9; }
                else if (c == '!') { token.lexeme[lex_len++] = c; state = 11; }
                else if (c == '>') { token.lexeme[lex_len++] = c; state = 13; }
                else if (c == '<') { token.lexeme[lex_len++] = c; state = 16; }
                else if (c == '_') { token.lexeme[lex_len++] = c; state = 22; }
                else if (c == '#') { token.lexeme[lex_len++] = c; state = 26; }
                else if (isDigit(c)) { token.lexeme[lex_len++] = c; state = 30; }
                else if (isSplAlpha(c)) { token.lexeme[lex_len++] = c; state = 40; }
                else if (isSmallAlpha(c)) { token.lexeme[lex_len++] = c; state = 50; }
                else {
                    // Unknown character error
                    token.type = TK_ERROR;
                    token.lexeme[lex_len++] = c;
                    token.lineNo = currentLineNo;
                    printf("Line no: %d : Error: Unknown pattern <%c>\n", currentLineNo, c);
                    return token;
                }
                break;
            
            case 2: // Comment loop
                if (c == '\n') {
                    currentLineNo++;
                    state = 1;
                    lex_len = 0;
                } else if (c == '\0') {
                    // EOF in comment
                    token.type = TK_EOF;
                    token.lineNo = currentLineNo;
                    strcpy(token.lexeme, "EOF");
                    return token;
                }
                break;

            case 3:
                if (c == '&') { token.lexeme[lex_len++] = c; state = 4; }
                else { retract(B, 1); token.type = TK_ERROR; token.lineNo = currentLineNo; return token; }
                break;
            case 4:
                if (c == '&') { token.lexeme[lex_len++] = c; token.type = TK_AND; token.lineNo = currentLineNo; return token; }
                else { retract(B, 1); token.type = TK_ERROR; token.lineNo = currentLineNo; return token; }
                break;

            case 6:
                if (c == '@') { token.lexeme[lex_len++] = c; state = 7; }
                else { retract(B, 1); token.type = TK_ERROR; token.lineNo = currentLineNo; return token; }
                break;
            case 7:
                if (c == '@') { token.lexeme[lex_len++] = c; token.type = TK_OR; token.lineNo = currentLineNo; return token; }
                else { retract(B, 1); token.type = TK_ERROR; token.lineNo = currentLineNo; return token; }
                break;

            case 9:
                if (c == '=') { token.lexeme[lex_len++] = c; token.type = TK_EQ; token.lineNo = currentLineNo; return token; }
                else { retract(B, 1); token.type = TK_ERROR; token.lineNo = currentLineNo; return token; }
                break;

            case 11:
                if (c == '=') { token.lexeme[lex_len++] = c; token.type = TK_NE; token.lineNo = currentLineNo; return token; }
                else { retract(B, 1); token.type = TK_ERROR; token.lineNo = currentLineNo; return token; }
                break;

            case 13:
                if (c == '=') { token.lexeme[lex_len++] = c; token.type = TK_GE; token.lineNo = currentLineNo; return token; }
                else { retract(B, 1); token.type = TK_GT; token.lineNo = currentLineNo; return token; }
                break;

            case 16:
                if (c == '=') { token.lexeme[lex_len++] = c; token.type = TK_LE; token.lineNo = currentLineNo; return token; }
                else if (c == '-') { token.lexeme[lex_len++] = c; state = 18; }
                else { retract(B, 1); token.type = TK_LT; token.lineNo = currentLineNo; return token; }
                break;
            case 18:
                if (c == '-') { token.lexeme[lex_len++] = c; state = 19; }
                else { retract(B, 2); token.lexeme[--lex_len] = '\0'; token.type = TK_LT; token.lineNo = currentLineNo; return token; }
                break;
            case 19:
                if (c == '-') { token.lexeme[lex_len++] = c; token.type = TK_ASSIGNOP; token.lineNo = currentLineNo; return token; }
                else { retract(B, 3); token.lexeme[lex_len -= 2] = '\0'; token.type = TK_LT; token.lineNo = currentLineNo; return token; }
                break;

            // TK_FUNID & TK_MAIN : _[a-zA-Z]+[0-9]* 
            case 22:
                if (isAlphabet(c)) { token.lexeme[lex_len++] = c; state = 23; }
                else { retract(B, 1); token.type = TK_ERROR; token.lineNo = currentLineNo; return token; }
                break;
            case 23:
                if (isAlphabet(c)) { token.lexeme[lex_len++] = c; state = 23; }
                else if (isDigit(c)) { token.lexeme[lex_len++] = c; state = 24; }
                else {
                    retract(B, 1);
                    if (strcmp(token.lexeme, "_main") == 0) {
                        token.type = TK_MAIN;
                    } else if (lex_len <= 30) {
                        token.type = TK_FUNID;
                    } else {
                        token.type = TK_ERROR;
                        printf("Line no: %d : Error: Identifier is longer than the prescribed length <%s>\n", currentLineNo, token.lexeme);
                    }
                    token.lineNo = currentLineNo;
                    return token;
                }
                break;
            case 24:
                if (isDigit(c)) { token.lexeme[lex_len++] = c; state = 24; }
                else {
                    retract(B, 1);
                    if (lex_len <= 30) token.type = TK_FUNID;
                    else { token.type = TK_ERROR; printf("Line no: %d : Error: Identifier is longer than the prescribed length <%s>\n", currentLineNo, token.lexeme); }
                    token.lineNo = currentLineNo;
                    return token;
                }
                break;

            // TK_RUID : #[a-z]+
            case 26:
                if (isSmallAlpha(c)) { token.lexeme[lex_len++] = c; state = 27; }
                else { retract(B, 1); token.type = TK_ERROR; token.lineNo = currentLineNo; return token; }
                break;
            case 27:
                if (isSmallAlpha(c)) { token.lexeme[lex_len++] = c; state = 27; }
                else { retract(B, 1); token.type = TK_RUID; token.lineNo = currentLineNo; return token; }
                break;

            // numbers
            case 30:
                if (isDigit(c)) { token.lexeme[lex_len++] = c; state = 30; }
                else if (c == '.') { token.lexeme[lex_len++] = c; state = 31; }
                else {
                    retract(B, 1);
                    token.type = TK_NUM;
                    token.value.numValue = atoi(token.lexeme);
                    token.lineNo = currentLineNo;
                    return token;
                }
                break;
            case 31:
                if (isDigit(c)) { token.lexeme[lex_len++] = c; state = 32; }
                else { 
                    retract(B, 2); 
                    token.lexeme[--lex_len] = '\0'; 
                    token.type = TK_NUM; 
                    token.value.numValue = atoi(token.lexeme); 
                    token.lineNo = currentLineNo; 
                    return token; 
                }
                break;
            case 32:
                if (isDigit(c)) { token.lexeme[lex_len++] = c; state = 33; }
                else { 
                    retract(B, 1); 
                    token.type = TK_ERROR; 
                    token.lineNo = currentLineNo; 
                    printf("Line no: %d : Error: Unknown pattern <%s>\n", currentLineNo, token.lexeme);
                    return token; 
                }
                break;
            case 33:
                if (c == 'E') { token.lexeme[lex_len++] = c; state = 34; }
                else {
                    retract(B, 1);
                    token.type = TK_RNUM;
                    token.value.rnumValue = atof(token.lexeme);
                    token.lineNo = currentLineNo;
                    return token;
                }
                break;
            case 34:
                if (c == '+' || c == '-') { token.lexeme[lex_len++] = c; state = 35; }
                else if (isDigit(c)) { token.lexeme[lex_len++] = c; state = 36; }
                else { 
                    retract(B, 1); 
                    token.type = TK_ERROR; 
                    token.lineNo = currentLineNo; 
                    printf("Line no: %d : Error: Unknown pattern <%s>\n", currentLineNo, token.lexeme);
                    return token; 
                }
                break;
            case 35:
                if (isDigit(c)) { token.lexeme[lex_len++] = c; state = 36; }
                else { 
                    retract(B, 1); 
                    token.type = TK_ERROR; 
                    token.lineNo = currentLineNo; 
                    printf("Line no: %d : Error: Unknown pattern <%s>\n", currentLineNo, token.lexeme);
                    return token; 
                }
                break;
            case 36:
                if (isDigit(c)) { 
                    token.lexeme[lex_len++] = c; 
                    state = 37; 
                }
                else { 
                    retract(B, 1); 
                    token.type = TK_ERROR; 
                    token.lineNo = currentLineNo; 
                    printf("Line no: %d : Error: Unknown pattern <%s>\n", currentLineNo, token.lexeme);
                    return token; 
                }
                break;
            case 37:
                // Expected 'Other' since we already read 2 digits for exponent
                // 'Other' includes anything, even a digit like '7' in '12.34E567'
                retract(B, 1);
                token.type = TK_RNUM;
                token.value.rnumValue = atof(token.lexeme);
                token.lineNo = currentLineNo;
                return token;

            // TK_ID : [b-d][2-7][b-d]*[2-7]* length 2 to 20
            case 40:
                if (isSplNum(c)) { token.lexeme[lex_len++] = c; state = 41; }
                else if (isSmallAlpha(c)) { token.lexeme[lex_len++] = c; state = 50; } // fell back into field id path since it wasn't a TK_ID
                else { retract(B, 1); token.type = checkKeywordOrFieldId(token.lexeme); token.lineNo = currentLineNo; return token; }
                break;
            case 41:
                if (isSplAlpha(c)) { token.lexeme[lex_len++] = c; state = 41; }
                else if (isSplNum(c)) { token.lexeme[lex_len++] = c; state = 42; }
                else {
                    retract(B, 1);
                    if (lex_len >= 2 && lex_len <= 20) token.type = TK_ID;
                    else { token.type = TK_ERROR; printf("Line %d: Lexical Error - TK_ID length violation\n", currentLineNo); }
                    token.lineNo = currentLineNo; return token;
                }
                break;
            case 42:
                if (isSplNum(c)) { token.lexeme[lex_len++] = c; state = 42; }
                else {
                    retract(B, 1);
                    if (lex_len >= 2 && lex_len <= 20) token.type = TK_ID;
                    else { token.type = TK_ERROR; printf("Line %d: Lexical Error - TK_ID length violation\n", currentLineNo); }
                    token.lineNo = currentLineNo; return token;
                }
                break;

            // TK_FIELDID / Keywords : [a-z]+
            case 50:
                if (isSmallAlpha(c)) { token.lexeme[lex_len++] = c; state = 50; }
                else {
                    retract(B, 1);
                    token.type = checkKeywordOrFieldId(token.lexeme);
                    token.lineNo = currentLineNo;
                    return token;
                }
                break;
        }
    }
}

void removeComments(char *testcaseFile, char *cleanFile) {
    FILE *in = fopen(testcaseFile, "r");
    FILE *out = fopen(cleanFile, "w");
    if (!in || !out) return;

    char c;
    bool in_comment = false;
    while ((c = fgetc(in)) != EOF) {
        if (c == '%') {
            in_comment = true;
        }
        if (c == '\n') {
            in_comment = false;
            fputc(c, out);
            continue;
        }
        if (!in_comment) {
            fputc(c, out);
        }
    }
    fclose(in);
    fclose(out);
}
