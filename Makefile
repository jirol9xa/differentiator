main:
	g++ diff_main.cpp diff.cpp Tree/Tree.cpp textLib.cpp texDump.cpp logsLib.cpp -o diff 
debug:
	g++ diff_main.cpp diff.cpp Tree/Tree.cpp textLib.cpp texDump.cpp logsLib.cpp -o diff -fsanitize=address,leak,undefined