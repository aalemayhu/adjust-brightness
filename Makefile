CC = $(CROSS_COMPILE)gcc
CFLAGS += -D_FILE_OFFSET_BITS=64 -Wall -g -O2
CFLAGS += -ludev

PROGS := adjust-brightness

all: $(PROGS)

%: %.c 
	$(CC) $(CFLAGS) $< -o $@

clean:
	$(RM) $(PROGS)

