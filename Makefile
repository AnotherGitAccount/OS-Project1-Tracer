CC=gcc 
CFLAGS=-g --pedantic -Wall -Wextra -Wmissing-prototypes -std=c99 
PROGRAM=tracer 
# DO NOT MODIFY CC, CFLAGS and PROGRAM
# COMPLETE BELOW
DEPENDENCIES=file.h instruction.h mem_map.h utils.h function_block.h
OBJECTS=tracer.o file.o instruction.o mem_map.o utils.o function_block.o

%.o: %.c$(DEPENDENCIES)
	$(CC) -c -o $@ $< $(CFLAGS)

$(PROGRAM): $(OBJECTS)
	$(CC) -o $@ $^ $(CFLAGS)
	$(MAKE) clean

.PHONY: cleanobj

clean:
	rm *.o
