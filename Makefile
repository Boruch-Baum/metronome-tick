LDFLAGS=-lasound

tick: tick.c lib/config.c lib/config.h
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm tick
