#ifndef DIFF_H
    #define DIFF_H

    const int IS_OPERATOR = 1 << 0;
    const int IS_NUMBER   = 1 << 1;
    const int IS_VARIABLE = 1 << 2;
    const int IS_FUNC     = 1 << 3;

    const int IS_COS      = 1 << 10;
    const int IS_SIN      = 1 << 11;
    const int IS_LN       = 1 << 12;

    int readFormula(FILE *sourse, Tree *tree);
    int readArg(Node *node, char *text);
    int diffur(Tree *sourse, Tree *result);
    int optimiz(Tree *tree);

    int diffNode(Node *dest, Node *sourse);
    int treeCpy (Node *dest, Node *sourse);
    int diffMul (Node *dest, Node *sourse);
    int diffAdd (Node *dest, Node *sourse);
    int diffSub (Node *dest, Node *sourse);
    int diffDiv (Node *dest, Node *sourse);
    int diffCos (Node *dest, Node *sourse);
    int diffSin (Node *dest, Node *sourse);
    int diffLn  (Node *dest, Node *sourse);
    int diffPow (Node *dest, Node *sourse);

    int removeConstant(Tree *tree);
    int cutTree(Tree *tree);


#endif