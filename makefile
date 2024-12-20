CC = cc -Wall -Wextra -O3
# -O0 -fsanitize=address,null -g
INC = -I./include
LIB = $(wildcard ./include/*.h)
OUT = hsll
SRC = $(wildcard ./src/*.c ./vshkh/*.c ./vshcfp/*.c)
OBJ_DIR = ./objs
OBJ = $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRC))

$(OUT): $(OBJ)
	mkdir -p $(OBJ_DIR)
	$(CC) $(OBJ) $(INC) -o $(OUT)
	wc */*.{c,h} > wc.txt
	rm ./objs/src/hsll.o

$(OBJ_DIR)/%.o: %.c $(LIB)
	mkdir -p $(dir $@)
	$(CC) -c $< $(INC) -o $@

clean:
	rm -rf $(OBJ_DIR)

install: $(OUT)
	mv $(OUT) ~/.local/bin/$(OUT)
	chmod +x ~/.local/bin/$(OUT)
