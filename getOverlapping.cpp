#include <stdlib.h>
#include "initbram.h"
// input query is an interger array with length 3
// 0: contig id, 1: start point, 2: end point


int overlaps(int query[3], int curContig[2]){
    if(query[1] <= curContig[1] && curContig[1] <= query[2])
        return 1;
    else
        return 0;
}


int firstPotentiallyReaching(int position, int reach[REACH_LENGTH], int reachLength, int shift, int vs_size) {
    int i = 0;
    for (i = 0; i < reachLength; i++) {
        if (reach[i] >= position) {
            return i<<shift;
        }
    }
    // if no one reaches to the given function
    return (vs_size-1);
}

int getOverlapping(int query[3], int* vs, int* reach, int vs_size, int reachLength, int shift, int buf_sel, int buf_start, int out_buffer_0[BRAM_BUFFER_SIZE], int out_buffer_1[BRAM_BUFFER_SIZE]){
    // use index to skip early non-overlapping entries.
    int result_len = 0;
    int* active_buffer = buf_sel == 0 ? out_buffer_0 : out_buffer_1;
    active_buffer[buf_start++] = -1;
    if(buf_start == BRAM_BUFFER_SIZE){
        active_buffer = buf_sel == 0 ? out_buffer_1 : out_buffer_0;
        buf_start = 0;
    }
    result_len++;
    
    int idx = firstPotentiallyReaching(query[1], reach, reachLength, shift, vs_size);
    if (idx < 0) {
        idx = 0;
    }

    // number of overlapping intervals
    int result_idx = 0;

    for (; idx < vs_size; idx++) {
        //struct interval v = vs[idx];
        int v[2];
        v[0] = vs[2*idx];
        v[1] = vs[2*idx+1];
        // they are sorted by start location, so if this one starts too late
        // then all of the others will, too.
        if (v[0] > query[2]) {
            break;
        }
        if (overlaps(query, v)) {
            active_buffer[buf_start++] = v[0];
            if(buf_start == BRAM_BUFFER_SIZE){
                active_buffer = buf_sel == 0 ? out_buffer_1 : out_buffer_0;
                buf_start = 0;
            }      
            if(buf_start == BRAM_BUFFER_SIZE){
                active_buffer = buf_sel == 0 ? out_buffer_1 : out_buffer_0;
                buf_start = 0;
            }
            result_len += 2;      
        }
    }
    // we now know how many values in result buffer to flush
    return result_len;
}

int getResult(int query[3], int** arrayAddrs, int* dram_buffer, int buf_sel, int buf_start){
	int vs_size = arrayAddrs[3][query[0]];
	int reachLength = arrayAddrs[1][query[0]];
	int shift = arrayAddrs[2][query[0]];

	int vs_start_offset = 0, reach_start_offset = 0;
	int i;
	for(i = 0; i < query[0]; i++){
		vs_start_offset += arrayAddrs[3][i];
		reach_start_offset += arrayAddrs[1][i];
	}

    int* out_buffer_0 = arrayAddrs[5], out_buffer_1 = arrayAddrs[6];
	int* vs = arrayAddrs[4] + vs_start_offset;	// base addr + shift
	int* reach = arrayAddrs[0] + reach_start_offset;	// base addr + shift
    int result_len = getOverlapping(query, vs, vs_size, reach, reachLength, shift, buf_sel, buf_start, out_buffer_0, out_buffer_1); 
    return result_len;
}

int* writeBack(int* bramBuffer, int* dramBuffer, int* dram_buffer_start){
    int start = *dram_buffer_start;
    for(int i = 0; i < BRAM_BUFFER_SIZE; i++){
        dramBuffer[start++] = bramBuffer[i];
        if(start == DRAM_BUFFER_SIZE){
            dramBuffer = (int*)malloc(sizeof(int)*DRAM_BUFFER_SIZE);
            start = 0;
        }       
    }
    return dramBuffer;
}
