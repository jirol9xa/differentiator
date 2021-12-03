#ifndef RECURS_DES
    #define RECURS_DES

    struct Tokens
    {
        Node **array;
        int capacity;
        int size;
    };


    Tree *constructTree(Tree *tree, char *string);
    int tokensCtor(Tokens *tokens);
    Node *number();
    Node *identific();
    Node *oper();

#endif