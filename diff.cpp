#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <cmath>
#include "include/logsLib.h"
#include "include/textLib.h"
#include "Tree/Tree.h"
#include "diff.h"

extern Tree rslt_tree;


#define NEW_NUMBER_NODE(arg)                                                                 \
    dest->node_type    = IS_NUMBER;                                                          \
    dest->value.number = arg;


int readFormula(FILE *sourse, Tree *tree)
{
    assert(sourse);
    assert(tree);
    if (tree->status & DESTRUCTED_TREE)
    {
        printf("!!! ERROR Can't work with destructed tree\n");
        return -1;
    }

    long int file_length = 0;
    fileLength(&file_length, sourse);

    char *text = (char *) calloc(file_length + 1, sizeof(char));
    long int num_symb = fread(text, sizeof(char), file_length, sourse);

    text[num_symb] = '\0';

    assert(text);

    Node *cur_node = &(tree->root);

    for (int i = 0; i < num_symb && text[i] != '\n'; i++)
    {
        i += skipSpace(text + i);

        if (text[i] == '(')
        {
            PRINT_LINE;
            if (tree->status & EMPTY_TREE)
            {
                PRINT_LINE;
                tree->status &= (~EMPTY_TREE);
                continue;
            }
            
            if (cur_node->left_child == nullptr)
            {
                PRINT_LINE;
                cur_node = nodeCtor(cur_node, cur_node->left_child, 1);
            }
            else if (cur_node->right_child == nullptr)
            {
                cur_node = nodeCtor(cur_node, cur_node->right_child, 0);
            }
            else
            {
                printf("!!! ERROR Node can't has more than two children !!!\n");
                tree->status |= DESTRUCTED_TREE;
                return -1;
            }
        }
        else if (text[i] == ')')
        {            
            cur_node = cur_node->parent;
        }
        else
        {
            i += readArg(cur_node, text + i);
        }
    }
    
    free(text);
    return 0;
}


int readArg(Node *node, char *text)
{
    assert(node);
    assert(text);

    double number = 0;
    int i = 0;
    i += skipSpace(text);
    
    if (sscanf(text + i, "%lg", &number) == 1)
    {
        node->node_type    = IS_NUMBER;
        node->value.number = number;
        return i + (int) log10(number);
    }
    else if (text[i + 1] != '(' && text[i + 1] != ')' && text[i + 1] != ' ')
    {
        PRINT_LINE;
        node->node_type = IS_FUNC;
        sscanf(text + i, "%m[cosinl]", &(node->value.func));

        switch (node->value.func[0])
        {
        case 'c':
            node->node_type |= IS_COS;
            break;
        
        case 's':
            node->node_type |= IS_SIN;
            break;
        case 'l':
            node->node_type |= IS_LN;
            break;
        }


        return i + strlen(node->value.func) - 1;
    }
    else
    {
        sscanf(text + i, "%c", &(node->value.symbol));

        char symbol = node->value.symbol;

        if (symbol == '+' || symbol == '-' || symbol == '*' || symbol == '/')
        {
            node->node_type |= IS_OPERATOR;
            return i;
        }

        node->node_type |= IS_VARIABLE;
        return i;
    }
}


int diffur(Tree *sourse, Tree *result)
{
    assert(sourse);
    assert(result);
    if ((sourse->status & DESTRUCTED_TREE) || (result->status & DESTRUCTED_TREE))
    {
        printf("!!! ERROR Can't work with destucted tree !!!\n");
        return -1;
    }
    if (!(result->status & EMPTY_TREE))
    {
        printf("!!! ERROR Can't write in not empty tree !!!\n");
        return -1;
    }

    diffNode(&(result->root), &(sourse->root));

    return 0;
}


int diffNode(Node *dest, Node *sourse)
{
    assert(dest);
    assert(sourse);

    treeDump(&rslt_tree);

    switch (sourse->node_type)
    {
    case IS_VARIABLE:
        NEW_NUMBER_NODE(1);
        break;
    case IS_NUMBER:
        NEW_NUMBER_NODE(0);
    case IS_OPERATOR:
        switch (sourse->value.symbol)
        {
        case '*':
            PRINT_LINE;
            diffMul(dest, sourse);
            break;
        case '+':
            PRINT_LINE;
            diffAdd(dest, sourse);
            break;
        case '-':
            diffSub(dest, sourse);
            break;
        case '/':
            diffDiv(dest, sourse);
            break;
        }
    }

    return 0;
}


int treeCpy(Node *dest, Node *sourse)
{
    assert(dest);
    assert(sourse);
    PRINT_LINE;
    dest->node_type = sourse->node_type;
    
    switch (dest->node_type)
    {
    case IS_VARIABLE:
        dest->value.symbol = sourse->value.symbol;
        break;
    case IS_OPERATOR:
        dest->value.symbol = sourse->value.symbol;
        break;
    case IS_NUMBER:
        dest->value.number = sourse->value.number;
        break;
    case IS_FUNC:
        dest->value.func = (char *) calloc(strlen(sourse->value.func) + 1, sizeof(char));
        strcpy(dest->value.func, sourse->value.func);
        break;
    }

    if (sourse->left_child)
    {
        dest->left_child = (Node *) calloc(1, sizeof(Node));
        assert(dest->left_child);

        treeCpy(dest->left_child, sourse->left_child);
    }
    if (sourse->right_child)
    {
        dest->right_child = (Node *) calloc(1, sizeof(Node));
        assert(dest->right_child);

        treeCpy(dest->right_child, sourse->right_child);
    }

    return 0;
}


