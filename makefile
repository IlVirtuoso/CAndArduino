CC=gcc
test-list: test-list.c list-module.h list-module.c
	gcc -std=c89 -o list test-list.c list-module.c

string-kids-alarm: string-kids-alarm.c 
	gcc -std=c89 -o string-kids-alarm string-kids-alarm.c 

char-loop: char-loop.c
	gcc -std=c89 -o char-loop char-loop.c