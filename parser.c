#include "parser.h"

typedef struct {
    uint16_t symbols[MAX_SYMBOLS];
    uint16_t count;
} SymbolSet;


typedef struct {
    SymbolSet *first_sets;  // first_sets[i] = FIRST set for symbol i
    SymbolSet *follow_sets; // follow_sets[i] = FOLLOW set for symbol i
    uint16_t num_symbols;
} FirstFollowSets;


typedef struct
{
    uint16_t lhs;
    uint16_t rhs[MAX_RHS_LENGTH];
    uint16_t rhs_length;

} Grammar;


struct Table
{

};


struct ParseTree
{

};


////////////////////////////////////////////////////////////////////////*Helper Functions*//////////////////////////////////////////////////////////////////////////////////////////////

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


bool is_terminal(uint16_t symbol_id) {
    // Check if symbol is a terminal (not enclosed in <>)
    // For testing: terminals have IDs >= 100
    return symbol_id >= 100 || symbol_id == EPSILON;
}


bool is_nonterminal(uint16_t symbol_id) {
    // Check if symbol is a non-terminal (was enclosed in <>)
    return symbol_id < 100 && symbol_id != EPSILON;
}


void free_first_follow_sets(FirstFollowSets *ff_sets) {
    // Free allocated memory
    if (ff_sets) {
        free(ff_sets->first_sets);
        free(ff_sets->follow_sets);
        free(ff_sets);
    }
}


////////////////////////////////////////////////////////////////////////////*Core Functions*//////////////////////////////////////////////////////////////////////////////////////////////


void compute_first_sets(FirstFollowSets *ff_sets, const Grammar *grammar, uint16_t grammar_count) {
    // Initialize FIRST sets for terminals and epsilon
    for (uint16_t r = 0; r < grammar_count; r++) {
        for (uint16_t j = 0; j < grammar[r].rhs_length; j++) {
            uint16_t sym = grammar[r].rhs[j];
            if (is_terminal(sym) && sym != EPSILON) {
                add_to_set(&ff_sets->first_sets[sym], sym);
            }
        }
    }
    add_to_set(&ff_sets->first_sets[EPSILON], EPSILON);

    // Iterate until no changes occur
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
                    if (sym != EPSILON) {
                        if (add_to_set(&ff_sets->first_sets[lhs], sym))
                            changed = true;
                    }
                }

               
                if (!is_in_set(&ff_sets->first_sets[rhs_symbol], EPSILON)) {
                    all_nullable = false;
                    break;
                }
            }
            
    
            if (all_nullable) {
                if (add_to_set(&ff_sets->first_sets[lhs], EPSILON))
                    changed = true;
            }
        }
    }
}


void compute_follow_sets(FirstFollowSets *ff_sets, const Grammar *grammar, uint16_t grammar_count, uint16_t start_symbol) {
    // Start symbol gets $ in its FOLLOW set
    add_to_set(&ff_sets->follow_sets[start_symbol], END_OF_INPUT);


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
                        if (first_sym != EPSILON) {
                            if (add_to_set(&ff_sets->follow_sets[symbol], first_sym))
                                changed = true;
                        }
                    }

              
                    if (!is_in_set(&ff_sets->first_sets[beta_symbol], EPSILON)) {
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


////////////////////////////////////////////////////////////////////////////////*Driver Function*//////////////////////////////////////////////////////////////////////////////////////////////

FirstFollowSets* ComputeFirstAndFollowSets(const char *filename, const Grammar *grammar, uint16_t grammar_count, uint16_t start_symbol, uint16_t num_symbols) {
    // Initialize and compute both FIRST and FOLLOW sets
    readGrammar(filename);
    FirstFollowSets *ff_sets = (FirstFollowSets *)malloc(sizeof(FirstFollowSets));
    ff_sets->num_symbols = num_symbols;
    ff_sets->first_sets = (SymbolSet *)calloc(num_symbols, sizeof(SymbolSet));
    ff_sets->follow_sets = (SymbolSet *)calloc(num_symbols, sizeof(SymbolSet));

    compute_first_sets(ff_sets, grammar, grammar_count);
    compute_follow_sets(ff_sets, grammar, grammar_count, start_symbol);

    return ff_sets;
}
/////////////////////////////////////////////////////////////////////////////*Utility Functions*//////////////////////////////////////////////////////////////////////////////////////////////
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



// Create LL(1) parse table:
// parse_table[non_terminal][terminal] = production index in grammar[], else -1
void createparsetable(int parse_table[MAX_NON_TERMINALS][MAX_TERMINALS], const Grammar *grammar, uint16_t grammar_count, const FirstFollowSets *ff_sets) {
    for (uint16_t i = 0; i < MAX_NON_TERMINALS; i++) {
        for (uint16_t j = 0; j < MAX_TERMINALS; j++) {
            parse_table[i][j] = -1;
        }
    }

    for (uint16_t r = 0; r < grammar_count; r++) {
        uint16_t lhs = grammar[r].lhs;
        SymbolSet first_rhs;
        compute_first_of_rhs(grammar[r].rhs, grammar[r].rhs_length, ff_sets, &first_rhs);

        for (uint16_t i = 0; i < first_rhs.count; i++) {
            uint16_t terminal = first_rhs.symbols[i];
            if (terminal != EPSILON) {
                parse_table[lhs][terminal] = r;
            }
        }

        if (is_in_set(&first_rhs, EPSILON)) {
            const SymbolSet *follow_lhs = &ff_sets->follow_sets[lhs];
            for (uint16_t i = 0; i < follow_lhs->count; i++) {
                uint16_t terminal = follow_lhs->symbols[i];
                parse_table[lhs][terminal] = r;
            }
        }
    }
}
/////////////////////////////////////////////Helper Function //////////////////////////////////////////////////////////////////////////

static void compute_first_of_rhs(const uint16_t rhs[], uint16_t rhs_length, const FirstFollowSets *ff_sets, SymbolSet *out_first) {
    out_first->count = 0;
    if (rhs_length == 0) {
        add_to_set(out_first, EPSILON);
        return;
    }

    bool all_nullable = true;
    for (uint16_t i = 0; i < rhs_length; i++) {
        uint16_t sym = rhs[i];

        for (uint16_t j = 0; j < ff_sets->first_sets[sym].count; j++) {
            uint16_t first_sym = ff_sets->first_sets[sym].symbols[j];
            if (first_sym != EPSILON) {
                add_to_set(out_first, first_sym);
            }
        }

        if (!is_in_set(&ff_sets->first_sets[sym], EPSILON)) {
            all_nullable = false;
            break;
        }
    }

    if (all_nullable) {
        add_to_set(out_first, EPSILON);
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
