all: recordio testio indexer
recordio: recordio.c recordio.h recordTests.c
	gcc -Wall -c recordio.c
	gcc -Wall -o recordio recordio.o recordTests.c
testio: recordio testio.c
	gcc -Wall -o testio testio.c recordio.o
indexer: indexer.c
	gcc -Wall -o indexer indexer.c
clean:
	rm ./*.o recordio testio indexer
submission:
	gtar czvf prog2.tgz Makefile recordio.c recordio.h testio.c indexer.c recordTests.c
