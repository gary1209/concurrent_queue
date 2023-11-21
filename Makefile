CC=g++-13
CFLAGS=-fopenmp -Wall
LIBS=-lpthread

# Name of the executable
TARGET=main

# Source files
SOURCES=main.c LockBasedQueue.c

# Object files
OBJECTS=$(SOURCES:.c=.o)

# Default target
all: $(TARGET)

# Rule to link the program
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# Rule to compile source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(TARGET) $(OBJECTS)

# Phony targets
.PHONY: all clean
