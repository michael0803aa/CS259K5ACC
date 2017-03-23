#ifndef __K5_ACC_M_H__
#define __K5_ACC_M_H__


#define R 1024*16     // 128
#define Q 32       // 64
#define M 16         // 8
#define QUERY_NUM Q*64*64*12   // 12M
#define REF_NUM R*M*32/16*2 // 8M
#define QUERY_BATCH 1
#define OUT_BUFFER_WIDTH 112
#define OUT_BUFFER_HEIGHT Q*QUERY_BATCH

void workload(int* queries, int* refs, int* dram_out_buffer);
int getOverlapping(int local_queries[Q*2], int local_ref[R*2], int local_fifo[Q*OUT_BUFFER_WIDTH], int offset);

#endif
