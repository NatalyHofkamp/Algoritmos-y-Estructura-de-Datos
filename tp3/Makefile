local: compile tests
	rm tp3

tests_queryable_dict: compile_queryable_dict tests
	rm tp3

tests_internal_iterator: compile_internal_iterator tests
	rm tp3

tests_operable_dict: compile_operable_dict tests
	rm tp3

EXTRAS_SRC_FILES = $(filter-out $(wildcard tests*.c), $(wildcard *.c))
QUERYABLE_SRC_FILES = $(EXTRAS_SRC_FILES) tests_queryable_dict.c
OPERABLE_SRC_FILES = $(EXTRAS_SRC_FILES) tests_operable_dict.c
ITERATOR_SRC_FILES = $(EXTRAS_SRC_FILES) tests_internal_iterator.c
VANILLA_SRC_FILES = $(EXTRAS_SRC_FILES) tests.c

compile:
	gcc -g -std=c99 -Wall -Wconversion -Wno-sign-conversion -Werror -Wl,--wrap=malloc -o tp3 $(VANILLA_SRC_FILES)

compile_queryable_dict:
	gcc -g -std=c99 -Wall -Wconversion -Wno-sign-conversion -Werror -Wl,--wrap=malloc -o tp3 $(QUERYABLE_SRC_FILES)

compile_internal_iterator:
	gcc -g -std=c99 -Wall -Wconversion -Wno-sign-conversion -Werror -Wl,--wrap=malloc -o tp3 $(ITERATOR_SRC_FILES)

compile_operable_dict:
	gcc -g -std=c99 -Wall -Wconversion -Wno-sign-conversion -Werror -Wl,--wrap=malloc -o tp3 $(OPERABLE_SRC_FILES)

tests:
	valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all --track-origins=yes -s ./tp3

unoptimized_compile:
	gcc -O0 -std=c99 -Wall -Wconversion -Wno-sign-conversion -Werror -Wl,--wrap=malloc -o tp3_bench $(VANILLA_SRC_FILES)

measure_time:
	/usr/bin/time -f "Measurements: time=%e maxmem=%M" ./tp3_bench 2>&1 1>/dev/null

clean_time:
	rm tp3_bench

time: unoptimized_compile measure_time clean_time

docker:
	docker build --tag udesa_tp3 .
	docker run -v $(CURDIR):/tp --rm -it --entrypoint bash udesa_tp3

clean_docker:
	docker rmi -f $(docker images | grep udesa_tp3 | tr -s ' ' | cut -d ' ' -f 3)