CC = gcc
CFLAGS = -g -w

all: riscv_sim

riscv_sim: main.c lab7_AI_13_15.c
	$(CC) $(CFLAGS) -o riscv_sim main.c lab7_AI_13_15.c

clean:
	rm -f riscv_sim
