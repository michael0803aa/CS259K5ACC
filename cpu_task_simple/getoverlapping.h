#ifndef __GET_OVERLAPPING__
#define __GET_OVERLAPPING__
#include "initbram.h"
// input query is an interger array with length 3
// 0: contig id, 1: start point, 2: end point


int overlaps(int query[3], int curContig[2]);

int firstPotentiallyReaching(int position, int reach[REACH_LENGTH], int reachLength, int shift, int vs_size);

int getOverlapping(int query[3], int* vs, int* reach, int vs_size, int reachLength, int shift, int* result_buffer);

int getResult(int query[3], int** arrayAddrs, int* out_buffer)

int* writeBack(int* bramBuffer, int* dramBuffer, int* dram_buffer_start);

#endif
