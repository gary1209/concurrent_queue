CC=g++-13
CFLAGS=-fopenmp -Wall
LIBS=-lpthread

# Original Executables
TARGET1=main
TARGET2=main2
TARGET3=main3
TARGET4=main4

# Benchmark Executables
BENCH1=bench1
BENCH2=bench2
BENCH3=bench3
BENCH4=bench4

# Original Source files
SOURCES1=main.c LockBasedQueue.c
SOURCES2=main2.cpp
SOURCES3=main3.cpp LockOmpQueue.c
SOURCES4=main4.cpp # Add any other source files for main4 as necessary

# Benchmark Source files
BSOURCES1=bench1.cpp LockBasedQueue.c
BSOURCES2=bench2.cpp
BSOURCES3=bench3.cpp LockOmpQueue.c
BSOURCES4=bench4.cpp # Add any other source files for bench4 as necessary

# Original Object files
OBJECTS1=$(SOURCES1:.c=.o)
OBJECTS2=$(SOURCES2:.cpp=.o)
OBJECTS3=$(SOURCES3:.cpp=.o)
OBJECTS3:=$(OBJECTS3:.c=.o)
OBJECTS4=$(SOURCES4:.cpp=.o)

# Benchmark Object files
BOBJECTS1=$(BSOURCES1:.c=.o)
BOBJECTS1:=$(BOBJECTS1:.cpp=.o)
BOBJECTS2=$(BSOURCES2:.cpp=.o)
BOBJECTS3=$(BSOURCES3:.cpp=.o)
BOBJECTS3:=$(BOBJECTS3:.c=.o)
BOBJECTS4=$(BSOURCES4:.cpp=.o)

# Default target
all: $(TARGET1) $(TARGET2) $(TARGET3) $(TARGET4) $(BENCH1) $(BENCH2) $(BENCH3) $(BENCH4)

# Rules to link the original programs
$(TARGET1): $(OBJECTS1)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

$(TARGET2): $(OBJECTS2)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

$(TARGET3): $(OBJECTS3)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

$(TARGET4): $(OBJECTS4)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# Rules to link the benchmark programs
$(BENCH1): $(BOBJECTS1)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

$(BENCH2): $(BOBJECTS2)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

$(BENCH3): $(BOBJECTS3)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

$(BENCH4): $(BOBJECTS4)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# Rule to compile C source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to compile C++ source files
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(TARGET1) $(OBJECTS1) $(TARGET2) $(OBJECTS2) $(TARGET3) $(OBJECTS3) $(TARGET4) $(OBJECTS4) $(BENCH1) $(BOBJECTS1) $(BENCH2) $(BOBJECTS2) $(BENCH3) $(BOBJECTS3) $(BENCH4) $(BOBJECTS4)

# Phony targets
.PHONY: all clean
