CC = cc -Wall -Wextra -O3
INC = -I./include
LIB = $(wildcard ./include/*.h)
OUT = hsll
SRC = $(wildcard ./src/*.c ./vshkh/*.c ./vshcfp/*.c)
OBJ = $(SRC:.c=.o)

$(OUT): $(OBJ)
	$(CC) $(OBJ) $(INC) -o $(OUT)
	rm ./src/hsll.o

%.o: %.c $(LIB)
	$(CC) -c $< $(INC) -o $@

clean:
	rm $(OBJ)

install: $(OUT)
	mv $(OUT) ~/.local/bin/$(OUT)
	chmod +x ~/.local/bin/$(OUT)
