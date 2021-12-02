#include <stdio.h>
#include "Tree/tree.h"
#include "Include/textLib.h"
#include "Include/logsLib.h"
#include "Include/differentiator.h"



int main(int argc, const char **argv)
{
    Tree srs_tree = {};
    Tree rslt_tree = {};
    treeCtor(&srs_tree);

    if (argc < 2)
    {
        printf("!!! Can't run without input file !!!\n");
        return -1;
    }
    FILE *sourse = fopen(argv[1], "r");

    beginTex();

    readFormula(sourse, &srs_tree);
    texOrigin(&srs_tree);

    treeCtor(&rslt_tree);

    diffur(&srs_tree, &rslt_tree);
    optimiz(&rslt_tree);

    treeDtor(&rslt_tree);
    treeDtor(&srs_tree);
    finishTex();
    fclose(sourse);
    return 0;
}