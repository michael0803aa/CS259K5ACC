#include <stdio.h>
#include <stdlib.h>
#include <string>
#define LOG_HEADER "ArrayTest"
#include <glog/logging.h>
#include "Task.h"
#include "initbram.h"
#include "getoverlapping.h"
#include <vector>
using namespace blaze;

class FindOverlappingTask : public Task{
public:
    // extends the base class constructor
    // to indicate how many input blocks
    // are required
    // the term "block" is decribed on p.24 of blaze slides
    // I "assume" the block number is the number of arrays
    // being passed in
    // Since we have 6 input array, blocks = 6
    FindOverlappingTest() : Task(6) {;}

    // This task loads all the required data to C
    virtual void compute(){
        // length of query array, should be 3 * nQueris
        int query_data_len = getInputLength(0);
        int query_num = getInputNumItems(0);    // numbers of queris
        LOG(INFO) << "====== Start Running cpu_task ======\n";
        LOG(INFO) << "query_data_len = " << query_data_len << "\n";
        LOG(INFO) << "query_num = " << query_num << "\n";
        // query_num == query_data_len / 3

        // get the pointer of each input arrays
        int* queris = (int*)getInput(0);
        
        // array length of the 5 other input arrays
        int start_end_arr_len = getInputLength(1);
        int reach_arr_len = getInputLength(2);
        int reach_length_arr_len = getInputLength(3);
        int shift_arr_len = getInputLength(4);
        int vs_size_arr_len = getInputLength(5);
        LOG(INFO) << "start_end_arr_len = " << start_end_arr_len << "\n";
        LOG(INFO) << "reach_arr_len = " << reach_arr_len << "\n";
        LOG(INFO) << "reach_len_arr_len = " << reach_len_arr_len << "\n";
        LOG(INFO) << "shift_arr_len = " << shift_arr_len << "\n";
        LOG(INFO) << "vs_size_arr_len = " << vs_size_arr_len << "\n";

        // get the pointer of each input arrays
        int* start_end_arr = (int*)getInput(1);
        int* reach_arr = (int*)getInput(2);
        int* reach_length_arr = (int*)getInput(3);
        int* shift_arr = (int*)getInput(4);
        int* vs_size_arr = (int*)getInput(5);
        // the last 3 length should equal the total number of contigs
        
        // put everything to BRAM and get their address on BRAM
		// addrs[0] is reach_arr, addrs[1] is reach_length_arr,
		// addrs[2] is shift_arr, addrs[3] is vs_size_arr
        int** addrs = (int**)malloc(5*sizeof(int*));
        init_workload(reach_arr, reach_length_arr,
                      shift_arr, vs_size_arr,
                      reach_arr_len, reach_length_arr_len,
                      shift_arr_len, vs_size_arr_len,
                      addrs);
        addrs[4] = start_end_arr;

        // a temparary buffer to hold all the output
        // when all the output is ready, compute the total size, put it back using getoutput
        int* dram_buffer = (int*)malloc(sizeof(int)*DRAM_BUFFER_SIZW);
        int dram_buffer_start = 0;
        std::vector<int*> bufferList;
        bufferList.push_back(drambuffer); 
        
        int buf_sel = 0, buf_start = 0; 
        int* bram_buffer_0 = addrs[5], bram_buffer_1 = addr[6];
        int total_output_len = 0;
        for(int i = 0; i < query_num; i++){
            int result_len = getResult(queries+i*3, addrs, output_buffer);
            total_output_len += result_len;
            if(buf_start + result_len >= BRAM_BIFFER_SIZE){
                // a bram-buffer-full is detected, write back
                if(buf_sel == 0) dram_buffer = writeBack(bram_buffer_0, dram_buffer, &dram_buffer_start);
                else dram_buffer = writeBack(bram_buffer_1, dram_buffer, &dram_buffer_start);
                // if dram buffer is full, writeBack function will create one more and return the addr
                if(dram_buffer != bufferList[bufferList.size()-1]) bufferList.push_back(dram_buffer);
                buf_sel = buf_sel ^ 1;
                buf_start = buf_start + result_len - BRAM_BUFFER_SIZE;
            }
        }
		// loop through each query and update the pointer of output buffer
		// not sure how to deal with almost full output buffer
		// int result_len = getResult(query, addrs, out_buffer);
		// result_num is number of intervals
        
        // write all the data in the dram buffers to the data exchangeing area
        int* out_to_scala = (int*)getOutput(0, total_output_len, 1, sizeof(int));
        int p = 0;
        for(int i = 0; i < bufferList.size()-1; i++){
            for(int j = 0; j < BRAM_BUFFER_SIZE; j++)
                out_to_scala[p++] = bufferList[i][j];      
        }
        int* last_buffer = bufferList[bufferList.size()-1];
        for(int i = 0; i < dram_buffer_start; i++)
            out_to_scala[p++] = last_buffer[i];
    }
}
