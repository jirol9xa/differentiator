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

    printf("Length = %ld\n", num_symb);
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
            PRINT_LINE;
            printf("cur_node = %p\n", cur_node);
            i += readArg(cur_node, text + i);
            printf("text[i] = %c text[i + 1] = %c\n", text[i], text[i + 1]);
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
        PRINT_LINE;
        node->node_type = IS_VARIABLE;
        sscanf(text + i, "%c", &(node->value.symbol));
        printf("node value = %c\n", node->value.symbol);
        return i;
    }
}