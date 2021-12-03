#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "LogsLib.h"
#include "../Tree/Tree.h"
#include "Recursive_descent.h"


char *S = nullptr;


Tree *constructTree(Tree *tree, char *string)
{
    assert(string);
    assert(tree);
    if (tree->status.destructed_tree)
    {
        printf("!!! ERROR Can't work with destructed tree !!!\n");
    }

    Tokens tokens = {};
    tokensCtor(&tokens);
    S = string;

    while (*S != '$')
    {
        if (tokens.size + 1 > tokens.capacity)
        {
            tokens.capacity *= 2;
            tokens.array = (Node **) realloc(tokens.array, sizeof(Node *) * tokens.capacity);
        }

        if (*S >= '0' && *S <= '9')     
        {
            tokens.array[tokens.size] = number();
        }
        else if (*S >= 'a' && *S <= 'z') 
        {
            tokens.array[tokens.size] = identific();
        }
        else                             
        {
            tokens.array[tokens.size] = oper();
        }
    }

    for (int i = 0; i < tokens.size; i++)
    {
        printf("type = %d\n", tokens.array[i]->node_type);
    }

    PRINT_LINE;

    return tree;
}


int tokensCtor(Tokens *tokens)
{
    assert(tokens);

    tokens->array = (Node **) calloc(1, sizeof(Node *));
    tokens->capacity = 1;
}


Node *number()
{
    Node *node = (Node *) calloc(1, sizeof(Node));
    double value = 0;
    printf("%s\n", S);

    while(*S >= '0' && *S <= '9')
    {
        value = value * 10 + (*S - '0');
        S++;
    }
    printf("value = %lg\n", value);

    node->node_type.bytes.is_number = 1;
    node->value.number = value;

    return node;
}


Node *identific()
{
    Node *node = (Node *) calloc(1, sizeof(Node));
    char buffer[20] = {};
    PRINT_LINE;

    for (int i = 0; i < 19 && *S >= 'a' && *S <= 'z'; i++)
    {
        buffer[i] = *S++;
    }

    node->node_type.bytes.is_func = 1;

    node->value.func = (char *) calloc(strlen(buffer), sizeof(char));
    strcpy(node->value.func, buffer);

    printf("ident = %s\n", node->value.func);

    return node;
}


Node *oper()
{
    Node *node = (Node *) calloc(1, sizeof(Node));
    PRINT_LINE;
    node->node_type.bytes.is_operator = 1;
    node->value.symbol = *S++;

    return node;
}



int SyntaxError()
{
    printf("!!! ERROR SyntaxError !!!\n");
    exit(1);
}


Node *GetG(const char *string)
{
    assert(string);

    S = (char *) string;

    if (*S == '$')  S++;
    else            SyntaxError();

}


Node *GetN()
{
    int val = 0;

    Node *node;
}

Node *GetS()
{

}


