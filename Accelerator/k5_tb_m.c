#include <stdlib.h>
#include <stdio.h>
#include "k5acc_m.h"

int overlap(int qryS, int qryE, int refS, int refE){
    if((qryS <= refS && refS <= qryE) || (qryS <= refE && refE <= qryE)) return 1;
    else return 0;
}

int main(){
    srand(1728);
    
    // storage for references and queries
    int ref[REF_NUM*2] = {};
    int queries[QUERY_NUM*2] = {};
    //int* queries = (int*)malloc(sizeof(int)*QUERY_NUM*2);
    // int refResult[REF_NUM*QUERY_NUM];
    int* refResult = (int*)malloc(sizeof(int)*REF_NUM*QUERY_NUM);

    const int maxRefInrLen = 50;
    const int maxQryInrLen = 100;
    const int maxHop = 15;
    const int maxQryStart = 100000;
    int refStart = 2;
    int queryStart = 5;
    // generate ref intervals
    for(int k = 0; k < REF_NUM*2; k += 2){
        ref[k] = refStart;
        ref[k+1] = refStart + (rand() % maxRefInrLen) + 1;
        refStart += rand() % maxHop + 1;
        //printf("%d: (%d, %d)\n", k >>1, ref[k], ref[k+1]);
    }
    //printf("======\n");
    // generate query intervals and the result
    int refResultIdx = 0;
    for(int i = 0; i < QUERY_NUM*2; i += 2){
        // queries[i] = queryStart;
        // queries[i+1] = queryStart + (rand() % maxQryInrLen) + 1;
        // queryStart += rand() % maxHop + 1;
        queries[i] = rand() % maxQryStart + 1;
        queries[i+1] = queries[i] + (rand() % maxQryInrLen) + 1;
        //printf("(%d, %d) ", queries[i], queries[i+1]);

        for(int j = 0; j < REF_NUM*2; j += 2){
            if(ref[j+1] < queries[i]) continue;
            if(ref[j] > queries[i+1]) break;
            refResult[refResultIdx++] = j >> 1;
            //printf("%d, ", j >> 1);
        }
        refResult[refResultIdx++] = -1;      
       // printf("\n");
    }
    refResult[refResultIdx++] = -2;

    // print refResult
    int p = 0;
    //while(refResult[p] != -2)
        //printf("%d, ", refResult[p++]);
    //printf("-2\n");


    // create output buffer for accelerator
    //int dram_out_buffer[OUT_BUFFER_WIDTH*QUERY_NUM] = {};
    //printf("size = %d\n", OUT_BUFFER_WIDTH*QUERY_NUM);
    int* dram_out_buffer = (int*)malloc(sizeof(int) * OUT_BUFFER_WIDTH*QUERY_NUM);
    workload(queries, ref, dram_out_buffer);

    // print dram result
    p = 0;
    int diff = 0;
    while(p < REF_NUM*QUERY_NUM && dram_out_buffer[p] != -2){
        if(dram_out_buffer[p] != refResult[p]) diff++;
        p++;
        //printf("%d, ", dram_out_buffer[p++]);
    }
    //printf("-2\n");
    printf("diff = %d\n", diff);
    
    return diff;
}
