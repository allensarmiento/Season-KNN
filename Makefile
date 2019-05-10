CC=gcc
CFLAGS=-fopenmp
LIBS=-lm
DEPS = readData.h utility.h structs.h KnnSequential.h KnnParallel.h

main: predict-seasons.c $(DEPS)
	$(CC) $(CFLAGS) predict-seasons.c -o main $(LIBS)

clean:
	rm main
