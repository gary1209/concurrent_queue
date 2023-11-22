CC=g++-13
CFLAGS=-fopenmp -Wall
LIBS=-lpthread

# Executables
TARGET1=main
TARGET2=main2
TARGET3=main3

# Source files
SOURCES1=main.c LockBasedQueue.c
SOURCES2=main2.cpp
SOURCES3=main3.cpp LockOmpQueue.c

# Object files
OBJECTS1=$(SOURCES1:.c=.o)
OBJECTS2=$(SOURCES2:.cpp=.o)
OBJECTS3=$(SOURCES3:.cpp=.o)
OBJECTS3:=$(OBJECTS3:.c=.o)

# Default target
all: $(TARGET1) $(TARGET2) $(TARGET3)

# Rules to link the programs
$(TARGET1): $(OBJECTS1)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

$(TARGET2): $(OBJECTS2)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

$(TARGET3): $(OBJECTS3)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# Rule to compile C source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to compile C++ source files
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(TARGET1) $(OBJECTS1) $(TARGET2) $(OBJECTS2) $(TARGET3) $(OBJECTS3)

# Phony targets
.PHONY: all clean
