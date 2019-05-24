
//	This is the driver source for the cache simulator

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simulator.h"

// declarations for parameters
unsigned int cache_size, associativity_size;
bool replacement_flag, writeback_flag;

int main(int argc, char **argv)
{
	FILE * trace_inp;
	// print format if arguments aren't met
	if (argc != 6) {
		fprintf(stderr, "Input format is as follows:");
		fprintf(stderr, "<CACHE_SIZE> <ASSOC> <REPLACEMENT> <WB> <TRACEFILE>\n");

		return -1;
	}

	// open the trace input file for reading
	if (!(trace_inp = fopen(argv[5], "r")))
	{
		fprintf(stderr, "Could not open \"%s\"\n", argv[5]);
		return -1;
	}

	// set parameters
	cache_size = atoi(argv[1]);
	associativity_size = atoi(argv[2]);
	replacement_flag = strcmp(argv[3], "0") ? true : false;
	writeback_flag = strcmp(argv[4], "0") ? true : false;

	// output
	fprintf(stdout, "Starting params:\n");
	fprintf(stdout, "SZ: %u\tASSOC: %u\t%s\t%s\n\n", cache_size, associativity_size, replacement_flag ? "FIFO" : "LRU", writeback_flag ? "WRITEBACK" : "WRITETHRU");

	// run simulator
	int err = simulateCache(trace_inp);

	// print error
	if (err == -1) fprintf(stderr, "Memory allocation failure!\n");

	// close the trace input file
	if (trace_inp) fclose(trace_inp);

	return err;
}