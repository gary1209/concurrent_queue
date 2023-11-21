CC=g++-13
CFLAGS=-fopenmp -Wall
LIBS=-lpthread

# Executables
TARGET1=main
TARGET2=main2

# Source files
SOURCES1=main.c LockBasedQueue.c
SOURCES2=main2.cpp

# Object files
OBJECTS1=$(SOURCES1:.c=.o)
OBJECTS2=$(SOURCES2:.cpp=.o)

# Default target
all: $(TARGET1) $(TARGET2)

# Rules to link the programs
$(TARGET1): $(OBJECTS1)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

$(TARGET2): $(OBJECTS2)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# Rule to compile C source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to compile C++ source files
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(TARGET1) $(OBJECTS1) $(TARGET2) $(OBJECTS2)

# Phony targets
.PHONY: all clean