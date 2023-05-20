local: compile tests
	rm tp3


compile:
	gcc -g -std=c99 -Wall -Wconversion -Wno-sign-conversion -Werror -Wl,--wrap=malloc -o tp3 *.c

tests:
	valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all --track-origins=yes ./tp3

time:
	gcc -O0 -std=c99 -Wall -Wconversion -Wno-sign-conversion -Werror -Wl,--wrap=malloc -o tp3_bench *.c
	echo "first run"
	time ./tp3_bench 2>&1 1>/dev/null
	echo "second run (there should be no caching and take the same time)"
	time ./tp3_bench 2>&1 1>/dev/null
	rm tp3_bench
