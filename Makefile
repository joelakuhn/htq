CFLAGS = -Werror -lm -pthread -std=c99 -IModest/source/

htq: main.c css_engine.c readfile.c str_arr.c Modest/lib/libmodest_static.a
	cc $(CFLAGS) main.c Modest/lib/libmodest_static.a -o htq

Modest/lib/libmodest_static.a:
	$(MAKE) -C Modest library

clean:
	rm htq
	$(MAKE) -C Modest clean

