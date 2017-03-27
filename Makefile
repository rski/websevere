all: websevere.out

run: websevere.out
	./$<

websevere.out: websevere.o
	gcc $^ -o $@ -g -Wall -Werror

websevere.o: websevere.c
