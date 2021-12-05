#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <cmath>
#include "LogsLib.h"
#include "TextLib.h"
#include "Tree.h"
#include "Differentiator.h"
#include "Recursive_descent.h"


#define NEW_NUMBER_NODE(arg)                                                                      \
    dest->node_type.bytes.is_number = 1;                                                          \
    dest->value.number = arg;


static int diffMul (Node *dest, Node *sourse);
static int diffAdd (Node *dest, Node *sourse);
static int diffSub (Node *dest, Node *sourse);
static int diffDiv (Node *dest, Node *sourse);
static int diffCos (Node *dest, Node *sourse);
static int diffSin (Node *dest, Node *sourse);
static int diffLn  (Node *dest, Node *sourse);
static int diffPow (Node *dest, Node *sourse);


int readFormula(FILE *sourse, Tree *tree)
{
    assert(sourse);
    assert(tree);
    if (tree->status.destructed_tree)
    {
        printf("!!! ERROR Can't work with destructed tree\n");
        return -1;
    }

    long int file_length = 0;
    fileLength(&file_length, sourse);

    char *text = (char *) calloc(file_length + 1, sizeof(char));
    long int num_symb = fread(text, sizeof(char), file_length, sourse);

    text[num_symb] = '\0';

    constructTree(tree, text);
    treeDump(tree);
    /*Node *cur_node = tree->root;

    for (int i = 0; i < num_symb && text[i] != '\n'; i++)
    {
        i += skipSpace(text + i);

        if (text[i] == '(')
        {
            if (tree->status.empty_tree)
            {
                tree->status.empty_tree = 0;
                continue;
            }
            tree->size ++;
            if (cur_node->left_child == nullptr)
            {
                cur_node = nodeCtor(cur_node, cur_node->left_child, 1);
            }
            else if (cur_node->right_child == nullptr)
            {
                cur_node = nodeCtor(cur_node, cur_node->right_child, 0);
            }
            else
            {
                printf("!!! ERROR Node can't has more than two children !!!\n");
                tree->status.destructed_tree = 1;
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
    */

    printf("root = %p\n", tree->root);   
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
        node->node_type.bytes.is_number = 1;
        node->value.number = number;
        return i + (int) log10(number);
    }
    else if (!strchr("( )", text[i + 1]))
    {
        
        node->node_type.bytes.is_func = 1;
        sscanf(text + i, "%m[cosinl]", &(node->value.func));

        switch (node->value.func[0])
        {
        case 'c':
            node->node_type.bytes.is_cos = 1;
            break;
        case 's':
            node->node_type.bytes.is_sin = 1;
            break;
        case 'l':
            node->node_type.bytes.is_ln  = 1;
            break;
        }

        return i + strlen(node->value.func) - 1;
    }
    else
    {
        sscanf(text + i, "%c", &(node->value.symbol));

        char symbol = node->value.symbol;

        if (strchr("+-/*", symbol))
        {
            node->node_type.bytes.is_operator = 1;
            return i;
        }

        node->node_type.bytes.is_variable = 1;
        return i;
    }
}


int diffur(Tree *sourse, Tree *result)
{
    assert(sourse);
    assert(result);
    if (sourse->status.destructed_tree || result->status.destructed_tree)
    {
        printf("!!! ERROR Can't work with destucted tree !!!\n");
        return -1;
    }
    if (!result->status.empty_tree)
    {
        printf("!!! ERROR Can't write in not empty tree !!!\n");
        return -1;
    }

    diffNode(result->root, sourse->root);

    return 0;
}


