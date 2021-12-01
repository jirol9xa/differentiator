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

#define LEFT(arg)      arg->left_child
#define RIGHT(arg)     arg->right_child
#define LEFTLEFT(arg)  arg->left_child->left_child
#define LEFTRIGHT(arg) arg->left_child->right_child

static int removeConst(Node *node);
static int cutNode(Node *node, Tree *tree);
static int cutAddSub(Node *node, Tree *tree);
static int cutMul(Node *node, Tree *tree);
static int cutDiv(Node *node, Tree *tree);


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

    Node *cur_node = tree->root;

    for (int i = 0; i < num_symb && text[i] != '\n'; i++)
    {
        i += skipSpace(text + i);

        if (text[i] == '(')
        {
            if (tree->status & EMPTY_TREE)
            {
                tree->status &= (~EMPTY_TREE);
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

    diffNode(result->root, sourse->root);

    return 0;
}


int diffNode(Node *dest, Node *sourse)
{
    assert(dest);
    assert(sourse);

    //treeDump(&rslt_tree);

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


int diffMul(Node *dest, Node *sourse)
{
    assert(dest);
    assert(sourse);

    dest->node_type   |= IS_OPERATOR;
    dest->value.symbol = '+';

    LEFT(dest)  = nodeCtor(dest, LEFT(dest), 1);
    RIGHT(dest) = nodeCtor(dest, RIGHT(dest), 0);
    
    LEFT(dest)->node_type    |= IS_OPERATOR;
    LEFT(dest)->value.symbol  = '*';
    RIGHT(dest)->node_type   |= IS_OPERATOR;
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


int diffAdd(Node *dest, Node *sourse)
{
    assert(dest);
    assert(sourse);

    dest->node_type   |= IS_OPERATOR;
    dest->value.symbol = '+';

    LEFT(dest)  = nodeCtor(dest, LEFT(dest), 1);
    RIGHT(dest) = nodeCtor(dest, RIGHT(dest), 0);

    diffNode(LEFT(dest), sourse->left_child);
    diffNode(RIGHT(dest), sourse->right_child);

    return 0;
}


int diffSub(Node *dest, Node *sourse)
{
    assert(dest);
    assert(sourse);

    dest->node_type   |= IS_OPERATOR;
    dest->value.symbol = '-';

    LEFT(dest)  = nodeCtor(dest, LEFT(dest), 1);
    RIGHT(dest) = nodeCtor(dest, RIGHT(dest), 0);

    diffNode(LEFT(dest), sourse->left_child);
    diffNode(RIGHT(dest), sourse->right_child);

    return 0;    
}


int diffDiv(Node *dest, Node *sourse)
{
    assert(dest);
    assert(sourse);

    dest->node_type     = IS_OPERATOR;
    dest->value.symbol |= '/';

    LEFT(dest)  = nodeCtor(dest, LEFT(dest), 1);
    RIGHT(dest) = nodeCtor(dest, RIGHT(dest), 0);;

    LEFT(dest)->node_type    |= IS_OPERATOR;
    LEFT(dest)->value.symbol  = '-';
    RIGHT(dest)->node_type   |= IS_OPERATOR;
    RIGHT(dest)->value.symbol = '*';

    RIGHT(dest)->left_child  = nodeCtor(RIGHT(dest), RIGHT(dest)->left_child, 1);
    RIGHT(dest)->right_child = nodeCtor(RIGHT(dest), RIGHT(dest)->right_child, 0);

    treeCpy(RIGHT(dest)->left_child,  sourse->right_child);
    treeCpy(RIGHT(dest)->right_child, sourse->right_child);
    

    LEFTLEFT(dest)  = nodeCtor(LEFT(dest), LEFTLEFT(dest), 1);
    LEFTRIGHT(dest) = nodeCtor(LEFT(dest), LEFTRIGHT(dest), 0);

    LEFTLEFT(dest)->node_type    |= IS_OPERATOR;
    LEFTLEFT(dest)->value.symbol  = '*';
    LEFTRIGHT(dest)->node_type   |= IS_OPERATOR;
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


int diffCos(Node *dest, Node *sourse)
{
    assert(dest);
    assert(sourse);

    dest->node_type   |= IS_OPERATOR;
    dest->value.symbol = '*';

    LEFT(dest)  = nodeCtor(dest, LEFT(dest), 1);
    RIGHT(dest) = nodeCtor(dest, RIGHT(dest), 0);

    LEFT(dest)->node_type    |= IS_OPERATOR;
    LEFT(dest)->value.symbol  = '*';
    RIGHT(dest)->node_type   |= IS_NUMBER;
    RIGHT(dest)->value.number = -1;

    LEFTLEFT(dest)  = nodeCtor(LEFT(dest), LEFTLEFT(dest), 1);
    LEFTRIGHT(dest) = nodeCtor(LEFT(dest), LEFTRIGHT(dest), 0);

    LEFTLEFT(dest)->node_type |= (IS_FUNC | IS_SIN);
    LEFTLEFT(dest)->value.func = "sin";
    
    treeCpy(LEFTLEFT(dest)->left_child, sourse->left_child);
    diffNode(LEFT(dest)->right_child, sourse->left_child);

    return 0;
}


int diffSin(Node *dest, Node *sourse)
{
    assert(dest);
    assert(sourse);

    dest->node_type   |= IS_OPERATOR;
    dest->value.symbol = '*';

    LEFT(dest)  = nodeCtor(dest, LEFT(dest), 1);
    RIGHT(dest) = nodeCtor(dest, RIGHT(dest), 0);

    LEFT(dest)->node_type |= IS_FUNC;
    LEFT(dest)->value.func = "cos";

    LEFTLEFT(dest) = nodeCtor(LEFT(dest), LEFTLEFT(dest), 1);

    diffNode(RIGHT(dest), sourse->left_child);
    treeCpy(LEFTLEFT(dest), sourse->left_child);

    return 0;
}


int diffLn(Node *dest, Node *sourse)
{
    assert(dest);
    assert(sourse);

    dest->node_type   |= IS_OPERATOR;
    dest->value.symbol = '/';

    LEFT(dest)  = nodeCtor(dest, LEFT(dest), 1);
    RIGHT(dest) = nodeCtor(dest, RIGHT(dest), 0);

    diffNode(LEFT(dest), sourse->left_child);
    treeCpy(RIGHT(dest), sourse->left_child);

    return 0;
}


int diffPow(Node *dest, Node *sourse)
{
    assert(dest);
    assert(sourse);

    dest->node_type   |= IS_OPERATOR;
    dest->value.symbol = '*';

    LEFT(dest)  = nodeCtor(dest, LEFT(dest), 1);
    RIGHT(dest) = nodeCtor(dest, RIGHT(dest), 0);

    diffNode(RIGHT(dest), sourse->left_child);

    LEFT(dest)->node_type   |= IS_OPERATOR;
    LEFT(dest)->value.symbol = '*';
 
    LEFTLEFT(dest)  = nodeCtor(LEFT(dest), LEFTLEFT(dest), 1);
    LEFTRIGHT(dest) = nodeCtor(LEFT(dest), LEFTRIGHT(dest), 0);

    treeCpy(LEFTLEFT(dest), sourse->right_child);

    LEFTRIGHT(dest)->node_type    = IS_OPERATOR;
    LEFTRIGHT(dest)->value.symbol = '^';

    LEFTRIGHT(dest)->left_child  = nodeCtor(LEFTRIGHT(dest), LEFTRIGHT(dest)->left_child, 1);
    LEFTRIGHT(dest)->right_child = nodeCtor(LEFTRIGHT(dest), LEFTRIGHT(dest)->right_child, 0);

    treeCpy(LEFTRIGHT(dest)->left_child, sourse->left_child);

    #define LEFTRIGHT2(arg) arg->left_child->right_child->right_child

    LEFTRIGHT2(dest)->node_type    = IS_OPERATOR;
    LEFTRIGHT2(dest)->value.symbol = '-';

    LEFTRIGHT2(dest)->left_child  = nodeCtor(LEFTRIGHT2(dest), LEFTRIGHT2(dest)->left_child, 1);
    LEFTRIGHT2(dest)->right_child = nodeCtor(LEFTRIGHT2(dest), LEFTRIGHT2(dest)->right_child, 0);

    LEFTRIGHT2(dest)->right_child->node_type   |= IS_NUMBER;
    LEFTRIGHT2(dest)->right_child->value.number = 1;

    treeCpy(LEFTRIGHT2(dest)->left_child, sourse->right_child);

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

    is_removed = removeConst(tree->root);

    return is_removed;
}


static int removeConst(Node *node)
{
    assert(node);

    if (!(node->node_type & IS_OPERATOR))
    {
        return 0;
    }

    int is_removed = 0;

    if (LEFT(node)->node_type & IS_OPERATOR)
    {
        is_removed += removeConst(LEFT(node));
    }
    if (RIGHT(node)->node_type & IS_OPERATOR)
    {
        is_removed = removeConst(RIGHT(node));
    }

    if ((LEFT(node)->node_type & IS_NUMBER) && (RIGHT(node)->node_type & IS_NUMBER))
    {
        node->node_type = IS_NUMBER;

        int left = LEFT(node)->value.number;
        int right = RIGHT(node)->value.number;

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

        free(LEFT(node));
        free(RIGHT(node));

        LEFT(node)  = nullptr;
        RIGHT(node) = nullptr;

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

    is_cutted = cutNode(tree->root, tree);

    return is_cutted;
}


static int cutNode(Node *node, Tree *tree)
{
    assert(node);
    assert(tree);

    if (!(node->node_type & IS_OPERATOR))
    {
        return 0;
    }

    int is_cutted = 0;

    if (LEFT(node)->node_type & IS_OPERATOR)
    {
        is_cutted += cutNode(LEFT(node), tree);
    }
    if (RIGHT(node)->node_type & IS_OPERATOR)
    {
        is_cutted += cutNode(RIGHT(node), tree);
    }

    switch (node->value.symbol)
    {
        case '+':
            is_cutted += !cutAddSub(node, tree);
            break;
        case '-':
            is_cutted += !cutAddSub(node, tree);
            break;
        case '*':
            is_cutted += !cutMul(node, tree);
            break;
        case '/':
            is_cutted += !cutDiv(node, tree);
            break;
    }

    return is_cutted;
}


static int cutAddSub(Node *node, Tree *tree)
{
    assert(node);
    assert(tree);
    PRINT_LINE;

    if (LEFT(node)->node_type & IS_NUMBER)
    {
        if (LEFT(node)->value.number == 0)
        {
            if (node != tree->root && node->parent->left_child == node)
            {
                node->parent->left_child = RIGHT(node);
            }
            else if (node != tree->root)
            {
                node->parent->right_child = RIGHT(node);
            }
            else
            {
                tree->root = RIGHT(node);
            }

            free(LEFT(node));
            free (node);
            return 0;
        }
    }
    else if (RIGHT(node)->node_type & IS_NUMBER)
    {
        if (RIGHT(node)->value.number == 0)
        {
            if (node != tree->root && node->parent->left_child == node)
            {
                node->parent->left_child = LEFT(node);
            }
            else if (node != tree->root)
            {
                node->parent->right_child = LEFT(node);
            }
            else
            {
                tree->root = LEFT(node);
            }

            free(RIGHT(node));
            free(node);
            return 0;
        }
    }

    return -1;
}


static int cutMul(Node *node, Tree *tree)
{
    assert(node);
    assert(tree);

    PRINT_LINE;

    if (LEFT(node)->node_type & IS_NUMBER)
    {
        if (LEFT(node)->value.number == 1)
        {
            if (node != tree->root && node->parent->left_child == node)
            {
                node->parent->left_child = RIGHT(node);
            }
            else if (node != tree->root)
            {
                node->parent->right_child = RIGHT(node);
            }
            else
            {
                tree->root = RIGHT(node);
            }

            free(LEFT(node));
            free(node);
            return 0;
        }
        if (LEFT(node)->value.number == 0)
        {
            
            if (node != tree->root && node->parent->left_child == node)
            {
                PRINT_LINE;
                node->parent->left_child = LEFT(node);
            }
            else if (node != tree->root)
            {
                PRINT_LINE;
                node->parent->right_child = LEFT(node);
            }
            else
            {
                PRINT_LINE;
                tree->root = LEFT(node);
            }

            PRINT_LINE;
            nodeDtor(RIGHT(node));
            PRINT_LINE;
            free(node);
            return 0;
        }
    }

    if (RIGHT(node)->node_type & IS_NUMBER)
    {
        if (RIGHT(node)->value.number == 1)
        {
            if (node != tree->root && node->parent->left_child == node)
            {
                node->parent->left_child = LEFT(node);
            }
            else if (node != tree->root)
            {
                node->parent->right_child = LEFT(node);
            }
            else
            {
                tree->root = LEFT(node);
            }

            free(RIGHT(node));
            free(node);
            return 0;
        }
        if (RIGHT(node)->value.number == 0)
        {
            if (node != tree->root && node->parent->left_child == node)
            {
                node->parent->left_child = RIGHT(node);
            }
            else if (node != tree->root)
            {
                node->parent->right_child = RIGHT(node);
            }
            else
            {
                tree->root = RIGHT(node);
            }
            
            nodeDtor(LEFT(node));
            free(node);
            return 0;
        }
    }

    return -1;
}


static int cutDiv(Node *node, Tree *tree)
{
    assert(node);
    assert(tree);

    if (RIGHT(node)->node_type & IS_NUMBER)
    {
        if (RIGHT(node)->value.number == 1)
        {
            if (node != tree->root && node->parent->left_child == node)
            {
                node->parent->left_child = LEFT(node);
            }
            else if (node != tree->root)
            {
                node->parent->right_child = LEFT(node);
            }
            else
            {
                tree->root = LEFT(node);
            }

            free(RIGHT(node));
            free(node);
            return 0;
        }
    }

    return -1;
}


int optimiz(Tree *tree)
{
    assert(tree);
    //PRINT_LINE;
    int is_optimized = 0;

    do
    {
        is_optimized = 0;
        PRINT_LINE;
        texDump(tree);
        is_optimized += removeConstant(tree);
        printf("after remove = %d\n", is_optimized);
        PRINT_LINE;
        //treeDump(&rslt_tree);
        is_optimized += cutTree(tree);
        printf("after cut = %d\n", is_optimized);
        PRINT_LINE;
        //treeDump(&rslt_tree);
    } while (is_optimized);
    
    return 0;
}
