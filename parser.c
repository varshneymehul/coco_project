#include "parser.h"

// Hardcoded grammar
GrammarRule initialRules[] = {
    { NT_PROGRAM, 2, { NT_OTHERFUNCTIONS, NT_MAINFUNCTION } },
    { NT_MAINFUNCTION, 3, { -TK_MAIN, NT_STMTS, -TK_END } },
    { NT_OTHERFUNCTIONS, 2, { NT_FUNCTION, NT_OTHERFUNCTIONS } },
    { NT_OTHERFUNCTIONS, 1, { -EPS } },
    { NT_FUNCTION, 6, { -TK_FUNID, NT_INPUT_PAR, NT_OUTPUT_PAR, -TK_SEM, NT_STMTS, -TK_END } },
    { NT_INPUT_PAR, 6, { -TK_INPUT, -TK_PARAMETER, -TK_LIST, -TK_SQL, NT_PARAMETER_LIST, -TK_SQR } },
    { NT_OUTPUT_PAR, 6, { -TK_OUTPUT, -TK_PARAMETER, -TK_LIST, -TK_SQL, NT_PARAMETER_LIST, -TK_SQR } },
    { NT_OUTPUT_PAR, 1, { -EPS } },
    { NT_PARAMETER_LIST, 3, { NT_DATATYPE, -TK_ID, NT_REMAINING_LIST } },
    { NT_DATATYPE, 1, { NT_PRIMITIVEDATATYPE } },
    { NT_DATATYPE, 1, { NT_CONSTRUCTEDDATATYPE } },
    { NT_PRIMITIVEDATATYPE, 1, { -TK_INT } },
    { NT_PRIMITIVEDATATYPE, 1, { -TK_REAL } },
    { NT_CONSTRUCTEDDATATYPE, 2, { -TK_RECORD, -TK_RUID } },
    { NT_CONSTRUCTEDDATATYPE, 2, { -TK_UNION, -TK_RUID } },
    { NT_CONSTRUCTEDDATATYPE, 1, { -TK_RUID } },
    { NT_REMAINING_LIST, 2, { -TK_COMMA, NT_PARAMETER_LIST } },
    { NT_REMAINING_LIST, 1, { -EPS } },
    { NT_STMTS, 4, { NT_TYPEDEFINITIONS, NT_DECLARATIONS, NT_OTHERSTMTS, NT_RETURNSTMT } },
    { NT_TYPEDEFINITIONS, 2, { NT_ACTUALORREDEFINED, NT_TYPEDEFINITIONS } },
    { NT_TYPEDEFINITIONS, 1, { -EPS } },
    { NT_ACTUALORREDEFINED, 1, { NT_TYPEDEFINITION } },
    { NT_ACTUALORREDEFINED, 1, { NT_DEFINETYPESTMT } },
    { NT_TYPEDEFINITION, 4, { -TK_RECORD, -TK_RUID, NT_FIELDDEFINITIONS, -TK_ENDRECORD } },
    { NT_TYPEDEFINITION, 4, { -TK_UNION, -TK_RUID, NT_FIELDDEFINITIONS, -TK_ENDUNION } },
    { NT_FIELDDEFINITIONS, 3, { NT_FIELDDEFINITION, NT_FIELDDEFINITION, NT_MOREFIELDS } },
    { NT_FIELDDEFINITION, 5, { -TK_TYPE, NT_FIELDTYPE, -TK_COLON, -TK_FIELDID, -TK_SEM } },
    { NT_FIELDTYPE, 1, { NT_PRIMITIVEDATATYPE } },
    { NT_FIELDTYPE, 1, { NT_CONSTRUCTEDDATATYPE } },
    { NT_MOREFIELDS, 2, { NT_FIELDDEFINITION, NT_MOREFIELDS } },
    { NT_MOREFIELDS, 1, { -EPS } },
    { NT_DECLARATIONS, 2, { NT_DECLARATION, NT_DECLARATIONS } },
    { NT_DECLARATIONS, 1, { -EPS } },
    { NT_DECLARATION, 6, { -TK_TYPE, NT_DATATYPE, -TK_COLON, -TK_ID, NT_GLOBAL_OR_NOT, -TK_SEM } },
    { NT_GLOBAL_OR_NOT, 2, { -TK_COLON, -TK_GLOBAL } },
    { NT_GLOBAL_OR_NOT, 1, { -EPS } },
    { NT_OTHERSTMTS, 2, { NT_STMT, NT_OTHERSTMTS } },
    { NT_OTHERSTMTS, 1, { -EPS } },
    { NT_STMT, 1, { NT_ASSIGNMENTSTMT } },
    { NT_STMT, 1, { NT_ITERATIVESTMT } },
    { NT_STMT, 1, { NT_CONDITIONALSTMT } },
    { NT_STMT, 1, { NT_IOSTMT } },
    { NT_STMT, 1, { NT_FUNCALLSTMT } },
    { NT_ASSIGNMENTSTMT, 4, { NT_SINGLEORRECID, -TK_ASSIGNOP, NT_ARITHMETICEXPRESSION, -TK_SEM } },
    { NT_SINGLEORRECID, 2, { -TK_ID, NT_OPTION_SINGLE_CONSTRUCTED } },
    { NT_OPTION_SINGLE_CONSTRUCTED, 1, { -EPS } },
    { NT_OPTION_SINGLE_CONSTRUCTED, 2, { NT_ONEEXPANSION, NT_MOREEXPANSIONS } },
    { NT_ONEEXPANSION, 2, { -TK_DOT, -TK_FIELDID } },
    { NT_MOREEXPANSIONS, 2, { NT_ONEEXPANSION, NT_MOREEXPANSIONS } },
    { NT_MOREEXPANSIONS, 1, { -EPS } },
    { NT_FUNCALLSTMT, 7, { NT_OUTPUTPARAMETERS, -TK_CALL, -TK_FUNID, -TK_WITH, -TK_PARAMETERS, NT_INPUTPARAMETERS, -TK_SEM } },
    { NT_OUTPUTPARAMETERS, 4, { -TK_SQL, NT_IDLIST, -TK_SQR, -TK_ASSIGNOP } },
    { NT_OUTPUTPARAMETERS, 1, { -EPS } },
    { NT_INPUTPARAMETERS, 3, { -TK_SQL, NT_IDLIST, -TK_SQR } },
    { NT_ITERATIVESTMT, 7, { -TK_WHILE, -TK_OP, NT_BOOLEANEXPRESSION, -TK_CL, NT_STMT, NT_OTHERSTMTS, -TK_ENDWHILE } },
    { NT_CONDITIONALSTMT, 8, { -TK_IF, -TK_OP, NT_BOOLEANEXPRESSION, -TK_CL, -TK_THEN, NT_STMT, NT_OTHERSTMTS, NT_ELSEPART } },
    { NT_ELSEPART, 4, { -TK_ELSE, NT_STMT, NT_OTHERSTMTS, -TK_ENDIF } },
    { NT_ELSEPART, 1, { -TK_ENDIF } },
    { NT_IOSTMT, 5, { -TK_READ, -TK_OP, NT_VAR, -TK_CL, -TK_SEM } },
    { NT_IOSTMT, 5, { -TK_WRITE, -TK_OP, NT_VAR, -TK_CL, -TK_SEM } },
    { NT_ARITHMETICEXPRESSION, 2, { NT_TERM, NT_EXPPRIME } },
    { NT_EXPPRIME, 3, { NT_LOWPRECEDENCEOPERATORS, NT_TERM, NT_EXPPRIME } },
    { NT_EXPPRIME, 1, { -EPS } },
    { NT_TERM, 2, { NT_FACTOR, NT_TERMPRIME } },
    { NT_TERMPRIME, 3, { NT_HIGHPRECEDENCEOPERATORS, NT_FACTOR, NT_TERMPRIME } },
    { NT_TERMPRIME, 1, { -EPS } },
    { NT_FACTOR, 3, { -TK_OP, NT_ARITHMETICEXPRESSION, -TK_CL } },
    { NT_FACTOR, 1, { NT_VAR } },
    { NT_HIGHPRECEDENCEOPERATORS, 1, { -TK_MUL } },
    { NT_HIGHPRECEDENCEOPERATORS, 1, { -TK_DIV } },
    { NT_LOWPRECEDENCEOPERATORS, 1, { -TK_PLUS } },
    { NT_LOWPRECEDENCEOPERATORS, 1, { -TK_MINUS } },
    { NT_BOOLEANEXPRESSION, 7, { -TK_OP, NT_BOOLEANEXPRESSION, -TK_CL, NT_LOGICALOP, -TK_OP, NT_BOOLEANEXPRESSION, -TK_CL } },
    { NT_BOOLEANEXPRESSION, 3, { NT_VAR, NT_RELATIONALOP, NT_VAR } },
    { NT_BOOLEANEXPRESSION, 4, { -TK_NOT, -TK_OP, NT_BOOLEANEXPRESSION, -TK_CL } },
    { NT_VAR, 1, { NT_SINGLEORRECID } },
    { NT_VAR, 1, { -TK_NUM } },
    { NT_VAR, 1, { -TK_RNUM } },
    { NT_LOGICALOP, 1, { -TK_AND } },
    { NT_LOGICALOP, 1, { -TK_OR } },
    { NT_RELATIONALOP, 1, { -TK_LT } },
    { NT_RELATIONALOP, 1, { -TK_LE } },
    { NT_RELATIONALOP, 1, { -TK_EQ } },
    { NT_RELATIONALOP, 1, { -TK_GT } },
    { NT_RELATIONALOP, 1, { -TK_GE } },
    { NT_RELATIONALOP, 1, { -TK_NE } },
    { NT_RETURNSTMT, 3, { -TK_RETURN, NT_OPTIONALRETURN, -TK_SEM } },
    { NT_OPTIONALRETURN, 3, { -TK_SQL, NT_IDLIST, -TK_SQR } },
    { NT_OPTIONALRETURN, 1, { -EPS } },
    { NT_IDLIST, 2, { -TK_ID, NT_MORE_IDS } },
    { NT_MORE_IDS, 2, { -TK_COMMA, NT_IDLIST } },
    { NT_MORE_IDS, 1, { -EPS } },
    { NT_DEFINETYPESTMT, 5, { -TK_DEFINETYPE, NT_A, -TK_RUID, -TK_AS, -TK_RUID } },
    { NT_A, 1, { -TK_RECORD } },
    { NT_A, 1, { -TK_UNION } }
};