int diffMul(Node *dest, Node *sourse)
{
    assert(dest);
    assert(sourse);

    dest->node_type   |= IS_OPERATOR;
    dest->value.symbol = '+';

    #define left  dest->left_child
    #define right dest->right_child

    left  = (Node *) calloc(1, sizeof(Node));
    right = (Node *) calloc(1, sizeof(Node));
    assert(left);
    assert(right);
    printf("left = %p\nleft_child = %p\n", left, dest->left_child);
    
    left->node_type    |= IS_OPERATOR;
    left->value.symbol  = '*';
    right->node_type   |= IS_OPERATOR;
    right->value.symbol = '*';

    left->left_child   = (Node *) calloc(1, sizeof(Node));
    assert(left->left_child);
    left->right_child  = (Node *) calloc(1, sizeof(Node));
    assert(left->right_child);
    right->left_child  = (Node *) calloc(1, sizeof(Node));
    assert(right->left_child);
    right->right_child = (Node *) calloc(1, sizeof(Node));
    assert(right->right_child);

    #undef left
    #undef right

    diffNode(dest->left_child->left_child, sourse->left_child);
    treeCpy(dest->left_child->right_child, sourse->right_child);

    diffNode(dest->right_child->right_child, sourse->right_child);
    treeCpy(dest->right_child->left_child, sourse->left_child);

    return 0;
}


int diffAdd(Node *dest, Node *sourse)
{
    assert(dest);
    assert(sourse);

    dest->node_type   |= IS_OPERATOR;
    dest->value.symbol = '+';

    dest->left_child  = (Node *) calloc(1, sizeof(Node));
    assert(dest->left_child);
    dest->right_child = (Node *) calloc(1, sizeof(Node));
    assert(dest->right_child);
    PRINT_LINE;
    diffNode(dest->left_child, sourse->left_child);
    PRINT_LINE;
    diffNode(dest->right_child, sourse->right_child);

    return 0;
}


int diffSub(Node *dest, Node *sourse)
{
    assert(dest);
    assert(sourse);

    dest->node_type   |= IS_OPERATOR;
    dest->value.symbol = '-';

    dest->left_child  = (Node *) calloc(1, sizeof(Node));
    assert(dest->left_child);
    dest->right_child = (Node *) calloc(1, sizeof(Node));
    assert(dest->right_child);

    diffNode(dest->left_child, sourse->left_child);
    diffNode(dest->right_child, sourse->right_child);

    return 0;    
}


int diffDiv(Node *dest, Node *sourse)
{
    assert(dest);
    assert(sourse);

    dest->node_type     = IS_OPERATOR;
    dest->value.symbol |= '/';

    #define left  dest->left_child
    #define right dest->right_child

    left  = (Node *) calloc(1, sizeof(Node));
    assert(left);
    right = (Node *) calloc(1, sizeof(Node));
    assert(right);

    left->node_type    |= IS_OPERATOR;
    left->value.symbol  = '-';
    right->node_type   |= IS_OPERATOR;
    right->value.symbol = '*';

    right->left_child  = (Node *) calloc(1, sizeof(Node));
    assert(right->left_child);
    right->right_child = (Node *) calloc(1, sizeof(Node));
    assert(right->right_child);

    treeCpy(right->left_child,  sourse->right_child);
    treeCpy(right->right_child, sourse->right_child);

    #define leftleft  dest->left_child->left_child
    #define leftright dest->left_child->right_child

    leftleft  = (Node *) calloc(1, sizeof(Node));
    assert(leftleft);
    leftright = (Node *) calloc(1, sizeof(Node));
    assert(leftright);

    leftleft->node_type    |= IS_OPERATOR;
    leftleft->value.symbol  = '*';
    leftright->node_type   |= IS_OPERATOR;
    leftright->value.symbol = '*';

    leftleft->left_child   = (Node *) calloc(1, sizeof(Node));
    assert(leftleft->left_child);
    leftleft->right_child  = (Node *) calloc(1, sizeof(Node));
    assert(leftleft->right_child);
    leftright->left_child  = (Node *) calloc(1, sizeof(Node));
    assert(leftright->left_child);
    leftright->right_child = (Node *) calloc(1, sizeof(Node));
    assert(leftright->right_child);

    #undef leftleft
    #undef leftright

    diffNode(dest->left_child->left_child->left_child, sourse->left_child);
    treeCpy(dest->left_child->left_child->right_child, sourse->right_child);

    diffNode(dest->left_child->right_child->left_child, sourse->right_child);
    treeCpy(dest->left_child->right_child->right_child, sourse->left_child);

    return 0;
}