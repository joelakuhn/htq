CCFLAGS = -Werror -std=c99 -IModest/source/

LINK_FLAGS = -lm -pthread

SOURCES=$(wildcard source/*.c)

OBJECTS=$(SOURCES:.c=.o)

MODEST_FLAGS =


all: htq


debug: CFLAGS += -g
debug: MODEST_FLAGS += MyCORE_BUILD_DEBUG=YES
debug: htq


htq: $(OBJECTS) Modest/lib/libmodest_static.a
	$(CC) $(LINK_FLAGS) $(CCFLAGS) $(OBJECTS) Modest/lib/libmodest_static.a -o htq

source/%.o: source/%.c
	$(CC) $(CCFLAGS) -c $^ -o $@

Modest/lib/libmodest_static.a:
	$(MAKE) -C Modest library $(MODEST_FLAGS)

clean:
	rm -rf htq*
	rm source/*.o
	$(MAKE) -C Modest clean