bool firstSets[NUM_NON_TERMINALS][NUM_TERMINALS + 2];
bool followSets[NUM_NON_TERMINALS][NUM_TERMINALS + 2];

const char* ntNames[] = {
    "program", "mainFunction", "otherFunctions", "function", "input_par", 
    "output_par", "parameter_list", "dataType", "primitiveDatatype", 
    "constructedDatatype", "remaining_list", "stmts", "typeDefinitions", 
    "actualOrRedefined", "typeDefinition", "fieldDefinitions", 
    "fieldDefinition", "fieldType", "moreFields", "declarations", 
    "declaration", "global_or_not", "otherStmts", "stmt", "assignmentStmt", 
    "singleOrRecId", "option_single_constructed", "oneExpansion", 
    "moreExpansions", "funCallStmt", "outputParameters", "inputParameters", 
    "iterativeStmt", "conditionalStmt", "elsePart", "ioStmt", 
    "arithmeticExpression", "expPrime", "term", "termPrime", "factor", 
    "highPrecedenceOperators", "lowPrecedenceOperators", "booleanExpression", 
    "var", "logicalOp", "relationalOp", "returnStmt", "optionalReturn", 
    "idList", "more_ids", "definetypestmt", "A"
};

void FirstAndFollowSets(grammar* G) {
    G->numRules = sizeof(initialRules) / sizeof(GrammarRule);
    for (int i = 0; i < G->numRules; i++) {
        G->rules[i] = initialRules[i];
    }
    
    memset(firstSets, 0, sizeof(firstSets));
    memset(followSets, 0, sizeof(followSets));
    
    bool changed = true;
    while(changed) {
        changed = false;
        for(int i = 0; i < G->numRules; i++) {
            GrammarRule rule = G->rules[i];
            int lhs = rule.lhs;
            bool eAll = true;
            for(int j = 0; j < rule.rhsSize; j++) {
                int sym = rule.rhs[j];
                if (sym < 0) {
                    if (!firstSets[lhs][-sym]) {
                        firstSets[lhs][-sym] = true;
                        changed = true;
                    }
                    if (sym != -EPS) eAll = false;
                    break;
                } else {
                    bool hasEps = false;
                    for(int k=0; k<=NUM_TERMINALS+1; k++) {
                        if (k == EPS) continue;
                        if (firstSets[sym][k] && !firstSets[lhs][k]) {
                            firstSets[lhs][k] = true;
                            changed = true;
                        }
                    }
                    if (firstSets[sym][EPS]) {
                        hasEps = true;
                    }
                    if (!hasEps) {
                        eAll = false;
                        break;
                    }
                }
            }
            if (eAll && !firstSets[lhs][EPS]) {
                firstSets[lhs][EPS] = true;
                changed = true;
            }
        }
    }

    followSets[NT_PROGRAM][DOLLAR] = true;
    changed = true;
    while(changed) {
        changed = false;
        for(int i = 0; i < G->numRules; i++) {
            GrammarRule rule = G->rules[i];
            int lhs = rule.lhs;
            for(int j = 0; j < rule.rhsSize; j++) {
                int B = rule.rhs[j];
                if (B >= 0) { // is NonTerminal
                    bool eAll = true;
                    for(int k = j + 1; k < rule.rhsSize; k++) {
                        int sym = rule.rhs[k];
                        if (sym < 0) {
                            if (sym != -EPS && !followSets[B][-sym]) {
                                followSets[B][-sym] = true;
                                changed = true;
                            }
                            if (sym != -EPS) eAll = false;
                            break;
                        } else {
                            bool hasEps = false;
                            for(int t=0; t<=NUM_TERMINALS+1; t++) {
                                if (t == EPS) continue;
                                if (firstSets[sym][t] && !followSets[B][t]) {
                                    followSets[B][t] = true;
                                    changed = true;
                                }
                            }
                            if (firstSets[sym][EPS]) hasEps = true;
                            if (!hasEps) { eAll = false; break; }
                        }
                    }
                    if (eAll) {
                        for(int t=0; t<=NUM_TERMINALS+1; t++) {
                            if (followSets[lhs][t] && !followSets[B][t]) {
                                followSets[B][t] = true;
                                changed = true;
                            }
                        }
                    }
                }
            }
        }
    }
}

