all:
	cc -lrt -o c client.c
	cc -lrt -o s run.c
clean:
	rm s c
