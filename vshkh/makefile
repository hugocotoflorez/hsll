INCL = ./include
SRC = ./src/*.c
FLAGS = -Wall -Wextra -fsanitize=address,null -O0 -g
CC = gcc

debug:
	 $(CC) $(SRC) ./test/test.c $(FLAGS) -I $(INCL) -o test.out
	 ./test.out
	 rm ./test.out

example:
	 $(CC) $(SRC) ./examples/example.c $(FLAGS) -I $(INCL) -o example.out
	./example.out