void createParseTable(FirstAndFollow F, table* T, grammar* G) {
    memset(T->table, -1, sizeof(T->table));
    for (int i = 0; i < G->numRules; i++) {
        GrammarRule rule = G->rules[i];
        int A = rule.lhs;
        bool eAll = true;
        for (int j = 0; j < rule.rhsSize; j++) {
            int sym = rule.rhs[j];
            if (sym < 0) {
                if (sym != -EPS) {
                    T->table[A][-sym] = i;
                }
                if (sym != -EPS) eAll = false;
                break;
            } else {
                for(int t=0; t<=NUM_TERMINALS+1; t++) {
                    if (t == EPS) continue;
                    if (firstSets[sym][t]) {
                        T->table[A][t] = i;
                    }
                }
                if (!firstSets[sym][EPS]) {
                    eAll = false; break;
                }
            }
        }
        if (eAll) {
            for(int t=0; t<=NUM_TERMINALS+1; t++) {
                if (followSets[A][t]) {
                    T->table[A][t] = i;
                }
            }
        }
    }
}

// Stack ADT for predictive parser
typedef struct StackNode {
    int symbol; // >=0 NonTerminal, <0 Terminal
    struct ParseTreeNode* treeNode;
    struct StackNode* next;
} StackNode;

void push(StackNode** top, int sym, struct ParseTreeNode* treeNode) {
    StackNode* nn = (StackNode*)malloc(sizeof(StackNode));
    nn->symbol = sym;
    nn->treeNode = treeNode;
    nn->next = *top;
    *top = nn;
}

