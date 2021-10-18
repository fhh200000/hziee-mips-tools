CC := gcc
CFLAGS := -Iinclude -O2 -DBIG_ENDIAN
SRCS := $(wildcard *.c) 
OBJS := $(patsubst %c,%c.o,$(SRCS))

all:${OBJS} linkage
%.c.o:%.c
	@echo -e "\e[32;1m[CC]\e[0m\t" $<
	@$(CC) ${CFLAGS} -c $< -o build/$@
linkage:
	@echo -e "\e[34;1m[CCLD]\e[0m	" fhhobjdump
	@$(CC) -o build/fhhobjdump build/*.o
clean:
	@echo -e "\e[31;1m[RM]\e[0m	" all
	@rm build/* 2> /dev/null || echo > /dev/null 
