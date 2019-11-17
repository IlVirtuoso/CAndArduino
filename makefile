CC=gcc
test-list: test-list.c list-module.h list-module.c
	gcc -std=c99 -o list test-list.c list-module.c

string-kids-alarm: string-kids-alarm.c 
	gcc -std=c99 -o string-kids-alarm string-kids-alarm.c 

char-loop: char-loop.c
	gcc -std=c99 -o char-loop char-loop.c