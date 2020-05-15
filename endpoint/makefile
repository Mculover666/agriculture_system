CC = gcc
CROSS_CC = arm-linux-guneabi-gcc

endpoint_PC:endpoint.c
	$(CC) endpoint.c -o endpoint

endpoint_ARM:endpoint.c
	$(CROSS_CC) endpoint.c -o endpoint

clean:
	rm -rf *.o endpoint
