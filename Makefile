OBJECTS = main.c
CFLAGS = `pkg-config --libs --cflags gtk+-2.0`

default: tiktok

%.o: %.c $(HEADERS)
	gcc $(CFLAGS) -c $< -o $@

tiktok: $(OBJECTS)
	gcc $(CFLAGS) $(OBJECTS) -o $@

clean:
	-rm -f $(OBJECTS)
	-rm -f tiktok
