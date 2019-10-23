CC=gcc

OBJ=timer
all:$(OBJ)

HERE=$(shell pwd)
incs=-I$(HERE)/src

files:=$(HERE)/src/main.c
files+=$(HERE)/src/Timer.c
files+=$(HERE)/src/TimerManager.c

OBJS_C := $(patsubst %.c,%.o,$(files))


CFLAGS=-O0 -g 
CFLAGS+=$(incs) 

LDFLAGS=-lpthread 
LDFLAGS+=-ldl 

$(OBJ):$(OBJS_C)
	$(CC)  $(LDFLAGS) -o $@ $^ 
			
$(OBJS_C):%.o:%.c
	$(CC) $(CFLAGS)  -c $< -o $@

clean:
	rm -rf $(HERE)/*.o
	rm -rf $(HERE)/$(OBJ)