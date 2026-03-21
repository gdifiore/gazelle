CC     = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -I..

TESTS = test_uart_buffer test_ipc

all: $(TESTS)

test_uart_buffer: test_uart_buffer.c ../drivers/uart_buffer.c
	$(CC) $(CFLAGS) -o $@ $^

test_ipc: test_ipc.c ../drivers/ipc.c ../tinylibc/string.c stubs.c
	$(CC) $(CFLAGS) -o $@ $^

test: all
	@echo ""
	@./test_uart_buffer; r1=$$?; \
	 echo ""; \
	 ./test_ipc; r2=$$?; \
	 echo ""; \
	 exit $$((r1 | r2))

clean:
	rm -f $(TESTS)

.PHONY: all test clean