int diffNode(Node *dest, Node *sourse)
{
    assert(dest);
    assert(sourse);

    switch (sourse->node_type.number & (IS_COS - 1))
    {
        case IS_VARIABLE:
            NEW_NUMBER_NODE(1);
            break;
        case IS_NUMBER:
            NEW_NUMBER_NODE(0);
            break;
        case IS_OPERATOR:
            switch (sourse->value.symbol)
            {
                case '*':
                    diffMul(dest, sourse);
                    break;
                case '+':
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
            switch (sourse->node_type.number & (~(IS_COS - 1)))
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
    
    dest->node_type = sourse->node_type;
    
    switch (dest->node_type.number)
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
        dest->left_child = nodeCtor(dest, dest->left_child, 1);

        treeCpy(dest->left_child, sourse->left_child);
    }
    if (sourse->right_child)
    {
        dest->right_child = nodeCtor(dest, dest->right_child, 0);

        treeCpy(dest->right_child, sourse->right_child);
    }

    return 0;
}


static int diffMul(Node *dest, Node *sourse)
{
    assert(dest);
    assert(sourse);

    dest->node_type.bytes.is_operator = 1;
    dest->value.symbol = '+';

    LEFT(dest)  = nodeCtor(dest, LEFT(dest), 1);
    RIGHT(dest) = nodeCtor(dest, RIGHT(dest), 0);
    
    LEFT(dest)->node_type .bytes.is_operator = 1;
    LEFT(dest)->value.symbol  = '*';
    RIGHT(dest)->node_type.bytes.is_operator = 1;
    RIGHT(dest)->value.symbol = '*';

    LEFT(dest)->left_child   = nodeCtor(LEFT(dest), LEFT(dest)->left_child, 1);
    LEFT(dest)->right_child  = nodeCtor(LEFT(dest), LEFT(dest)->right_child, 0);
    RIGHT(dest)->left_child  = nodeCtor(RIGHT(dest), RIGHT(dest)->left_child, 1);
    RIGHT(dest)->right_child = nodeCtor(RIGHT(dest), RIGHT(dest)->right_child, 0);

    diffNode(LEFT(dest)->left_child, sourse->left_child);
    treeCpy(LEFT(dest)->right_child, sourse->right_child);

    diffNode(RIGHT(dest)->right_child, sourse->right_child);
    treeCpy(RIGHT(dest)->left_child, sourse->left_child);

    return 0;
}


static int diffAdd(Node *dest, Node *sourse)
{
    assert(dest);
    assert(sourse);

    dest->node_type.bytes.is_operator = 1;
    dest->value.symbol = '+';

    LEFT(dest)  = nodeCtor(dest, LEFT(dest), 1);
    RIGHT(dest) = nodeCtor(dest, RIGHT(dest), 0);

    diffNode(LEFT(dest), sourse->left_child);
    diffNode(RIGHT(dest), sourse->right_child);

    return 0;
}


static int diffSub(Node *dest, Node *sourse)
{
    assert(dest);
    assert(sourse);

    dest->node_type.bytes.is_operator = 1;
    dest->value.symbol = '-';

    LEFT(dest)  = nodeCtor(dest, LEFT(dest), 1);
    RIGHT(dest) = nodeCtor(dest, RIGHT(dest), 0);

    diffNode(LEFT(dest), sourse->left_child);
    diffNode(RIGHT(dest), sourse->right_child);

    return 0;    
}


static int diffDiv(Node *dest, Node *sourse)
{
    assert(dest);
    assert(sourse);

    dest->node_type.bytes.is_operator = 1;
    dest->value.symbol |= '/';

    LEFT(dest)  = nodeCtor(dest, LEFT(dest), 1);
    RIGHT(dest) = nodeCtor(dest, RIGHT(dest), 0);;

    LEFT(dest)->node_type.bytes.is_operator = 1;
    LEFT(dest)->value.symbol  = '-';
    RIGHT(dest)->node_type.bytes.is_operator = 1;
    RIGHT(dest)->value.symbol = '*';

    RIGHT(dest)->left_child  = nodeCtor(RIGHT(dest), RIGHT(dest)->left_child, 1);
    RIGHT(dest)->right_child = nodeCtor(RIGHT(dest), RIGHT(dest)->right_child, 0);

    treeCpy(RIGHT(dest)->left_child,  sourse->right_child);
    treeCpy(RIGHT(dest)->right_child, sourse->right_child);

    LEFTLEFT(dest)  = nodeCtor(LEFT(dest), LEFTLEFT(dest), 1);
    LEFTRIGHT(dest) = nodeCtor(LEFT(dest), LEFTRIGHT(dest), 0);

    LEFTLEFT(dest)->node_type.bytes.is_operator = 1;
    LEFTLEFT(dest)->value.symbol  = '*';
    LEFTRIGHT(dest)->node_type.bytes.is_operator = 1;
    LEFTRIGHT(dest)->value.symbol = '*';

    LEFTLEFT(dest)->left_child   = nodeCtor(LEFTLEFT(dest), LEFTLEFT(dest)->left_child, 1);
    LEFTLEFT(dest)->right_child  = nodeCtor(LEFTLEFT(dest), LEFTLEFT(dest)->right_child, 0);
    LEFTRIGHT(dest)->left_child  = nodeCtor(LEFTRIGHT(dest), LEFTRIGHT(dest)->left_child, 1);
    LEFTRIGHT(dest)->right_child = nodeCtor(LEFTRIGHT(dest), LEFTRIGHT(dest)->right_child, 0);

    diffNode(LEFTLEFT(dest)->left_child, sourse->left_child);
    treeCpy(LEFTLEFT(dest)->right_child, sourse->right_child);

    diffNode(LEFTRIGHT(dest)->left_child, sourse->right_child);
    treeCpy(LEFTRIGHT(dest)->right_child, sourse->left_child);

    return 0;
}


static int diffCos(Node *dest, Node *sourse)
{
    assert(dest);
    assert(sourse);

    dest->node_type.bytes.is_operator = 1;
    dest->value.symbol = '*';

    LEFT(dest)  = nodeCtor(dest, LEFT(dest), 1);
    RIGHT(dest) = nodeCtor(dest, RIGHT(dest), 0);

    LEFT(dest)->node_type.bytes.is_operator = 1;
    LEFT(dest)->value.symbol  = '*';
    RIGHT(dest)->node_type.bytes.is_number = 1;
    RIGHT(dest)->value.number = -1;

    LEFTLEFT(dest)  = nodeCtor(LEFT(dest), LEFTLEFT(dest), 1);
    LEFTRIGHT(dest) = nodeCtor(LEFT(dest), LEFTRIGHT(dest), 0);

    LEFTLEFT(dest)->node_type.bytes.is_func = 1;
    LEFTLEFT(dest)->node_type.bytes.is_sin  = 1;
    LEFTLEFT(dest)->value.func = "sin";
    
    treeCpy(LEFTLEFT(dest)->left_child, sourse->left_child);
    diffNode(LEFT(dest)->right_child, sourse->left_child);

    return 0;
}


static int diffSin(Node *dest, Node *sourse)
{
    assert(dest);
    assert(sourse);

    dest->node_type.bytes.is_operator = 1;
    dest->value.symbol = '*';

    LEFT(dest)  = nodeCtor(dest, LEFT(dest), 1);
    RIGHT(dest) = nodeCtor(dest, RIGHT(dest), 0);

    LEFT(dest)->node_type.bytes.is_func = 1;
    LEFT(dest)->value.func = "cos";

    LEFTLEFT(dest) = nodeCtor(LEFT(dest), LEFTLEFT(dest), 1);

    diffNode(RIGHT(dest), sourse->left_child);
    treeCpy(LEFTLEFT(dest), sourse->left_child);

    return 0;
}


static int diffLn(Node *dest, Node *sourse)
{
    assert(dest);
    assert(sourse);

    dest->node_type.bytes.is_operator = 1;
    dest->value.symbol = '/';

    LEFT(dest)  = nodeCtor(dest, LEFT(dest), 1);
    RIGHT(dest) = nodeCtor(dest, RIGHT(dest), 0);

    diffNode(LEFT(dest), sourse->left_child);
    treeCpy(RIGHT(dest), sourse->left_child);

    return 0;
}


static int diffPow(Node *dest, Node *sourse)
{
    assert(dest);
    assert(sourse);

    dest->node_type.bytes.is_operator = 1;
    dest->value.symbol = '*';

    LEFT(dest)  = nodeCtor(dest, LEFT(dest), 1);
    RIGHT(dest) = nodeCtor(dest, RIGHT(dest), 0);

    diffNode(RIGHT(dest), sourse->left_child);

    LEFT(dest)->node_type.bytes.is_operator = 1;
    LEFT(dest)->value.symbol = '*';
 
    LEFTLEFT(dest)  = nodeCtor(LEFT(dest), LEFTLEFT(dest), 1);
    LEFTRIGHT(dest) = nodeCtor(LEFT(dest), LEFTRIGHT(dest), 0);

    treeCpy(LEFTLEFT(dest), sourse->right_child);

    LEFTRIGHT(dest)->node_type.bytes.is_operator = 1;
    LEFTRIGHT(dest)->value.symbol = '^';

    LEFTRIGHT(dest)->left_child  = nodeCtor(LEFTRIGHT(dest), LEFTRIGHT(dest)->left_child, 1);
    LEFTRIGHT(dest)->right_child = nodeCtor(LEFTRIGHT(dest), LEFTRIGHT(dest)->right_child, 0);

    treeCpy(LEFTRIGHT(dest)->left_child, sourse->left_child);

    #define LEFTRIGHT2(arg) arg->left_child->right_child->right_child

    LEFTRIGHT2(dest)->node_type.bytes.is_operator = 1;
    LEFTRIGHT2(dest)->value.symbol = '-';

    LEFTRIGHT2(dest)->left_child  = nodeCtor(LEFTRIGHT2(dest), LEFTRIGHT2(dest)->left_child, 1);
    LEFTRIGHT2(dest)->right_child = nodeCtor(LEFTRIGHT2(dest), LEFTRIGHT2(dest)->right_child, 0);

    LEFTRIGHT2(dest)->right_child->node_type.bytes.is_number = 1;
    LEFTRIGHT2(dest)->right_child->value.number = 1;

    treeCpy(LEFTRIGHT2(dest)->left_child, sourse->right_child);

    #undef LEFTRIGHT2

    return 0;
}


