run: websevere.out
	./$<

websevere.out: websevere.o
	gcc $^ -o $@ -g

websevere.o: websevere.c
