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

#define LEFT      dest->left_child
#define RIGHT     dest->right_child
#define LEFTLEFT  dest->left_child->left_child
#define LEFTRIGHT dest->left_child->right_child

static int removeConst(Node *node);


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
            
            tree->size ++;

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

    switch (sourse->node_type & (IS_COS - 1))
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
    
            break;
        case IS_FUNC:
            switch (sourse->node_type & (~(IS_COS - 1)))
            {
                case IS_COS:
                    diffCos(dest, sourse);
                    break;
                case IS_SIN:
                    diffSin(dest, sourse);
                    break;
                case IS_LN:
                    diffLn(dest, sourse);
                    break;
            }
            
            break;
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

        treeCpy(dest->left_child, sourse->left_child);
    }
    if (sourse->right_child)
    {
        dest->right_child = (Node *) calloc(1, sizeof(Node));

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

    LEFT  = (Node *) calloc(1, sizeof(Node));
    RIGHT = (Node *) calloc(1, sizeof(Node));
    
    LEFT->node_type    |= IS_OPERATOR;
    LEFT->value.symbol  = '*';
    RIGHT->node_type   |= IS_OPERATOR;
    RIGHT->value.symbol = '*';

    LEFT->left_child   = (Node *) calloc(1, sizeof(Node));
    LEFT->right_child  = (Node *) calloc(1, sizeof(Node));
    RIGHT->left_child  = (Node *) calloc(1, sizeof(Node));
    RIGHT->right_child = (Node *) calloc(1, sizeof(Node));

    diffNode(LEFT->left_child, sourse->left_child);
    treeCpy(LEFT->right_child, sourse->right_child);

    diffNode(RIGHT->right_child, sourse->right_child);
    treeCpy(RIGHT->left_child, sourse->left_child);

    return 0;
}


int diffAdd(Node *dest, Node *sourse)
{
    assert(dest);
    assert(sourse);

    dest->node_type   |= IS_OPERATOR;
    dest->value.symbol = '+';

    LEFT  = (Node *) calloc(1, sizeof(Node));
    RIGHT = (Node *) calloc(1, sizeof(Node));

    diffNode(dest->left_child, sourse->left_child);
    diffNode(dest->right_child, sourse->right_child);

    return 0;
}


int diffSub(Node *dest, Node *sourse)
{
    assert(dest);
    assert(sourse);

    dest->node_type   |= IS_OPERATOR;
    dest->value.symbol = '-';

    LEFT  = (Node *) calloc(1, sizeof(Node));
    RIGHT = (Node *) calloc(1, sizeof(Node));

    diffNode(LEFT, sourse->left_child);
    diffNode(RIGHT, sourse->right_child);

    return 0;    
}


int diffDiv(Node *dest, Node *sourse)
{
    assert(dest);
    assert(sourse);

    dest->node_type     = IS_OPERATOR;
    dest->value.symbol |= '/';

    LEFT  = (Node *) calloc(1, sizeof(Node));
    RIGHT = (Node *) calloc(1, sizeof(Node));

    LEFT->node_type    |= IS_OPERATOR;
    LEFT->value.symbol  = '-';
    RIGHT->node_type   |= IS_OPERATOR;
    RIGHT->value.symbol = '*';

    RIGHT->left_child  = (Node *) calloc(1, sizeof(Node));
    RIGHT->right_child = (Node *) calloc(1, sizeof(Node));

    treeCpy(RIGHT->left_child,  sourse->right_child);
    treeCpy(RIGHT->right_child, sourse->right_child);
    

    LEFTLEFT  = (Node *) calloc(1, sizeof(Node));
    LEFTRIGHT = (Node *) calloc(1, sizeof(Node));

    LEFTLEFT->node_type    |= IS_OPERATOR;
    LEFTLEFT->value.symbol  = '*';
    LEFTRIGHT->node_type   |= IS_OPERATOR;
    LEFTRIGHT->value.symbol = '*';

    LEFTLEFT->left_child   = (Node *) calloc(1, sizeof(Node));
    LEFTLEFT->right_child  = (Node *) calloc(1, sizeof(Node));
    LEFTRIGHT->left_child  = (Node *) calloc(1, sizeof(Node));
    LEFTRIGHT->right_child = (Node *) calloc(1, sizeof(Node));

    diffNode(LEFTLEFT->left_child, sourse->left_child);
    treeCpy(LEFTLEFT->right_child, sourse->right_child);

    diffNode(LEFTRIGHT->left_child, sourse->right_child);
    treeCpy(LEFTRIGHT->right_child, sourse->left_child);

    return 0;
}


int diffCos(Node *dest, Node *sourse)
{
    assert(dest);
    assert(sourse);

    dest->node_type   |= IS_OPERATOR;
    dest->value.symbol = '*';

    LEFT  = (Node *) calloc(1, sizeof(Node));
    RIGHT = (Node *) calloc(1, sizeof(Node));

    LEFT->node_type    |= IS_OPERATOR;
    LEFT->value.symbol  = '*';
    RIGHT->node_type   |= IS_NUMBER;
    RIGHT->value.number = -1;

    LEFTLEFT  = (Node *) calloc(1, sizeof(Node));
    LEFTRIGHT = (Node *) calloc(1, sizeof(Node));

    LEFTLEFT->node_type |= (IS_FUNC | IS_SIN);
    LEFTLEFT->value.func = "sin";
    
    treeCpy(LEFTLEFT->left_child, sourse->left_child);
    diffNode(LEFT->right_child, sourse->left_child);

    return 0;
}


