all: r_server r_client rclient1 rclient2
r_server: r_server.c
	gcc -g -Wall -o r_server r_server.c
r_client: r_client.c r_client.h
	gcc -g -Wall -c r_client.c
rclient1: r_client rclient1.c
	gcc -g -Wall -o rclient1 rclient1.c r_client.o
rclient2: r_client rclient2.c
	gcc -g -Wall -o rclient2 rclient2.c r_client.o
clean:
	rm r_client.o r_server rclient1 rclient2
submission:
	gtar czvf prog4.tgz Makefile r_server.c r_client.h r_client.c rclient1.c rclient2.c
