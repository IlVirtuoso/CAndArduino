CC=gcc
test-list: test-list.c list-module.h list-module.c
	gcc -std=c89  -Wpedantic -Wall -Werror  -o list test-list.c list-module.c

string-kids-alarm: string-kids-alarm.c 
	gcc -std=c89 -Wpedantic -Wall -Werror -o string-kids-alarm string-kids-alarm.c 

char-loop: char-loop.c
	gcc -std=c89  -Wpedantic -Wall -Werror -o char-loop char-loop.c