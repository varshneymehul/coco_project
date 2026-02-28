#include "parser.h"
#include "lexer.h"

char grammar_sym_table[MAX_SYMBOLS][MAX_SYMBOL_LENGTH];
uint16_t symbol_count = 0;

Grammar grammar[MAX_GRAMMAR_LENGTH];
uint16_t grammar_count = 0;

//////////////////////          Helper Functions            ///////////////////////////////

void initializeTerminals() {
    for (int i = 0; i <= TK_EOF; i++) {
        strcpy(grammar_sym_table[symbol_count++], terminalNames[i]);
    }
}

uint16_t get_symbol_id(char *symbol) {
    for (uint16_t i = 0; i < symbol_count; i++)
    {
        if (strcmp(grammar_sym_table[i], symbol) == 0)
            return i;
    }

    strcpy(grammar_sym_table[symbol_count], symbol);
    symbol_count++;
    return symbol_count - 1;
}

bool is_terminal(uint16_t symbol_id) {
    return (grammar_sym_table[symbol_id][0] != '<');
}

bool is_nonterminal(uint16_t symbol_id) {
    return (grammar_sym_table[symbol_id][0] == '<');
}

bool is_in_set(const SymbolSet *set, uint16_t symbol) {
        // Check if a symbol is in a set
    for (uint16_t i = 0; i < set->count; i++) {
        if (set->symbols[i] == symbol)
            return true;
    }
    return false;
}

bool add_to_set(SymbolSet *set, uint16_t symbol) {
    // Add symbol to set (returns true if added, false if already present)
    if (!is_in_set(set, symbol)) {
        if (set->count < MAX_SYMBOLS) {
            set->symbols[set->count++] = symbol;
            return true;
        }
    }
    return false;
}

bool merge_sets(SymbolSet *dest, const SymbolSet *src) {
    // Merge set src into dest (returns true if dest changed)
    bool changed = false;
    for (uint16_t i = 0; i < src->count; i++) {
        if (add_to_set(dest, src->symbols[i]))
            changed = true;
    }
    return changed;
}

void compute_first_sets(FirstFollowSets *ff_sets) {
    // Initialize FIRST sets for terminals and epsilon
    for (uint16_t r = 0; r < grammar_count; r++) {
        for (uint16_t j = 0; j < grammar[r].rhs_length; j++) {
            uint16_t sym = grammar[r].rhs[j];
            if (is_terminal(sym) && sym != EPS) {
                add_to_set(&ff_sets->first_sets[sym], sym);
            }
        }
    }
    add_to_set(&ff_sets->first_sets[EPS], EPS);

    bool changed = true;
    while (changed) {
        changed = false;

        for (uint16_t r = 0; r < grammar_count; r++) {
            uint16_t lhs = grammar[r].lhs;
            uint16_t rhs_len = grammar[r].rhs_length;

            if (rhs_len == 0) continue;

            bool all_nullable = true;
        
            for (uint16_t i = 0; i < rhs_len; i++) {
                uint16_t rhs_symbol = grammar[r].rhs[i];

                for (uint16_t j = 0; j < ff_sets->first_sets[rhs_symbol].count; j++) {
                    uint16_t sym = ff_sets->first_sets[rhs_symbol].symbols[j];
                    if (sym != EPS) {
                        if (add_to_set(&ff_sets->first_sets[lhs], sym))
                            changed = true;
                    }
                }
               
                if (!is_in_set(&ff_sets->first_sets[rhs_symbol], EPS)) {
                    all_nullable = false;
                    break;
                }
            }
    
            if (all_nullable) {
                if (add_to_set(&ff_sets->first_sets[lhs], EPS))
                    changed = true;
            }
        }
    }
}

