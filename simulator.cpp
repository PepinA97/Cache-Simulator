
//	This is the source code for the cache simulator

#include "simulator.h"

// 2d array of cache, [index of set][block in set]
block **blocks;

// declare counters (read & write is when data is read/written from/to lower memory)
unsigned int access_counter = 0, hit_counter = 0, miss_counter = 0, write_counter = 0, read_counter = 0;

// **DEBUG** (prints counters & contents of first set, unused)
char states[][20] = {"INVALID", "CLEAN", "DIRTY"};
void debug(block * set)
{
	fprintf(stderr, "ACC:%d HIT:%d MIS:%d WRI:%d REA:%d\n", access_counter, hit_counter, miss_counter, write_counter, read_counter);
	for (int i = 0; i < associativity_size; i++)
		fprintf(stderr, "%d\t\t%s\t\t%llu\t\t\t%d\n", i, states[set[i].state], set[i].tag, set[i].priority);
	fprintf(stderr, "\n");
}

void print_output()
{
	double miss_ratio = (double)miss_counter / (double)access_counter; // miss-ratio = misses / (misses + hits)
	fprintf(stdout, "MR: %lf\tWRITES: %u\tREADS: %u\n", miss_ratio, write_counter, read_counter);
}

int getCacheHit(unsigned long long int tag, block * set)
{
	for (int i = 0; i < associativity_size; i++) {
		if (set[i].state != INVALID && set[i].tag == tag) { // on hit
			if (!replacement_flag) // on LRU (when accessed, update priority)
				set[i].priority = access_counter;
			hit_counter++;
			return i; // return index of cache hit block in set
		}
	}
	miss_counter++;
	return -1; // miss!
}

int locateBlockToUseIndex(block * set)
{
	int index = 0, min_priority = 2147483647; // taken from INT_MAX define
	for (int i = 0; i < associativity_size; i++) {
		// use an invalid block if available
		if (set[i].state == INVALID) {
			index = i;
			break;
		}
		// look for block with lowest priority
		if (set[i].priority < min_priority) {
			min_priority = set[i].priority;
			index = i;
		}
	}
	return index;
}

void handleWriteback(unsigned long long int tag, block * set, bool isRead)
{
	if (isRead) {
		if (getCacheHit(tag, set) == -1) { // on miss
			int index = locateBlockToUseIndex(set);
			if (set[index].state == DIRTY)
				write_counter++;
			set[index].tag = tag;
			set[index].priority = access_counter;
			set[index].state = CLEAN;
			read_counter++;
		}
	}
	else { // is Write
		int index = getCacheHit(tag, set);
		if (index == -1) { // on cache miss
			index = locateBlockToUseIndex(set);
			if (set[index].state == DIRTY) // if the block is dirty (not yet written to memory), write to memory
				write_counter++;
			read_counter++;
		}
		set[index].tag = tag;
		set[index].priority = access_counter;
		set[index].state = DIRTY; // set to dirty (not yet written to memory)
	}
}

void handleWritethru(unsigned long long int tag, block * set, bool isRead)
{
	if (isRead) {
		if (getCacheHit(tag, set) == -1) { // on miss
			int index = locateBlockToUseIndex(set);
			set[index].tag = tag;
			set[index].priority = access_counter;
			set[index].state = CLEAN; // read from lower memory (reads are clean)
			read_counter++;
		}
	}
	else { // is write
		int index = getCacheHit(tag, set);
		if (index == -1) { // on cache miss
			index = locateBlockToUseIndex(set);
			read_counter++;
		}
		set[index].tag = tag;
		set[index].priority = access_counter;
		set[index].state = CLEAN;
		write_counter++; // write to lower memory, block is now clean
	}
}

// simulate cache function (called from main.cpp, returns an error code)
int simulateCache(FILE *trace_inp)
{
	// allocate memory for blocks
	int numBlocks = cache_size / BLOCK_SIZE;
	int numSets = numBlocks / associativity_size;
	blocks = (block**)calloc(numSets, associativity_size * sizeof(block*));
	if (!blocks) return -1; // returns error if allocation fails
	for (int i = 0; i < numSets; i++) {
		blocks[i] = (block*)calloc(associativity_size, sizeof(block));
		if (!blocks[i]) return -1; // returns error if allocation fails
		for (int j = 0; j < associativity_size; j++) { // initialize each block
			blocks[i][j].state = INVALID;
			blocks[i][j].priority = -1;
		}
	}

	// scan each line of trace input file
	char op_flag, address[ADDRESS_SIZE];
	unsigned long long int decimal_address, tag, set_index;
	while (fscanf(trace_inp, "%c %s\n", &op_flag, address) == 2)
	{
		decimal_address = strtoull(address, NULL, HEX); // converts from hex to dec
		tag = (decimal_address / BLOCK_SIZE);
		set_index = (decimal_address / BLOCK_SIZE) % numSets;

		bool isRead = (op_flag == 'R') ? true : false; // determine which operation to perform

		if (writeback_flag)
			handleWriteback(tag, blocks[set_index], isRead);
		else
			handleWritethru(tag, blocks[set_index], isRead);
		access_counter++;
	}

	// free memory for blocks
	for (int i = 0; i < numSets; i++)
		free(blocks[i]);
	free(blocks);

	// print the output
	print_output();

	return 0;
}