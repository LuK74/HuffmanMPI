FLAGS = -g -Wall -Werror
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
MKDIR = mkdir

CC = mpic++

all : directories huffman

directories : $(OBJ_DIR) $(BIN_DIR)

$(OBJ_DIR) :
	$(MKDIR) $(OBJ_DIR)

$(BIN_DIR) :
	$(MKDIR) $(BIN_DIR)

#### main exec

huffman : huffman.o tree.o simpl_tbl.o
	@echo "\nLinking objects"
	$(CC) -g -o $(BIN_DIR)/$@ $(OBJ_DIR)/huffman.o $(OBJ_DIR)/tree.o $(OBJ_DIR)/simpl_tbl.o

#### .o

huffman.o : $(SRC_DIR)/huffman.cpp
	$(CC) -c $(FLAGS) $(SRC_DIR)/huffman.cpp -o $(OBJ_DIR)/huffman.o

tree.o : $(SRC_DIR)/tree.cpp
	$(CC) -c $(FLAGS) $(SRC_DIR)/tree.cpp -o $(OBJ_DIR)/tree.o

simpl_tbl.o : $(SRC_DIR)/simpl_tbl.cpp
	$(CC) -c $(FLAGS) $(SRC_DIR)/simpl_tbl.cpp -o $(OBJ_DIR)/simpl_tbl.o


#### clear / clean

clean :
	@echo "Cleaning obj dir and bin dir\n"
	rm -r $(OBJ_DIR)/
	rm -r $(BIN_DIR)/
