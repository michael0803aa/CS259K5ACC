#include <stdlib.h>
#include <stdio.h>
#include "k5acc.h"

int overlap(int qryS, int qryE, int refS, int refE){
    if((qryS <= refS && refS <= qryE) || (qryS <= refE && refE <= qryE)) return 1;
    else return 0;
}

int main(){
    srand(8888);
    
    // storage for references and queries
    int ref[REF_NUM*2] = {};
    int queries[QUERY_NUM*2] = {};
    int refResult[REF_NUM*QUERY_NUM];

    const int maxRefInrLen = 50;
    const int maxQryInrLen = 100;
    const int maxHop = 15;
    int refStart = 2;
    int queryStart = 5;
    // generate ref intervals
    for(int i = 0; i < REF_NUM*2; i += 2){
        ref[i] = refStart;
        ref[i+1] = refStart + (rand() % maxRefInrLen) + 1;
        refStart += rand() % maxHop + 1;
        printf("%d: (%d, %d)\n", i >>1, ref[i], ref[i+1]);
    }
    printf("======\n");
    // generate query intervals and the result
    int refResultIdx = 0;
    for(int i = 0; i < QUERY_NUM*2; i += 2){
        queries[i] = queryStart;
        queries[i+1] = queryStart + (rand() % maxQryInrLen) + 1;
        queryStart += rand() % maxHop + 1;
        printf("(%d, %d) ", queries[i], queries[i+1]);

        for(int j = 0; j < REF_NUM*2; j += 2){
            if(ref[j+1] < queries[i]) continue;
            if(ref[j] > queries[i+1]) break;
            refResult[refResultIdx++] = j >> 1;
            printf("%d, ", j >> 1);
        }
        refResult[refResultIdx++] = -1;      
        printf("\n");
    }
    refResult[refResultIdx++] = -2;

    // print refResult
    int p = 0;
    while(refResult[p] != -2)
        printf("%d, ", refResult[p++]);
    printf("-2\n");


    // create output buffer for accelerator
    int dram_out_buffer[REF_NUM*QUERY_NUM] = {};
    workload(queries, ref, dram_out_buffer, QUERY_NUM*2, REF_NUM*2);

    // print dram result
    p = 0;
    int diff = 0;
    while(p < REF_NUM*QUERY_NUM && dram_out_buffer[p] != -2){
        if(dram_out_buffer[p] != refResult[p]) diff++;
        printf("%d, ", dram_out_buffer[p++]);
    }
    printf("-2\n");
    printf("diff = %d\n", diff);
    
    return 0;
}