#include <stdio.h>
#include "Tree/Tree.h"
#include "include/textLib.h"
#include "include/logsLib.h"
#include "diff.h"

Tree rslt_tree = {};


int main(int argc, const char **argv)
{
    Tree srs_tree = {};
    treeCtor(&srs_tree);

    if (argc < 2)
    {
        printf("!!! Can't run without input file !!!\n");
    }
    FILE *sourse = fopen(argv[1], "r");

    readFormula(sourse, &srs_tree);
    treeDump(&srs_tree);

    treeCtor(&rslt_tree);

    diffur(&srs_tree, &rslt_tree);
    treeDump(&rslt_tree);

    treeDtor(&rslt_tree);
    treeDtor(&srs_tree);
    fclose(sourse);
    return 0;
}