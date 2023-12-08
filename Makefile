LDFLAGS=-lasound

tick: tick.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

clean:
	rm tick tick.o
