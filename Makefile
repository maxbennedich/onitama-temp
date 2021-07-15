C        := -clang
CFLAGS   := -Wall -Wextra -march=native -Ofast
CXX      := -clang++
CXXFLAGS := -std=c++17 -mlzcnt $(CFLAGS)
OBJ_DIR  := obj

tb: TablebaseGenerator.cpp $(OBJ_DIR)/7zFile.o $(OBJ_DIR)/7zStream.o $(OBJ_DIR)/Alloc.o $(OBJ_DIR)/LzFind.o $(OBJ_DIR)/LzmaDec.o $(OBJ_DIR)/LzmaEnc.o
	@mkdir -p dtm
	$(CXX) $(CXXFLAGS) -c -o $(OBJ_DIR)/TablebaseGenerator.o TablebaseGenerator.cpp
	$(CXX) $(CXXFLAGS) -pthread -o tb $(OBJ_DIR)/TablebaseGenerator.o $(OBJ_DIR)/7zFile.o $(OBJ_DIR)/7zStream.o $(OBJ_DIR)/Alloc.o $(OBJ_DIR)/LzFind.o $(OBJ_DIR)/LzmaDec.o $(OBJ_DIR)/LzmaEnc.o

# This is for the LZMA library which is written in C
$(OBJ_DIR)/%.o: lzma/%.c
	@mkdir -p $(@D)
	$(C) -c -o $@ $< $(CFLAGS)
