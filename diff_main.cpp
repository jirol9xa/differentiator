#include <stdio.h>
#include "Tree/Tree.h"
#include "include/textLib.h"
#include "include/logsLib.h"
#include "diff.h"

int main(int argc, const char **argv)
{
    Tree tree = {};
    treeCtor(&tree);

    if (argc < 2)
    {
        printf("!!! Can't run without input file !!!\n");
    }
    FILE *sourse = fopen(argv[1], "r");

    readFormula(sourse, &tree);
    PRINT_LINE;
    treeDump(&tree);

    treeDtor(&tree);
    fclose(sourse);
    return 0;
}