APP=ichigodotshow

all:      $(APP)

GLOBAL_DEP  = lpc21isp.h lpcterm.h
CC = gcc

ifneq ($(findstring(freebsd, $(OSTYPE))),)
CFLAGS+=-D__FREEBSD__
endif

ifeq ($(OSTYPE),)
OSTYPE		= $(shell uname)
endif

CFLAGS		+= -Wall

ifneq ($(findstring Darwin,$(OSTYPE)),)
CFLAGS+=-D__APPLE__
else
CFLAGS		+= -static
endif

lpcterm.o: lpcterm.c $(GLOBAL_DEP)
	$(CC) $(CDEBUG) $(CFLAGS) -c -o lpcterm.o lpcterm.c

lpc21isp.o: lpc21isp.c $(GLOBAL_DEP)
	$(CC) $(CDEBUG) $(CFLAGS) -c -o lpc21isp.o lpc21isp.c

main.o: main.c $(GLOBAL_DEP)
	$(CC) $(CDEBUG) $(CFLAGS) -c -o main.o main.c

$(APP): main.o lpc21isp.o lpcterm.o $(GLOBAL_DEP)
	$(CC) $(CDEBUG) $(CFLAGS) -o $(APP) lpc21isp.o lpcterm.o main.o

clean:
	$(RM) make.o lpcterm.o lpc21isp.o $(APP)
