CC      = gcc
STD     = -std=c11 -Wall -Wextra -Wpedantic -Werror
SAN     = -fsanitize=address,undefined -fno-omit-frame-pointer -g -O1

.PHONY: all test bench clean
all: test

test: t_stack_array t_stack_list t_queue_circ t_queue_list t_expr
	./t_stack_array
	./t_stack_list
	./t_queue_circ
	./t_queue_list
	./t_expr

t_stack_array: test_stack.c stack_array.c
	$(CC) $(STD) $(SAN) $^ -o $@

t_stack_list: test_stack.c stack_list.c
	$(CC) $(STD) $(SAN) $^ -o $@

t_queue_circ: test_queue.c queue_circ.c
	$(CC) $(STD) $(SAN) $^ -o $@

t_queue_list: test_queue.c queue_list.c
	$(CC) $(STD) $(SAN) $^ -o $@

t_expr: test_expr.c expr.c
	$(CC) $(STD) $(SAN) $^ -lm -o $@

bench:
	$(CC) $(STD) -O2 -DIMPL_NAME='"array"' bench.c stack_array.c queue_circ.c -o bench_array
	$(CC) $(STD) -O2 -DIMPL_NAME='"list"'  bench.c stack_list.c  queue_list.c -o bench_list
	./bench_array
	./bench_list

clean:
	rm -f t_* bench_array bench_list
