SOURCE	= $(wildcard ./src/*.c)
OBJS	= $(SOURCE:.c=.o)
CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = -static
ifeq ($(OS),Windows_NT)
	SHELL=cmd
	RM = del /S
	DEL_OBJS = *.o *.exe *.out
	OUT	= file2c.exe
else
	RM = rm -f
	DEL_OBJS = $(OBJS) ./file2c ./file2c.exe
	OUT	= file2c
endif

all: $(OUT)

$(OUT): $(OBJS)
	$(CC) $^ $(LFLAGS) -o $@

%.o: %.c
	$(CC) $< $(FLAGS) -o $@

.PHONY : clean
clean:
	$(RM) $(DEL_OBJS)