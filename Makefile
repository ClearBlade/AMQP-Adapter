CC = gcc

CFLAGS = -g -Wall -Werror -D_POSIX_SOURCE -D_GNU_SOURCE -std=c99

INCLUDES = -I/adapter

LIBS = -lpthread -lpaho-mqtt3c

SRC1 = adapter

SRCS = main.c $(wildcard $(SRC1)/*.c)

OBJS = $(SRCS:.c=.o)

MAIN = main 

all: $(MAIN)
	@echo Compile complete. Execute main by ./main

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LIBS)

clean:
	$(RM) *.o adapter/*.o $(MAIN)