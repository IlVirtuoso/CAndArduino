CC=gcc
test-list: test-list.c list-module.h list-module.c
	gcc -std=c99 -o list test-list.c list-module.c