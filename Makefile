all: parsing2.c
	cc -std=c99 -Wall parsing2.c mpc.c -ledit -lm -o parsing
