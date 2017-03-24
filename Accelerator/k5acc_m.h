#ifndef __K5_ACC_M_H__
#define __K5_ACC_M_H__


#define R 1024*16     // 128
#define Q 128       // 64
#define M 32         // 8
#define QUERY_NUM (Q/128)*4096*32*12   // 12M
#define REF_NUM (R*M/1024/16/32)*1024*32*4*8 // 8M
#define QUERY_BATCH 1
#define OUT_BUFFER_WIDTH 112
#define OUT_BUFFER_HEIGHT Q*QUERY_BATCH
/*
#define R 16     // 128
#define Q 8       // 64
#define M 2         // 8
#define QUERY_NUM Q*4   // 12M
#define REF_NUM R*M*6 // 8M
#define QUERY_BATCH 1
#define OUT_BUFFER_WIDTH 112
#define OUT_BUFFER_HEIGHT Q*QUERY_BATCH
*/
void workload(int* queries, int* refs, int* dram_out_buffer);
int getOverlapping(int local_queries[Q*2], int local_ref[R*2], int local_fifo[Q*OUT_BUFFER_WIDTH], int offset);

#endif
