CCFLAGS = -Werror -std=c99 -IModest/source/

LINK_FLAGS = -lm -pthread

SOURCES=$(wildcard source/*.c)
OBJECTS=$(SOURCES:.c=.o)

MODEST_FLAGS =
MODEST_ARCHIVE = Modest/lib/libmodest_static.a


all: htq


debug: CCFLAGS += -g3
debug: MODEST_FLAGS += MyCORE_BUILD_DEBUG=YES
debug: htq


htq: Modest/lib/libmodest_static.a $(OBJECTS)
	$(CC) $(LINK_FLAGS) $(CCFLAGS) $(OBJECTS) $(MODEST_ARCHIVE) -o htq

source/%.o: source/%.c
	$(CC) $(CCFLAGS) -c $^ -o $@

Modest/lib/libmodest_static.a:
	$(MAKE) -C Modest static $(MODEST_FLAGS)

shallowclean:
	rm -rf htq*
	rm -f source/*.o

clean: shallowclean
	$(MAKE) -C Modest clean

