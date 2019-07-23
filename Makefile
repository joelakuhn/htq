CFLAGS = -Werror -lm -pthread -std=c99 -IModest/source/

MODEST_FLAGS =

all: htq

debug: CFLAGS += -g
debug: MODEST_FLAGS += MyCORE_BUILD_DEBUG=YES
debug: htq

htq: main.c css_engine.c readfile.c str_arr.c Modest/lib/libmodest_static.a
	$(CC) $(CFLAGS) main.c Modest/lib/libmodest_static.a -o htq

Modest/lib/libmodest_static.a:
	$(MAKE) -C Modest library $(MODEST_FLAGS)

clean:
	rm -rf htq*
	$(MAKE) -C Modest clean

