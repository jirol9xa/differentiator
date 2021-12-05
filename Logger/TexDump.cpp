#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <cmath>
#include "LogsLib.h"
#include "Tree.h"
#include "Differentiator.h"


int beginTex()
{
    openLogs("LOGS/Dump.tex");
    
    writeLogs("\\documentclass{article}\n"
               "\\usepackage{graphicx}\n"
               "\\begin{document}\n");
    writeLogs("\\title{Taking a derivative according to all the rules of mat.analysis}\n");
    writeLogs("\\maketitle\n");
    writeLogs("\\section{Some info}\n");
    writeLogs("This program is designed to satisfy true nerds... lovers of matanalysis."
              "All actions performed will be commented on and explained in detail.\n");

    writeLogs("\\section{Taking a derivative}");

    return 0;
}


int finishTex()
{
    writeLogs("\\end{document}");
    closeLogs();

    system("latex LOGS/Dump.tex");

    return 0;
}


int texDump(Tree *tree)
{
    assert(tree);

    writeLogs("\\subsection{Obviously\n");
    printTree(tree->root);
    writeLogs("\n}\n");

    return 0;
}


int texOrigin(Tree *tree)
{
    assert(tree);

    writeLogs("\\subsection{Originally\n");
    printTree(tree->root);
    writeLogs("\n}\n");

    return 0;
}
