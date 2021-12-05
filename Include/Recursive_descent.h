#ifndef RECURS_DES
    #define RECURS_DES

    struct Tokens
    {
        Node **array;
        int capacity;
        int size;
    };


    int constructTree(Tree *tree, char *string);

#endif