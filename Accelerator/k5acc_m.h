#ifndef __K5_ACC_H__
#define __K5_ACC_H__


#define R 256      // 128
#define Q 128       // 64
#define M 4         // 8
#define QUERY_NUM Q*256
#define REF_NUM R*M*128
#define QUERY_BATCH 16
#define OUT_BUFFER_WIDTH 112
#define OUT_BUFFER_HEIGHT Q*QUERY_BATCH

void workload(int* queries, int* refs, int* dram_out_buffer);
int getOverlapping(int local_queries[Q*2], int local_ref[R*2], int local_fifo[Q*OUT_BUFFER_WIDTH], int offset);

#endif
