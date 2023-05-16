local:
	gcc -g -std=c99 -Wall -Wconversion -Wno-sign-conversion -Werror -Wl,--wrap=malloc -o tp3 *.c
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes  ./tp3
	rm tp3
