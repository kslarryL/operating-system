TARGET=test_array test_process_ancestors
QEMU_LAUNCH_DIR=../../../linux-stable/

CC_C = gcc

CFLAGS = -Werror -Wall -g -std=c99 -D _GNU_SOURCE -static -pthread

all: clean $(TARGET)

$(TARGET):
	$(CC_C) $(CFLAGS) $@.c -o $@

clean:
	rm -f $(TARGET)

transfer:
	scp -P 10022 $(TARGET) ubuntu@localhost: