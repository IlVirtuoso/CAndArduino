CC=gcc

master: master.c ./resources/libs/parameters.h  ./resources/libs/piece.h ./resources/libs/piece.c ./resources/libs/player.h ./resources/libs/player.c
	gcc -std=c89  -Wpedantic -Wall -Werror  -o master master.c  ./resources/libs/piece.c ./resources/libs/player.c