int diffSin(Node *dest, Node *sourse)
{
    assert(dest);
    assert(sourse);

    dest->node_type   |= IS_OPERATOR;
    dest->value.symbol = '*';

    LEFT  = (Node *) calloc(1, sizeof(Node));
    RIGHT = (Node *) calloc(1, sizeof(Node));

    LEFT->node_type |= IS_FUNC;
    LEFT->value.func = "cos";

    LEFTLEFT = (Node *) calloc(1, sizeof(Node));

    diffNode(RIGHT, sourse->left_child);
    treeCpy(LEFTLEFT, sourse->left_child);

    return 0;
}


int diffLn(Node *dest, Node *sourse)
{
    assert(dest);
    assert(sourse);

    dest->node_type   |= IS_OPERATOR;
    dest->value.symbol = '/';

    dest->left_child  = (Node *) calloc(1, sizeof(Node));
    dest->right_child = (Node *) calloc(1, sizeof(Node));

    diffNode(LEFT, sourse->left_child);
    treeCpy(RIGHT, sourse->left_child);

    return 0;
}


int diffPow(Node *dest, Node *sourse)
{
    assert(dest);
    assert(sourse);

    dest->node_type   |= IS_OPERATOR;
    dest->value.symbol = '*';

    LEFT  = (Node *) calloc(1, sizeof(Node));
    RIGHT = (Node *) calloc(1, sizeof(Node));

    diffNode(RIGHT, sourse->left_child);

    LEFT->node_type   |= IS_OPERATOR;
    LEFT->value.symbol = '*';
 
    LEFTLEFT  = (Node *) calloc(1, sizeof(Node));
    LEFTRIGHT = (Node *) calloc(1, sizeof(Node));

    treeCpy(LEFTLEFT, sourse->right_child);

    LEFTRIGHT->node_type    = IS_OPERATOR;
    LEFTRIGHT->value.symbol = '^';

    LEFTRIGHT->left_child  = (Node *) calloc(1, sizeof(Node));
    LEFTRIGHT->right_child = (Node *) calloc(1, sizeof(Node));

    treeCpy(LEFTRIGHT->left_child, sourse->left_child);

    #define LEFTRIGHT2 dest->left_child->right_child->right_child

    LEFTRIGHT2->node_type    = IS_OPERATOR;
    LEFTRIGHT2->value.symbol = '-';

    LEFTRIGHT2->left_child  = (Node *) calloc(1, sizeof(Node));
    LEFTRIGHT2->right_child = (Node *) calloc(1, sizeof(Node));

    LEFTRIGHT2->right_child->node_type   |= IS_NUMBER;
    LEFTRIGHT2->right_child->value.number = 1;

    treeCpy(LEFTRIGHT2->left_child, sourse->right_child);

    #undef LEFTRIGHT2

    return 0;
}


int removeConstant(Tree *tree)
{
    assert(tree);
    if (tree->status & DESTRUCTED_TREE)
    {
        printf("!!! ERROR Can't work with destructed tree !!!\n");
        return -1;
    }

    int is_removed = 0;

    is_removed = removeConst(&(tree->root));

    return is_removed;
}


static int removeConst(Node *node)
{
    assert(node);

    int is_removed = 0;

    if (node->left_child->node_type & IS_OPERATOR)
    {
        is_removed = removeConst(node->left_child);
    }
    if (node->right_child->node_type & IS_OPERATOR)
    {
        is_removed = removeConst(node->right_child);
    }

    if ((node->left_child->node_type & IS_NUMBER) && (node->right_child->node_type & IS_NUMBER))
    {
        node->node_type = IS_NUMBER;

        int left = node->left_child->value.number;
        int right = node->right_child->value.number;

        switch (node->value.symbol)
        {
            case '+':
                node->value.number = left + right; 
                break;
            case '-':
                node->value.number = left - right;
                break;
            case '*':
                node->value.number = left * right;
                break;
            case '/':
                node->value.number = left / right;
                break;
            case '^':
                node->value.number = pow(left, right);
                break;
        }

        free(node->left_child);
        free(node->right_child);

        node->left_child  = nullptr;
        node->right_child = nullptr;

        is_removed = 1;
    }

    return is_removed;
}


int cutTree(Tree *tree)
{
    assert(tree);
    if (tree->status & DESTRUCTED_TREE)
    {
        printf("!!! ERROR Can't work with destructed tree !!!\n");
        return -1;
    }

    int is_cutted = 0;

    cutNode(&(tree->root));

    return is_cutted;
}


static int cutNode(Node *node)
{
    assert(node);

    int is_cutted = 0;

    if (node->left_child->node_type & IS_OPERATOR)
    {
        is_cutted = cutNode(node->left_child);
    }
    if (node->right_child->node_type & IS_OPERATOR)
    {
        is_cutted = cutNode(node->right_child);
    }

    switch (node->value.symbol)
    {
        case '+':
            is_cutted = !cutPlusMinus(node);
        case '-':
            is_cutted = !cutPlusMinus(node);
        case '*'
            
    }
}


int cutPlusMinus(Node *node)
{
    assert(node);

    Node *left  = node->left_child;
    Node *right = node->right_child;

    if (left->node_type & IS_NUMBER)
    {
        if (left->value.number == 0)
        {
            if (node->parent->left_child == node)
            {
                node->parent->left_child = right;
            }
            else
            {
                node->parent->right_child = right;
            
            free(left);
            free (node);
            return 0;
        }
    }
    else if (right->node_type & IS_NUMBER)
    {
        if (right->value.number == 0)
        {
            if (node->parent->left_child == node)
            {
                node->parent->left_child = left;
            }
            else
            {
                node->parent->right_child = left;
            }

            free(right);
            free(node);
            return 0;
        }
    }

    return -1;
}