.PHONY: all clean

CFLAGS += -Wall -Werror -MD

SRCS = struct.c struct-pack.c struct-unpack.c utils.c

all: tobin binto

tobin: struct.o struct-pack.o utils.o
	$(CC) $(LDFLAGS) -o $@ $^

binto: struct.o struct-unpack.o utils.o
	$(CC) $(LDFLAGS) -o $@ $^

clean:
	-rm -f *.o *.d tobin binto

-include $(SRCS:.c=.d)