void compute_follow_sets(FirstFollowSets *ff_sets, uint16_t start_symbol) {
    // Start symbol gets $ in its FOLLOW set
    add_to_set(&ff_sets->follow_sets[start_symbol], EOF);

    bool changed = true;
    while (changed) {
        changed = false;

        for (uint16_t r = 0; r < grammar_count; r++) {
            uint16_t rhs_len = grammar[r].rhs_length;

            for (uint16_t i = 0; i < rhs_len; i++) {
                uint16_t symbol = grammar[r].rhs[i];
     
                if (!is_nonterminal(symbol))
                    continue;
           
                bool all_have_epsilon = true;
                for (uint16_t j = i + 1; j < rhs_len; j++) {
                    uint16_t beta_symbol = grammar[r].rhs[j];

                    for (uint16_t k = 0; k < ff_sets->first_sets[beta_symbol].count; k++) {
                        uint16_t first_sym = ff_sets->first_sets[beta_symbol].symbols[k];
                        if (first_sym != EPS) {
                            if (add_to_set(&ff_sets->follow_sets[symbol], first_sym))
                                changed = true;
                        }
                    }
              
                    if (!is_in_set(&ff_sets->first_sets[beta_symbol], EPS)) {
                        all_have_epsilon = false;
                        break;
                    }
                }

                if (all_have_epsilon) {
                    uint16_t lhs = grammar[r].lhs;
                    if (merge_sets(&ff_sets->follow_sets[symbol], &ff_sets->follow_sets[lhs]))
                        changed = true;
                }
            }
        }
    }
}

static void compute_first_of_rhs(const uint16_t rhs[], uint16_t rhs_length, const FirstFollowSets *ff_sets, SymbolSet *out_first) {
    out_first->count = 0;
    if (rhs_length == 0) {
        add_to_set(out_first, EPS);
        return;
    }

    bool all_nullable = true;
    for (uint16_t i = 0; i < rhs_length; i++) {
        uint16_t sym = rhs[i];

        for (uint16_t j = 0; j < ff_sets->first_sets[sym].count; j++) {
            uint16_t first_sym = ff_sets->first_sets[sym].symbols[j];
            if (first_sym != EPS) {
                add_to_set(out_first, first_sym);
            }
        }

        if (!is_in_set(&ff_sets->first_sets[sym], EPS)) {
            all_nullable = false;
            break;
        }
    }

    if (all_nullable) {
        add_to_set(out_first, EPS);
    }
}

ParseTreeNode* createNode(uint16_t symbol) {
    ParseTreeNode *node = (ParseTreeNode*)malloc(sizeof(ParseTreeNode));
    node->symbol = symbol;
    node->lexeme[0] = '\0';
    node->line_no = -1;
    node->parent = NULL;
    node->child_count = 0;
    return node;
}

void push(NodeStack *s, ParseTreeNode *node) {
    s->data[++(s->top)] = node;
}

ParseTreeNode* pop(NodeStack *s) {
    return s->data[(s->top)--];
}

ParseTreeNode* peek(NodeStack *s) {
    return s->data[s->top];
}

bool isEmpty(NodeStack *s) {
    return s->top == -1;
}

//////////////////////          Implementation Functions            ///////////////////////////////

void readGrammar(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("Error opening grammar file\n");
        exit(1);
    }

    initializeTerminals();

    char line[256];

    while (fgets(line, sizeof(line), fp)) {

        line[strcspn(line, "\n")] = '\0';

        if (strlen(line) == 0 || line[0] == '#')                        //Lines beginning with # are considered to be comments
            continue;

        Grammar g;
        g.rhs_length = 0;

        char *token = strtok(line, " ");
        
        if (token[0] == '<') {
            char lhs[MAX_SYMBOL_LENGTH];
            strcpy(lhs, token);
            g.lhs = get_symbol_id(lhs);

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
                g.rhs[g.rhs_length++] = EPS;
            }
            else {
                g.rhs[g.rhs_length++] = get_symbol_id(token);
            }
        }
        grammar[grammar_count++] = g;
    }
    fclose(fp);
}

FirstFollowSets* ComputeFirstAndFollowSets(const char *filename) {
    readGrammar(filename);
    uint16_t start_symbol = grammar[0].lhs;
    FirstFollowSets *ff_sets = (FirstFollowSets *)malloc(sizeof(FirstFollowSets));
    ff_sets->num_symbols = symbol_count;
    ff_sets->first_sets = (SymbolSet *)calloc(symbol_count, sizeof(SymbolSet));
    ff_sets->follow_sets = (SymbolSet *)calloc(symbol_count, sizeof(SymbolSet));

    compute_first_sets(ff_sets);
    compute_follow_sets(ff_sets, start_symbol);

    return ff_sets;
}



