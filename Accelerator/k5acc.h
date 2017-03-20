#ifndef __K5_ACC_H__
#define __K5_ACC_H__


#define R 32      // 128
#define Q 16       // 64
#define M 32         // 8
#define QUERY_NUM Q*10240
#define REF_NUM R*M*5
#define QUERY_BATCH 2
#define OUT_BUFFER_WIDTH 112
#define OUT_BUFFER_HEIGHT Q*QUERY_BATCH

void workload(int* queries, int* refs, int* dram_out_buffer, int queries_len, int ref_len);
int getOverlapping(int local_queries[Q*2], int local_ref[R*2], int local_fifo[Q*OUT_BUFFER_WIDTH], int offset);

#endif