CC = gcc
CFLAGS = -g -Og -Wall
LDFLAGS = -shared
PREFIX = /usr/local
LIBNAME = sched_deadline

all: sched_deadline

sched_deadline: sched_deadline.o 
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o lib$(LIBNAME).so

%.o : %.cpp
	$(CC) $(CFLAGS) -c $< 

.PHONY: install
install: all
	install -v -m 644 $(LIBNAME).h $(PREFIX)/include/$(LIBNAME).h
	install -v -m 755 lib$(LIBNAME).so $(PREFIX)/lib/lib$(LIBNAME).so

.PHONY: clean
clean:
	rm -f *.o *.so