StackNode* pop(StackNode** top) {
    if (*top == NULL) return NULL;
    StackNode* temp = *top;
    *top = (*top)->next;
    return temp;
}

struct ParseTreeNode* createTreeNode(int sym) {
    struct ParseTreeNode* node = (struct ParseTreeNode*)malloc(sizeof(struct ParseTreeNode));
    node->symbol = sym;
    node->parent = NULL;
    node->numChildren = 0;
    node->isLeaf = (sym < 0);
    if (sym < 0) {
        node->tokenName = getTokenName(-sym);
    } else {
        node->nonTerminalName = ntNames[sym];
    }
    memset(node->lexeme, 0, MAX_LEXEME_SIZE);
    return node;
}

parseTree parseInputSourceCode(char *testcaseFile, table T, grammar G) {
    FILE *fp = fopen(testcaseFile, "r");
    if(!fp) { printf("Error opening file.\n"); return NULL; }
    
    twinBuffer B = initBuffer(fp);
    
    StackNode* st = NULL;
    push(&st, -DOLLAR, NULL);
    
    parseTree root = createTreeNode(NT_PROGRAM);
    push(&st, NT_PROGRAM, root);
    
    tokenInfo token = getNextToken(B);
    bool errorFound = false;

    while (st != NULL) {
        StackNode* curr = pop(&st);
        int X = curr->symbol;
        struct ParseTreeNode* Xnode = curr->treeNode;
        free(curr);

        if (X == -DOLLAR) {
            if (token.type == TK_EOF) {
                // Done
            } else {
                printf("Line %d: Syntax Error - Expected EOF but got %s\n", token.lineNo, getTokenName(token.type));
                errorFound = true;
            }
            break;
        }

        if (X < 0) { // Terminal on stack
            if (X == -EPS) {
                Xnode->isLeaf = true;
                strcpy(Xnode->lexeme, "----");
            } else if (-X == token.type) {
                Xnode->isLeaf = true;
                strcpy(Xnode->lexeme, token.lexeme);
                Xnode->lineNo = token.lineNo;
                if (token.type == TK_NUM || token.type == TK_RNUM) {
                    Xnode->value = token.value;
                }
                token = getNextToken(B);
            } else {
                printf("Line %d: Syntax Error - The token %s does not match expected token %s\n", token.lineNo, getTokenName(token.type), getTokenName(-X));
                errorFound = true;
                break; // Or error recovery
            }
        } else { // Non-Terminal on stack
            int ruleIdx = T.table[X][token.type];
            if (ruleIdx != -1) {
                GrammarRule r = G.rules[ruleIdx];
                for(int j = 0; j < r.rhsSize; j++) {
                    struct ParseTreeNode* child = createTreeNode(r.rhs[j]);
                    child->parent = Xnode;
                    Xnode->children[j] = child;
                    Xnode->numChildren++;
                }
                for(int j = r.rhsSize - 1; j >= 0; j--) {
                    push(&st, r.rhs[j], Xnode->children[j]);
                }
            } else {
                printf("Line %d: Syntax Error - Unrecognized syntactic construct (token %s) for non-terminal %s\n", token.lineNo, getTokenName(token.type), ntNames[X]);
                errorFound = true;
                // Simple panic mode recovery: skip token to find progress
                token = getNextToken(B);
                if (token.type == TK_EOF) break;
                // push it back instead of failing immediately to attempt continuation could go here
                // but standard dictates failing/reporting.
            }
        }
    }
    
    closeBuffer(B);
    fclose(fp);

    if (!errorFound) {
        printf("Input source code is syntactically correct...........\n");
    }
    return root;
}

