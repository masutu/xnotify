CC = gcc
CFLAGS = -Wall -g 
LIBS = -lX11
EXECS = screen_info hello_win
all: $(EXECS)

% : %.c
	$(CC) $(LIBS) $(CFLAGS) $< -o $@

.PHONY: clean

clean:
	rm -f $(EXECS)

