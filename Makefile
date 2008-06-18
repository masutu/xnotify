CC = gcc
CFLAGS =  -g  -Ilibev
LIBS = -lX11 -lm
EXECS = screen_info hello_win xnotify
all: $(EXECS)

% : %.c
	$(CC) $(LIBS) $(CFLAGS) $< -o $@

.PHONY: clean

clean:
	rm -f $(EXECS)

