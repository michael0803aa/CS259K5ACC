#include "k5acc_m.h"
#include <stdio.h>

int getOverlapping(int local_queries[Q*2], int local_ref[R*2], int local_fifo[Q*OUT_BUFFER_WIDTH], int offset){
    //printf("offset = %d\n", offset);
    int idx = 0;
    int i = 0;
    for(i = 0; i < Q*2; i += 2){ // one query
        //#pragma HLS PIPELINE II=1
        //printf("query: (%d, %d): ", local_queries[i], local_queries[i+1]);
        int j = 0;
        for(j = 0; j < R*2; j += 2){    // one ref interval
            //#pragma HLS PIPELINE II=1
            //printf("(%d, %d) ? ", local_ref[j], local_ref[j+1]);
            if(!(local_queries[i+1] < local_ref[j] || local_ref[j+1] < local_queries[i])){
                local_fifo[idx++] = (j >> 1) + offset;
                //printf("%d, ", (j >> 1) + offset);
            }

        }
        //printf("-1\n");
        local_fifo[idx++] = -1;
    }
    return idx;
}

void workload(int* queries, int* refs, int** dram_out_buffer){
    // queries_len is a multiple of Q*2
    // ref_len is a multiple of M*R*2
    int out_buffer[OUT_BUFFER_HEIGHT][OUT_BUFFER_WIDTH];
    int idx_out_buffer[OUT_BUFFER_HEIGHT] = {};
    int idx_dram_out_buffer = 0;
    int local_ref[M][R*2];
    #pragma HLS ARRAY_PARTITION variable=local_ref complete dim=1


    int local_fifo[M][Q*OUT_BUFFER_WIDTH];
    int local_queries[M][Q*2];  // M copies of queries
    #pragma HLS ARRAY_PARTITION variable=local_fifo complete dim=1
    #pragma HLS ARRAY_PARTITION variable=local_queries complete dim=1

    
    int queries_len = QUERY_NUM * 2;
    int ref_len = REF_NUM * 2;

    #pragma HLS INTERFACE m_axi port=queries offset=slave depth=1000000
    #pragma HLS INTERFACE m_axi port=refs offset=slave depth=1000000
    #pragma HLS INTERFACE m_axi port=dram_out_buffer offset=slave depth=1000000000

//    #pragma HLS INTERFACE s_axilite port=queries_len bundle=control
//    #pragma HLS INTERFACE s_axilite port=ref_len bundle=control
    #pragma HLS INTERFACE s_axilite port=return bundle=control

    // initialize fifo
    int i = 0, j, rIter;
    for(i = 0; i < M; i++){ // Loop2
        #pragma HLS UNROLL
        for(j = 0; j < Q*OUT_BUFFER_WIDTH; j++){    // loop 2 ~ 33
            local_fifo[i][j] = -2;
        }
    }

    for(i = 0, rIter = 0; i < ref_len; i += REF_NUM/REF_SLICE_NUM*2, rIter++){
        
        printf("i = %d\n", i);
        idx_dram_out_buffer = 0;
        // load refs from dram to bram
        for(j = 0; j < M; j++){ // M process unit  Loop 34.1.1
            //#pragma HLS PIPELINE II=1
            // copy queries from dram to bram
            int k;
            int offset = i + j*R*2;
            for(k = 0; k < R*2; k += 2){    // for each process unit, load corresponding refs
                //#pragma HLS PIPELINE II=1 
                local_ref[j][k] = refs[offset + k];
                local_ref[j][k+1] = refs[offset + k + 1];
            }
        }

        // iterate through all the queries
        int qIter;
        for(j = 0, qIter = 0; j < queries_len; j += 2*Q){
            printf("query = %d\n", j);
            // copy queries from dram to bram
            int k = 0;
            for(k = 0; k < 2*Q; k += 2){
                //#pragma HLS PIPELINE II=1
                local_queries[0][k] = queries[i + k];
                local_queries[0][k+1] = queries[i + k + 1];
            }

            // duplicate the queries M-1 times
            for(k = 1; k < M; k++){
                int l;
                for(l = 0; l < 2*Q; l += 2){
                    //#pragma HLS PIPELINE II=1
                    local_queries[k][l] = local_queries[0][l];
                    local_queries[k][l+1] = local_queries[0][l+1];
                }
            }

            for(k = 0; k < M; k++){ // M process unit
                #pragma HLS UNROLL
                // process queries
                int offset = j + k*R*2;
                int len = getOverlapping(local_queries[k], local_ref[k], local_fifo[k], offset >> 1);
                local_fifo[k][len] = -2;
                
            }
            // flush FIFO to out_buffer
            for(k = 0; k < M; k++){ // M process unit, loop 34.1.2
                //#pragma HLS PIPELINE II=1
                int row = qIter*Q;
                //printf("flush when j = %d\n", j);
                int l;
                for(l = 0; l < Q*OUT_BUFFER_WIDTH; l++){
                    #pragma HLS PIPELINE II=2
                    int num = local_fifo[k][l];
                    local_fifo[k][l] = -2;
                    if(num == -1){
                        ++row;
                    }
                    else if(num != -2){
                   //     if(row >= OUT_BUFFER_HEIGHT)
                     //       printf("row = %d, idx = %d, num = %d\n", row, idx_out_buffer[row], num);
                        out_buffer[row][idx_out_buffer[row]++] = num;
                    }
                    
                }
                //printf("done\n");
            }
            // flush out_buffer to dram_buffer
            if (qIter == QUERY_BATCH-1){ 
                qIter = 0;
                int k = 0;
                for (k = 0; k < Q * QUERY_BATCH; k++){  // 34.3
                    //#pragma HLS PIPELINE II=1
                    int l = 0;
                    for (l = 0; l < OUT_BUFFER_WIDTH; l++){ // 34.3.1
                        #pragma HLS PIPELINE II=3
                        if (l < idx_out_buffer[k]){
                            dram_out_buffer[rIter][idx_dram_out_buffer++] = out_buffer[j][k];
                        }
                        else if (k == idx_out_buffer[j]){
                            dram_out_buffer[rIter][idx_dram_out_buffer++] = -1;
                        }
                    }
                    idx_out_buffer[k] = 0;
                }
            }
            else{
                qIter++;
            }
        }
        dram_out_buffer[rIter][idx_dram_out_buffer++] = -2;
    }
    //printf("done, idx = %d\n", idx_dram_out_buffer);
    //dram_out_buffer[idx_dram_out_buffer++] = -2;
}