void createParseTable(int parse_table[MAX_SYMBOLS][MAX_SYMBOLS], const FirstFollowSets *ff_sets) {
    for (uint16_t i = 0; i < MAX_SYMBOLS; i++) {
        for (uint16_t j = 0; j < MAX_SYMBOLS; j++) {
            parse_table[i][j] = -1;
        }
    }

    for (uint16_t r = 0; r < grammar_count; r++) {
        uint16_t lhs = grammar[r].lhs;
        SymbolSet first_rhs;
        compute_first_of_rhs(grammar[r].rhs, grammar[r].rhs_length, ff_sets, &first_rhs);

        for (uint16_t i = 0; i < first_rhs.count; i++) {
            uint16_t terminal = first_rhs.symbols[i];
            if (terminal != EPS) {
                parse_table[lhs][terminal] = r;
            }
        }

        if (is_in_set(&first_rhs, EPS)) {
            const SymbolSet *follow_lhs = &ff_sets->follow_sets[lhs];
            for (uint16_t i = 0; i < follow_lhs->count; i++) {
                uint16_t terminal = follow_lhs->symbols[i];
                parse_table[lhs][terminal] = r;
            }
        }
    }
}


ParseTree parseInputSourceCode(char *testcaseFile, int parse_table[MAX_SYMBOLS][MAX_SYMBOLS]) {

    FILE *fp = fopen(testcaseFile, "r");
    if (!fp) {
        printf("Error opening source file\n");
        exit(1);
    }

    NodeStack stack;
    stack.top = -1;

    uint16_t start_symbol = grammar[0].lhs;
    ParseTree tree;
    tree.root = createNode(start_symbol);

    ParseTreeNode *dollar_node = createNode(TK_EOF);
    push(&stack, dollar_node);
    push(&stack, tree.root);

    twinBuffer B = initBuffer(fp);
    tokenInfo lookahead = getNextToken(B);

    int syntax_errors = 0;

    while (!isEmpty(&stack)) {

        ParseTreeNode* top_node = peek(&stack);
        uint16_t X = top_node->symbol;

        if (is_terminal(X) || X == TK_EOF) {
            if (X == lookahead.type) {
                pop(&stack);

                strcpy(top_node->lexeme, lookahead.lexeme);
                top_node->line_no = lookahead.lineNo;

                lookahead = getNextToken(B);
            }
            else {
                printf("Syntax Error at line %d: Unexpected token %s\n", lookahead.lineNo, lookahead.lexeme);

                syntax_errors++;

                lookahead = getNextToken(B);
            }

            continue;
        }

        int rule_index = parse_table[X][lookahead.type];
        if (rule_index == -1) {
            printf("Syntax Error at line %d near token %s\n", lookahead.lineNo, lookahead.lexeme);
            lookahead = getNextToken(B);
            syntax_errors++;
            if (lookahead.type == TK_EOF)
                break;
            continue;
        }

        pop(&stack);
        Grammar rule = grammar[rule_index];

        if (rule.rhs_length == 1 && rule.rhs[0] == EPS) {
            continue;
        } 

        ParseTreeNode *children[MAX_RHS_LENGTH];

        for (int i = 0; i < rule.rhs_length; i++) {
            children[i] = createNode(rule.rhs[i]);
            children[i]->parent = top_node;
            top_node->children[top_node->child_count++] = children[i];
        }

        for (int i = rule.rhs_length - 1; i >= 0; i--) {
            if (rule.rhs[i] != EPS)
                push(&stack, children[i]);
        }
    }

    fclose(fp);
    closeBuffer(B);

    if (syntax_errors == 0) {
        printf("Input source code is syntactically correct\n");
    }

    return tree;

}

void printNode(ParseTreeNode *node, FILE *out) {

    if (!node) return;

    fprintf(out, "Symbol: %d", node->symbol);

    if (node->child_count == 0) {   // terminal
        fprintf(out, "  Lexeme: %s  Line: %d",
                node->lexeme,
                node->line_no);
    }

    fprintf(out, "\n");

    for (int i = 0; i < node->child_count; i++) {
        printNode(node->children[i], out);
    }
}

void printParseTree(struct ParseTree PT, char *outfile) {

    FILE *out = fopen(outfile, "w");
    if (!out) {
        printf("Error opening output file\n");
        return;
    }

    printNode(PT.root, out);

    fclose(out);
}

void free_first_follow_sets(FirstFollowSets *ff_sets) {
    // Free allocated memory
    if (ff_sets) {
        free(ff_sets->first_sets);
        free(ff_sets->follow_sets);
        free(ff_sets);
    }
}