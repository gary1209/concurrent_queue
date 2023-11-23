CC=g++-13
CFLAGS=-fopenmp -Wall
LIBS=-lpthread

# Original Executables
TARGET1=main
TARGET2=main2
TARGET3=main3

# Benchmark Executables
BENCH1=bench1
BENCH2=bench2
BENCH3=bench3

# Original Source files
SOURCES1=main.c LockBasedQueue.c
SOURCES2=main2.cpp
SOURCES3=main3.cpp LockOmpQueue.c

# Benchmark Source files
BSOURCES1=bench1.cpp LockBasedQueue.c
BSOURCES2=bench2.cpp
BSOURCES3=bench3.cpp LockOmpQueue.c

# Original Object files
OBJECTS1=$(SOURCES1:.c=.o)
OBJECTS2=$(SOURCES2:.cpp=.o)
OBJECTS3=$(SOURCES3:.cpp=.o)
OBJECTS3:=$(OBJECTS3:.c=.o)

# Benchmark Object files
BOBJECTS1=$(BSOURCES1:.c=.o)
BOBJECTS1:=$(BOBJECTS1:.cpp=.o)
BOBJECTS2=$(BSOURCES2:.cpp=.o)
BOBJECTS3=$(BSOURCES3:.cpp=.o)
BOBJECTS3:=$(BOBJECTS3:.c=.o)

# Default target
all: $(TARGET1) $(TARGET2) $(TARGET3) $(BENCH1) $(BENCH2) $(BENCH3)

# Rules to link the original programs
$(TARGET1): $(OBJECTS1)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

$(TARGET2): $(OBJECTS2)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

$(TARGET3): $(OBJECTS3)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# Rules to link the benchmark programs
$(BENCH1): $(BOBJECTS1)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

$(BENCH2): $(BOBJECTS2)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

$(BENCH3): $(BOBJECTS3)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# Rule to compile C source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to compile C++ source files
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(TARGET1) $(OBJECTS1) $(TARGET2) $(OBJECTS2) $(TARGET3) $(OBJECTS3) $(BENCH1) $(BOBJECTS1) $(BENCH2) $(BOBJECTS2) $(BENCH3) $(BOBJECTS3)

# Phony targets
.PHONY: all clean
