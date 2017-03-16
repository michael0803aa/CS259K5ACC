#include <stdlib.h>
#include "initbram.h"

void init_workload(int* reach_arr, int* reach_length_arr, int* shift_arr, int* vs_size_arr, int reach_arr_len, int reach_length_arr_len, int shift_arr_len, int vs_size_arr_len, int** arr_ptrs){
    // declare arrays for BRAM
    // dynamic size array, not sure if HLS would support this
    // hopefully, this would transfer data to BRAM
    int* local_reach_arr = (int*)malloc(reach_arr_len * sizeof(int));
    int* local_reach_length_arr = (int*)malloc(reach_length_arr_len * sizeof(int));
    int* local_shift_arr = (int*)malloc(shift_arr_len * sizeof(int));
    int* local_vs_size_arr = (int*)malloc(vs_size_arr_len * sizeof(int));
    int* local_out_buffer_1[BRAM_BUFFER_SIZE];
    int* local_out_buffer_2[BRAM_BUFFER_SIZE];

    // move data to BRAM
    int i;
    for(i = 0; i < reach_arr_len; i++)
        local_reach_arr[i] = reach_arr[i];
    for(i = 0; i < reach_length_arr_len; i++)
        local_reach_length_arr = reach_length_arr[i];
    for(i = 0; i < shift_arr_len; i++)
        local_shift_arr[i] = shift_arr[i];
    for(i = 0; i < vs_size_arr_len; i++)
        local_vs_size_arr[i] = vs_size_arr[i];

    arr_ptrs[0] = local_reach_arr;
    arr_ptrs[1] = local_reach_length_arr;
    arr_ptrs[2] = local_shift_arr;
    arr_ptrs[3] = local_vs_size_arr;
    arr_ptrs[5] = local_out_buffer_1;
    arr_ptrs[6] = local_out_buffer_2;
    // done
}
