
//	This is the header for the cache simulator source code

#ifndef __SIM_H__
#define __SIM_H__

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define ADDRESS_SIZE 64
#define BLOCK_SIZE 64
#define HEX 16

extern unsigned int cache_size, associativity_size;
extern bool replacement_flag, writeback_flag;
// replacement flag: true - FIFO, false - LRU
// writeback flag: true - write-back, false - write through

typedef enum {
	INVALID,
	CLEAN,
	DIRTY
} blockState;

struct block {
	unsigned long long int tag;
	int priority;
	blockState state;
};

int simulateCache(FILE*);

#endif