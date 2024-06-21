# Variable declaration
CC=g++
CFLAGS=-g -Wall

LDIR =-L/usr/lib/arm-linux-gnueabihf
LIBS=-l:libmysqlcppconn.so -lpcan

SRC=src
OBJ=obj
BIN=bin
SRCS=$(wildcard $(SRC)/*.cpp)
OBJS=$(patsubst $(SRC)/%.cpp, $(OBJ)/%.o, $(SRCS))

TARGET=$(BIN)/main

# Debug build
all:$(TARGET)

# Release build
release: CFLAGS=-Wall -O2 -DNDEBUG
release: clean
release: $(TARGET)

# Create binary files and object files
$(TARGET): $(OBJS)
	@mkdir -p $(BIN)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDIR) $(LIBS)

$(OBJ)/%.o: $(SRC)/%.cpp
	@mkdir -p $(OBJ)
	$(CC) $(CFLAGS) -c $< -o $@

# Additional build commands
clean:
	$(RM) -r $(BIN)/* $(OBJ)/*
