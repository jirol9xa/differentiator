#ifndef DIFF_H
    #define DIFF_H

    const int IS_SYMBOL   = 1 << 0;
    const int IS_NUMBER   = 1 << 1;
    const int IS_VARIABLE = 1 << 2;
    const int IS_FUNC     = 1 << 3;

    const int IS_COS      = 1 << 10;
    const int IS_SIN      = 1 << 11;
    const int IS_LN       = 1 << 12;

    int readFormula(FILE *sourse, Tree *tree);
    int readArg(Node *node, char *text);
    

#endif