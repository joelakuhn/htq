CFLAGS = -lm -pthread -std=c99 -IModest/source/

htq: main.c Modest/lib/libmodest_static.a
	cc $(CFLAGS) main.c Modest/lib/libmodest_static.a -o htq

Modest/lib/libmodest_static.a:
	$(MAKE) -C Modest library

clean:
	rm htq
	$(MAKE) -C Modest clean