void printParseTreeInorder(parseTree PT, FILE *fout) {
    if (PT == NULL) return;
    
    // Inorder: visit leftmost child, visit parent, visit remaining children
    if (PT->numChildren > 0) {
        printParseTreeInorder(PT->children[0], fout);
    }
    
    // Visit current node
    // Format: lexeme CurrentNodeLineNo tokenName valueIfNumber parentNodeSymbol isLeafNode(yes/no) NodeSymbol
    char lex[MAX_LEXEME_SIZE+4] = "----";
    char lineStr[20] = "----";
    char tokenStr[50] = "----";
    char valStr[50] = "----";
    char parentStr[50] = "ROOT";
    char isLeafStr[10] = "no";
    char nodeStr[50] = "----";

    if (PT->isLeaf) {
        if (PT->symbol != -EPS) {
            strcpy(lex, PT->lexeme);
            sprintf(lineStr, "%d", PT->lineNo);
            strcpy(tokenStr, PT->tokenName);
            if (PT->symbol == -TK_NUM) sprintf(valStr, "%d", PT->value.numValue);
            else if (PT->symbol == -TK_RNUM) sprintf(valStr, "%f", PT->value.rnumValue);
        } else {
            strcpy(tokenStr, "EPS");
        }
        strcpy(isLeafStr, "yes");
    } else {
        strcpy(nodeStr, PT->nonTerminalName);
    }

    if (PT->parent != NULL) {
        strcpy(parentStr, PT->parent->nonTerminalName);
    }

    fprintf(fout, "%-20s %-10s %-20s %-15s %-25s %-10s %-25s\n", 
            lex, lineStr, tokenStr, valStr, parentStr, isLeafStr, nodeStr);
    
    // Visit remaining children
    for (int i = 1; i < PT->numChildren; i++) {
        printParseTreeInorder(PT->children[i], fout);
    }
}

void printParseTree(parseTree PT, char *outfile) {
    FILE *fout = fopen(outfile, "w");
    if (!fout) { printf("Error opening output file for parse tree.\n"); return; }
    
    fprintf(fout, "%-20s %-10s %-20s %-15s %-25s %-10s %-25s\n", 
            "Lexeme", "LineNo", "TokenName", "ValueIfNumber", "ParentNodeSymbol", "IsLeaf(yes/no)", "NodeSymbol");
    printParseTreeInorder(PT, fout);
    
    fclose(fout);
}

void freeParseTree(parseTree PT) {
    if (!PT) return;
    for(int i=0; i<PT->numChildren; i++) {
        freeParseTree(PT->children[i]);
    }
    free(PT);
}
