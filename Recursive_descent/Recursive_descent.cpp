#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "LogsLib.h"
#include "../Tree/Tree.h"
#include "Recursive_descent.h"


static int   lexical(Tokens *tokens, char *string);
static int   tokensCtor(Tokens *tokens);
static Node *number();
static Node *identific();
static Node *oper();
static int   SyntaxError();
static int tokensResize(Tokens *tokens);

static Node *GetG(Tokens *tokens);
static Node *GetN(Tokens *tokens, int *iter);
static Node *GetT(Tokens *tokens, int *iter);
static Node *GetP(Tokens *tokens, int *iter);
static Node *GetN(Tokens *tokens, int *iter);
static Node *GetE(Tokens *tokens, int *iter);



char *S = nullptr;


int constructTree(Tree *tree, char *string)
{
    assert(string);
    assert(tree);
    if (tree->status.destructed_tree)
    {
        printf("!!! ERROR Can't work with destructed tree !!!\n");
        return -1;
    }

    Tokens tokens = {};
    tokensCtor(&tokens);
    
    lexical(&tokens, string);

    for (int i = 0; i < tokens.size; i++)
    {
        printf("type = %d\n", tokens.array[i]->node_type.number);
    }

    tree->root = GetG(&tokens);

    free(tokens.array);
    return 0;
}


static int lexical(Tokens *tokens, char *string)
{
    assert(tokens);
    assert(string);

    S = string;
    int is_ok = 0;

    while (*S != '$')
    {
        is_ok = 1;

        if (tokens->size + 1 > tokens->capacity)
        {
            tokens->capacity *= 2;
            void *temp_ptr = (Node **) realloc(tokens->array, sizeof(Node *) * tokens->capacity);
            if (temp_ptr)
            {
                tokens->array = (Node **) temp_ptr;
            }
            else
            {
                printf("!!! ERROR Can't allocate memory !!!\n");
                return -1;
            }
        }

        if (*S >= '0' && *S <= '9')     
        {
            tokens->array[tokens->size] = number();
            tokens->size++;
        }
        else if (*S >= 'a' && *S <= 'z') 
        {
            tokens->array[tokens->size] = identific();
            tokens->size++;
        }
        else                             
        {
            tokens->array[tokens->size] = oper();
            tokens->size++;
        }
    }
    if (is_ok)
    {
        if (tokens->size + 1 > tokens->capacity)
        {
            tokens->capacity *= 2;
            void *temp_ptr = (Node **) realloc(tokens->array, sizeof(Node *) * tokens->capacity);
            if (temp_ptr)
            {
                tokens->array = (Node **) temp_ptr;
            }
            else
            {
                printf("!!! ERROR Can't allocate memory !!!\n");
                return -1;
            }
        }

        tokens->array[tokens->size] = (Node *) calloc(1, sizeof(Node));
        tokens->array[tokens->size]->node_type.bytes.is_operator = 1;
        tokens->array[tokens->size]->value.symbol                = '$';
        tokens->size++;

        return 0;
    }

    SyntaxError();
    return -1;
}


static int tokensCtor(Tokens *tokens)
{
    assert(tokens);

    tokens->array = (Node **) calloc(1, sizeof(Node *));
    tokens->capacity = 1;

    return 0;
}


static int tokensDtor(Tokens *tokens)
{
    assert(tokens);

    free(tokens);
    return 0;
}


static Node *number()
{
    Node *node = (Node *) calloc(1, sizeof(Node));
    double value = 0;

    while(*S >= '0' && *S <= '9')
    {
        value = value * 10 + (*S - '0');
        S++;
    }

    node->node_type.bytes.is_number = 1;
    node->value.number = value;

    return node;
}


static Node *identific()
{
    Node *node = (Node *) calloc(1, sizeof(Node));
    char buffer[20] = {};

    for (int i = 0; i < 19 && *S >= 'a' && *S <= 'z'; i++)
    {
        buffer[i] = *S++;
    }

    if (strlen(buffer) == 1)
    {
        node->node_type.bytes.is_variable = 1;
        node->value.symbol = buffer[0];

        return node;
    }

    node->node_type.bytes.is_func = 1;

    node->value.func = (char *) calloc(strlen(buffer), sizeof(char));
    strcpy(node->value.func, buffer);

    if (strstr("sincosln", node->value.func))
    {
        switch (node->value.func[0])
        {
            case 's':
                node->node_type.bytes.is_sin = 1;
                return node;
            case 'c':
                node->node_type.bytes.is_cos = 1;
                return node;
            case 'l':
                node->node_type.bytes.is_ln = 1;
                return node;
        }
    }

    SyntaxError();
    return nullptr;
}


static Node *oper()
{
    Node *node = (Node *) calloc(1, sizeof(Node));

    node->value.symbol = *S++;
    node->node_type.bytes.is_operator = 1;
    printf("node value = %c\n", node->value.symbol);

    if (!strchr("+-/*^()$", node->value.symbol))
    {
        SyntaxError();
    }

    return node;
}


static int SyntaxError()
{
    printf("!!! ERROR SyntaxError !!!\n");
    exit(1);
}


static Node *GetG(Tokens *tokens)
{
    assert(tokens);

    int iter = 0;

    Node *root = GetE(tokens, &iter);
    printf("iter = %d\n", iter);

    if (tokens->array[iter]->node_type.bytes.is_operator && tokens->array[iter]->value.symbol == '$')
    {
        iter++;
    }
    else
    {
        
        SyntaxError();
    }

    free(tokens->array[iter -1]);
    return root;
}


static Node *GetT(Tokens *tokens, int *iter)
{
    assert(tokens);
    assert(iter);

    Node *oper = nullptr;
    
    Node *value = GetP(tokens, iter);

    while (tokens->array[*iter]->node_type.bytes.is_operator && strchr("*/", tokens->array[*iter]->value.symbol))
    {
        oper = tokens->array[*iter];
        oper->left_child = value;
        (*iter)++;

        oper->right_child = GetP(tokens, iter);

        value = oper;
    }

    return value;
}


static Node *GetP(Tokens *tokens, int *iter)
{
    assert(tokens);
    assert(iter);

    if (tokens->array[*iter]->node_type.bytes.is_operator && tokens->array[*iter]->value.symbol == '(')
    {
        (*iter)++;
        free(tokens->array[*iter - 1]);

        Node *value = GetE(tokens, iter);

        if (tokens->array[*iter]->node_type.bytes.is_operator && tokens->array[*iter]->value.symbol == ')')
        {
            (*iter)++;
            free(tokens->array[*iter - 1]);
            return value;
        }
        
        SyntaxError();
    }
    else
    {
        return GetN(tokens, iter);
    }

    return nullptr;
}


static Node *GetN(Tokens *tokens, int *iter)
{
    assert(tokens);
    assert(iter);

    if (tokens->array[*iter]->node_type.bytes.is_number || tokens->array[*iter]->node_type.bytes.is_variable)
    {   
        printf("iter = %d\n", *iter);
        (*iter)++;
        printf("iter = %d\n", *iter);
        return tokens->array[*iter - 1];
    }
    
    SyntaxError();
    return nullptr;
}


static Node *GetE(Tokens *tokens, int *iter)
{
    assert(tokens);
    assert(iter);

    Node *oper = nullptr;
    
    Node *value = GetT(tokens, iter);

    while (tokens->array[*iter]->node_type.bytes.is_operator && strchr("+-", tokens->array[*iter]->value.symbol))
    {
        
        oper = tokens->array[*iter];
        oper->left_child = value;
        (*iter)++;

        oper->right_child = GetT(tokens, iter);

        value = oper;
    }    

    return value;
}