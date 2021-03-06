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


static int removeConst(Node *node);

static int cutNode(Node *node, Tree *tree);
static int cutCut(Tree *tree, Node *node, bool is_left);
static int cutAddSub(Node *node, Tree *tree);
static int cutMul(Node *node, Tree *tree);
static int cutDiv(Node *node, Tree *tree);
static int cutPow(Node *node, Tree *tree);

static int foo(Node *node, Tree *tree);

static int swap(void *element1, void *element2, unsigned int element_size);


int removeConstant(Tree *tree)
{
    assert(tree);
    if (tree->status.destructed_tree)
    {
        printf("!!! ERROR Can't work with destructed tree !!!\n");
        return 0;
    }

    int is_removed = removeConst(tree->root);

    return is_removed;
}


static int removeConst(Node *node)
{
    assert(node);

    if (!(node->node_type.bytes.is_operator || node->node_type.bytes.is_func))
    {
        return 0;
    }

    int is_removed = 0;
    int left_type  = LEFT(node)->node_type.number;
    int right_type = 0;
    if (node->node_type.bytes.is_operator) 
    {
        right_type = RIGHT(node)->node_type.number;
    }
    
    if ((left_type & IS_OPERATOR) || (left_type & IS_FUNC))
    {
        is_removed += removeConst(LEFT(node));
    }
    if (node->node_type.bytes.is_operator)
    {
        if ((right_type & IS_OPERATOR) || (right_type & IS_FUNC))
        {
            is_removed += removeConst(RIGHT(node));
        }
    }

    if (node->node_type.bytes.is_operator && (left_type & IS_NUMBER) && (right_type & IS_NUMBER))
    {
        node->node_type.bytes.is_number   = 1;
        node->node_type.bytes.is_operator = 0;

        double left  = LEFT(node)->value.number;
        double right = RIGHT(node)->value.number;

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
        return is_removed;
    }
    if (node->node_type.bytes.is_func && (left_type & IS_NUMBER))
    {
        node->node_type.number = IS_NUMBER;

        is_removed = 1;
        free(node->value.func);

        switch (node->node_type.number & (~(IS_COS - 1)))
        {
            case IS_SIN:
                node->value.number = sin(node->left_child->value.number);
                break;
            case IS_COS:
                node->value.number = cos(node->left_child->value.number);
                break;
            case IS_LN:
                node->value.number = logl(node->left_child->value.number);
                break;
        }
        
        free(LEFT(node));
        LEFT(node) = nullptr;
        return is_removed; 
    }
    

    return is_removed;
}


int cutTree(Tree *tree)
{
    assert(tree);
    if (tree->status.destructed_tree)
    {
        printf("!!! ERROR Can't work with destructed tree !!!\n");
        return -1;
    }

    int is_cutted = cutNode(tree->root, tree);

    return is_cutted;
}


static int cutNode(Node *node, Tree *tree)
{
    assert(node);
    assert(tree);

    if (!node->node_type.bytes.is_operator)
    {
        return 0;
    }

    int is_cutted = 0;

    if (LEFT(node)->node_type.bytes.is_operator)
    {
        is_cutted += cutNode(LEFT(node), tree);
    }
    if (RIGHT(node)->node_type.bytes.is_operator)
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
        case '^':
            is_cutted += !cutPow(node, tree);
            break;
    }

    return is_cutted;
}


