#include <stdio.h>
#include "Tree.h"
#include "TextLib.h"
#include "LogsLib.h"
#include "Differentiator.h"



int main(int argc, const char **argv)
{
    Tree srs_tree = {};
    Tree rslt_tree = {};

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
    texDump(&rslt_tree);

    Tree rslt_tree1 = {};
    treeCtor(&rslt_tree1);
    diffur(&srs_tree, &rslt_tree1);
    optimiz(&rslt_tree1);

    /*Tree rslt_tree2 = {};
    treeCtor(&rslt_tree2);
    diffur(&rslt_tree1, &rslt_tree2);
    optimiz(&rslt_tree2);

    Tree rslt_tree3 = {};
    treeCtor(&rslt_tree3);
    diffur(&rslt_tree2, &rslt_tree3);
    optimiz(&rslt_tree3);

    Tree rslt_tree4 = {};
    treeCtor(&rslt_tree4);
    diffur(&rslt_tree3, &rslt_tree4);
    optimiz(&rslt_tree4);

    Tree rslt_tree5 = {};
    treeCtor(&rslt_tree5);
    diffur(&rslt_tree4, &rslt_tree5);
    optimiz(&rslt_tree5);

    Tree rslt_tree6 = {};
    treeCtor(&rslt_tree6);
    diffur(&rslt_tree5, &rslt_tree6);
    optimiz(&rslt_tree6);*/

    double res = calcDiff(&rslt_tree1, 0);
    texDump(&rslt_tree1);
    printf("%lg\n", res);

    treeDtor(&rslt_tree);
    treeDtor(&rslt_tree1);
    /*treeDtor(&rslt_tree2);
    treeDtor(&rslt_tree3);
    treeDtor(&rslt_tree4);
    treeDtor(&rslt_tree5);
    treeDtor(&rslt_tree6);*/
    treeDtor(&srs_tree);
    finishTex();
    fclose(sourse);
    return 0;
}