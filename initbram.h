#ifndef __INIT_BRAM__
#define __INIT_BRAM

#define BRAM_BUFFER_SIZE 5000
#define DRAM_BUFFER_SIZE 1000000

void init_workload(int* reach_arr, int* reach_length_arr, int* shift_arr, int* vs_size_arr, int reach_arr_len, int reach_length_arr_len, int shift_arr_len, int vs_size_arr_len);

#endif
