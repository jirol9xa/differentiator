CC = g++
DEBUG__FLAGS = -fsanitize=address,leak,undefined -Wall -g

all: build

build: diff_main.o diff.o logsLib.o textLib.o Tree.o texDump.o
	$(CC) diff_main.o diff.o logsLib.o textLib.o Tree.o texDump.o -o diff

debug: diff_main.o diff.o logsLib.o textLib.o Tree.o texDump.o
	$(CC) diff_main.o diff.o logsLib.o textLib.o Tree.o texDump.o -o diff $(DEBUG__FLAGS)

diff_main.o: diff_main.cpp
	$(CC) -c diff_main.cpp
diff.o:	diff.cpp
	$(CC) -c diff.cpp
logsLib.o: logsLib.cpp
	$(CC) -c logsLib.cpp
textLib.o: textLib.cpp
	$(CC) -c textLib.cpp
Tree.o: Tree/Tree.cpp
	$(CC) -c Tree/Tree.cpp
texDump.o: texDump.cpp
	$(CC) -c texDump.cpp

clean:
	rm -rf *.o diff
