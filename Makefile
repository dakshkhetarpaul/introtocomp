# Define the compiler
CC=gcc

# Define compile-time flags
CFLAGS=-Wall

# Define link-time flags
LDFLAGS=-lm

# Define the source files
SRC=main.c generate_file.c data_processing.c ProcessManagement.c IPC.c

# Define the object files
OBJ=$(SRC:.c=.o)

# Define the executable file 
MAIN=main

# The default target
all: $(MAIN)
	@echo Simple compiler named main has been compiled

# Link the program
$(MAIN): $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $(MAIN)

# Compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean target
clean:
	rm -f $(MAIN) $(OBJ)

.PHONY: all clean
