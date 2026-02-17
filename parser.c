struct FirstAndFollow
{

};

struct Grammar
{

};

struct Table
{

};

struct ParseTree
{

};


struct FirstAndFollow ComputeFirstAndFollowSets (struct Grammar G) {
    /*
        This function needs to take as input a Grammar and compute the FIRST and FOLLOW for each non terminal in the Grammar.
        Challenges:
            - In what format would you represent the Grammar and the First and Follow sets
            - The method used for computing the sets is also something that needs to be solved
            - Outside the driver you will need a function to load the Grammar when you first run it.
    */
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
