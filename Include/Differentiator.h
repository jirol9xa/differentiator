#ifndef DIFF_H
    #define DIFF_H

    const int IS_OPERATOR = 1 << 0;
    const int IS_NUMBER   = 1 << 1;
    const int IS_VARIABLE = 1 << 2;
    const int IS_FUNC     = 1 << 3;
    const int IS_COS      = 1 << 4;
    const int IS_SIN      = 1 << 5;
    const int IS_LN       = 1 << 6;

    int readFormula(FILE *sourse, Tree *tree);
    int readArg(Node *node, char *text);
    int diffur(Tree *sourse, Tree *result);
    int optimiz(Tree *tree);

    int diffNode(Node *dest, Node *sourse);
    int treeCpy (Node *dest, Node *sourse);

    int removeConstant(Tree *tree);
    int cutTree(Tree *tree);

    int beginTex();
    int texOrigin(Tree *tree);
    int texDump(Tree *tree);
    int finishTex();

    double calcDiff(Tree *tree, double x);

    
    
    #define LEFT(arg)      arg->left_child
    #define RIGHT(arg)     arg->right_child
    #define LEFTLEFT(arg)  arg->left_child->left_child
    #define LEFTRIGHT(arg) arg->left_child->right_child

#endif