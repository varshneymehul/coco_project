#include "parser.h"


struct FirstAndFollow
{

};

struct Table
{

};

struct ParseTree
{

};

uint16_t get_symbol_id(char *symbol) {
    for (uint16_t i = 0; i < symbol_count; i++)
    {
        if (strcmp(symbol_table[i], symbol) == 0)
            return i;
    }

    strcpy(symbol_table[symbol_count], symbol);
    symbol_count++;
    return symbol_count - 1;
}

void readGrammar(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("Error opening grammar file\n");
        exit(1);
    }

    char line[256];

    while (fgets(line, sizeof(line), fp)) {

        line[strcspn(line, "\n")] = '\0';

        if (strlen(line) == 0 || line[0] == '#')
            continue;

        Grammar g;
        g.rhs_length = 0;

        char *token = strtok(line, " ");
        
        if (token[0] == '<') {
            char clean[MAX_SYMBOL_LENGTH];
            strcpy(clean, token[0]);
            g.lhs = get_symbol_id(clean);

        } else {
            printf("Invalid LHS format\n");
            exit(1);

        }

        token = strtok(NULL, " ");
        if (!token || strcmp(token, "===>") != 0) {
            printf("Expected ===>\n");
            exit(1);
        }

        while ((token = strtok(NULL, " ")) != NULL) {
            if (strcmp(token, "epsilon") == 0) {
                g.rhs[g.rhs_length++] = EPSILON;
            }
            else {
                // Terminal (ALL CAPS)
                g.rhs[g.rhs_length++] = get_symbol_id(token);
            }
        }
        grammar[grammar_count++] = g;
    }
    fclose(fp);
}

struct FirstAndFollow ComputeFirstAndFollowSets (const char *filename) {
   readGrammar(filename);
}

void createParseTable(struct FirstAndFollow F, struct Table T) {
    /*
        This function needs to construct the predictive table. 
        Challenges:
            - The structure of the table would be related to sturcture used for the First and Follow sets and the Grammar.
    */
}

struct ParseTree parseInputSourceCode(char *testcaseFile, struct Table T) {
    /*
        This funciton should be relatively straight forward. The challenge here would be maintaining the n-ary tree for the parse tree.
    */
}

printParseTree(struct ParseTree PT, char *outfile) {
    /*
        Most straight forward function of them all. Standard tree DSA.
    */
}
