CC = g++
DEBUG__FLAGS = -fsanitize=address,leak,undefined -Wall -g

all: build

build: main.o differentiator.o logsLib.o textLib.o tree.o texDump.o
	$(CC) main.o differentiator.o logsLib.o textLib.o tree.o texDump.o -o diff

debug: main.o differentiator.o logsLib.o textLib.o tree.o texDump.o
	$(CC) main.o differentiator.o logsLib.o textLib.o tree.o texDump.o -o diff $(DEBUG__FLAGS)

main.o: main.cpp
	$(CC) -c main.cpp
differentiator.o: Tree/differentiator.cpp
	$(CC) -c Tree/differentiator.cpp
logsLib.o: Logger/logsLib.cpp
	$(CC) -c Logger/logsLib.cpp
textLib.o: TextLib/textLib.cpp
	$(CC) -c TextLib/textLib.cpp
tree.o: Tree/tree.cpp
	$(CC) -c Tree/tree.cpp
texDump.o: Logger/texDump.cpp
	$(CC) -c Logger/texDump.cpp

clean:
	rm -rf *.o diff
