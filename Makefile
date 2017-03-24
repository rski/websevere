run: websevere.out
	./$<

websevere.out: websevere.o
	gcc $^ -o $@

websevere.o: websevere.c
