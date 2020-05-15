CC = gcc
CROSS_CC = arm-none-guneabi-linux

endpoint_PC:endpoint.c
	$(CC) endpoint.c -o endpoint

endpoint_ARM:endpoint.c
	$(CROSS_CC) endpoint.c -o endpoint

clean:
	rm -rf *.o endpoint
