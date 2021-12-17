CFLAGS=-Wall -Wextra -Werror -O2
TARGETS=lab1test lab1tvhN3249 libtvhN3249.so libtvhN3249-2.so 

.PHONY: all clean

all: $(TARGETS)

clean:
	rm -rf *.o $(TARGETS)

lab1test: lab1test.c plugin_api.h
	gcc $(CFLAGS) -o lab1test lab1test.c -ldl

lab1tvhN3249: lab1tvhN3249.c plugin_api.h
	gcc $(CFLAGS) -o lab1tvhN3249 lab1tvhN3249.c -ldl

libtvhN3249.so: libtvhN3249.c plugin_api.h
	gcc $(CFLAGS) -shared -fPIC -o libtvhN3249.so libtvhN3249.c -ldl -lm

libtvhN3249-2.so: libtvhN3249-2.c plugin_api.h
	gcc $(CFLAGS) -shared -fPIC -o libtvhN3249-2.so libtvhN3249-2.c -ldl -lm

