# Concurrent Queue

# Complie the program (On CIMS)

`module avail gcc`

`module load gcc-11.2`

# Update makefile line 1 to your g++ cmd

`make`

## After the make command, you will compile 6 sample programs and 6 benchmark programs.

# Run the program

`./bench1`  to run benchmark for lock-based quque (pthread mutex)

`./bench3`  to run benchmark for lock-based quque (OpenMP lock)

`./bench4`  to run benchmark for lock-free basic quque

`./bench5`  to run benchmark for lock-free optimized1 quque

`./bench6`  to run benchmark for lock-free optimized2 quque