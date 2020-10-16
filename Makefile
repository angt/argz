all: libargz.so libargz.a

libargz.so: shared

libargz.a: static

shared: argz.o
	$(CC) -shared -o libargz.so argz.o

static: argz.o
	$(AR) rcs libargz.a argz.o

argz.o:
	$(CC) -c -o argz.o argz.c -Wall -Wextra -fPIC

exemple_static: static
	$(CC) -o readme readme.c -largz -L. -I.
	@echo "To launch './readme'"

exemple_shared: shared
	$(CC) -o readme readme.c -largz -L. -I.
	@echo "To launch on Linux: 'LD_LIBRARY_PATH=/path/to/folder/of/libargz.so ./readme'"
	@echo "To launch on macOs: 'DYLD_LIBRARY_PATH=/path/to/folder/of/libargz.so ./readme'"

clean:
	$(RM) -f readme argz.o libargz.so libargz.a

.PHONY: all clean static shared exemple_static exemple_shared
