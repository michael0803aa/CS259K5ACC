#define R 128
#define Q 64
#define M 8
#define QUERY_BATCH 10
#define OUT_BUFFER_WIDTH 112
#define OUT_BUFFER_HEIGHT Q*QUERY_BATCH


int getOverlapping(int local_queries[Q*2], int local_ref[R*2], int local_fifo[Q*OUT_BUFFER_WIDTH], int offset){
    int idx = 0;
    for(int i = 0; i < Q*2; i += 2){ // one query
        for(int j = 0; j < R*2; j += 2){    // one ref interval
            if(local_queries[i] <= local_ref[j] && local_ref[j] <= local_queries[i+1]
                || local_queries[i] <= local_ref[j+1] && local_ref[j+1] <= local_queries[i+1])
                local_fifo[idx++] = (j >> 1) + offset;
        }
        local_fifo[idx++] = -1;
    }
    return idx;
}

void wordload(int* queries, int* refs, int* dram_out_buffer, int queries_len, int ref_len){
    // queries_len is a multiple of Q*2
    // ref_len is a multiple of M*R*2
    int out_buffer[OUT_BUFFER_WIDTH*OUT_BUFFER_HEIGHT];
    int idx_out_buffer[OUT_BUFFER_HEIGHT] = {};
    int idx_dram_out_buffer = 0;
    int local_ref[M][R*2];
    int local_fifo[M][Q*OUT_BUFFER_WIDTH];
    int local_queries[Q*2];

    for(int i = 0, iter = 0; i < queries_len; i += 2*Q){
        // copy queries from dram to bram
        for(int j = 0; j < 2*Q; j += 2)
            local_queries[j] = queries[i + j];
            local_queries[j+1] = queries[i + j + 1];
        }
        // copy references from dram to bram
        for(int j = 0; j < ref_len; j += 2*M){  // iter through all the refs
            // loop unroll here
            for(int k = 0; k < M; k++){ // M process unit
                int offset = j + k*R*2;
                for(int l = 0; l < R*2; l += 2){    // for each process unit, load corresponding refs
                    local_ref[k][l] = refs[offset + l];
                    local_ref[k][l+1] = refs[offset + l + 1];
                }
                // process queries
                int len = getOverlapping(local_queries, local_ref[k], local_fifo[k], offset >> 1);
                local_fifo[len] = -2;
                // flush FIFO to out_buffer
                int row = iter*Q;
                for(int l = 0; l < Q*OUT_BUFFER_WIDTH; l++){
                    if(local_fifo[l] == -1){
                        ++row;
                    }
                    else if(local_fifo[l] != -2){
                        out_buffer[row][idx_out_buffer[row]++] = local_fifo[l];
                    }
                    
                }
            }    
        }
        // if iter === QUERY_BATCH, write back to dram, reset iter
        // otherwise iter++
        if (iter == QUERY_BATCH){
            iter = 0;
            for (int j = 0; j < Q * QUERY_BATCH; j++){
                for (int k = 0; k < OUT_BUFFER_WIDTH; k++){
                    if (k < idx_out_buffer[j]){
                        dram_out_buffer[idx_dram_out_buffer++] = out_buffer[j][k];
                    }
                    else if (k == idx_out_buffer[j]){
                        dram_out_buffer[idx_dram_out_buffer++] = -1;
                    }
                }
            }
        }
        else{
            iter++;
        }
    }
}