static int cutAddSub(Node *node, Tree *tree)
{
    assert(node);
    assert(tree);
    
    if (LEFT(node)->node_type.bytes.is_number)
    {
        if (LEFT(node)->value.number == 0)
        {
            cutCut(tree, node, 0);
            free(LEFT(node));
            free (node);
            return 0;
        }
    }
    else if (RIGHT(node)->node_type.bytes.is_number)
    {
        if (RIGHT(node)->value.number == 0)
        {
            cutCut(tree, node, 1);
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

    
    int is_cutted = foo(node, tree);
    if (!is_cutted)
    {
        return 0;
    }
    swap(&(LEFT(node)), &(RIGHT(node)), sizeof(Node *));
    foo(node, tree);
    swap(&(LEFT(node)), &(RIGHT(node)), sizeof(Node *));
    
    return -1;
}


static int cutDiv(Node *node, Tree *tree)
{
    assert(node);
    assert(tree);

    if (RIGHT(node)->node_type.bytes.is_number)
    {
        if (RIGHT(node)->value.number == 1)
        {
            cutCut(tree, node, 1);

            free(RIGHT(node));
            free(node);
            return 0;
        }
    }

    return -1;
}


static int cutPow(Node *node, Tree *tree)
{
    assert(node);
    assert(tree);

    if (LEFT(node)->node_type.bytes.is_number)
    {
        if (LEFT(node)->value.number == 1)
        {
            cutCut(tree, node, 0);

            free(LEFT(node));
            free(node);
            return 0;
        }
        else if (LEFT(node)->value.number == 0)
        {
            cutCut(tree, node, 0);

            nodeDtor(RIGHT(node));
            free(node);
            return 0;
        }
    }
    if (RIGHT(node)->node_type.bytes.is_number)
    {
        if (RIGHT(node)->value.number == 1)
        {
            cutCut(tree, node, 1);

            free(RIGHT(node));
            free(node);
            return 0;
        }
        else if (RIGHT(node)->value.number == 0)
        {
            cutCut(tree, node, 1);

            nodeDtor(LEFT(node));
            free(node);
            return 0;
        }
    }

    return -1;
}


int optimiz(Tree *tree)
{
    assert(tree);
    
    int is_optimized = 0;

    do
    {
        is_optimized = 0;
        
        //texDump(tree);
        is_optimized += removeConstant(tree);
        is_optimized += cutTree(tree);
        
    } while (is_optimized);
    //texDump(tree);
    
    return 0;
}


static int cutCut(Tree *tree, Node *node, bool is_left)
{
    assert(node);

    if (node != tree->root && node->parent->left_child == node)
    {
        node->parent->left_child = (is_left) ? LEFT(node) : RIGHT(node);
        node->parent->left_child->parent = node->parent;        
    }
    else if (node != tree->root)
    {
        node->parent->right_child = (is_left) ? LEFT(node) : RIGHT(node);
        node->parent->right_child->parent = node->parent;        
    }
    else
    {
        tree->root = (is_left) ? LEFT(node) : RIGHT(node);
        tree->root->parent = nullptr;
    }

    return 0;
}


static int foo(Node *node, Tree *tree)
{
    assert(node);
    assert(tree);

    if (LEFT(node)->node_type.bytes.is_number)
    {
        if (LEFT(node)->value.number == 1)
        {
            cutCut(tree, node, 0);
            free(LEFT(node));
            free(node);
            return 0;
        }
        if (LEFT(node)->value.number == 0)
        {
            cutCut(tree, node, 1);
            nodeDtor(RIGHT(node));
            free(node);
            return 0;
        }
    }

    return -1;
}


static int swap(void *element1, void *element2, unsigned int element_size)
{
    char *elem1 = (char *) element1;
    char *elem2 = (char *) element2;

    while (element_size >= 8) 
    {
        __int64_t buffer = *((__int64_t *) elem1);
        *((__int64_t *) elem1) = *((__int64_t *) elem2);
        *((__int64_t *) elem2) = buffer;

        elem1 += 8;
        elem2 += 8;
        element_size -= 8;
    }
    while (element_size >= 4) 
    {
        __int32_t buffer = *((__int32_t *) elem1);
        *((__int32_t *) elem1) = *((__int32_t *) elem2);
        *((__int32_t *) elem2) = buffer;

        elem1 += 4;
        elem2 += 4;
        element_size -= 4;
    }
    while (element_size >= 2) 
    {
        __int16_t buffer = *((__int16_t *) elem1);
        *((__int16_t *) elem1) = *((__int16_t *) elem2);
        *((__int16_t *) elem2) = buffer;

        elem1 += 2;
        elem2 += 2;
        element_size -=2;
    }
    if (element_size == 1)
    {
        __int8_t buffer = *elem1;
        *elem1 = *elem2;
        *elem2 = buffer;
    }

    return 0;
}
