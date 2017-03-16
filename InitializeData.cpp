#include <stdio.h>
#include <stdlib.h>
#include "Task.h"
#include "initbram.h"
using namespace blaze;

class InitializeData : public Task{
public:
    // extends the base class constructor
    // to indicate how many input blocks
    // are required
    // the term "block" is decribed on p.24 of blaze slides
    // I "assume" the block number is the number of arrays
    // being passed in
    // Since we have 5 input arrays, blocks = 5
    FindOverlappingTest() : Task(5) {;}

    // This task loads all the required data to FPGA
    virtual void compute(){
        // array length of the 5 input arrays
        int start_end_arr_len = getInputLength(0);
        int reach_arr_len = getInputLength(1);
        int reach_length_arr_len = getInputLength(2);
        int shift_arr_len = getInputLength(3);
        int vs_size_arr_len = getInputLength(4);

        // get the pointer of each input arrays
        int* start_end_arr = (int*)getInput(0);
        int* reach_arr = (int*)getInput(1);
        int* reach_length_arr = (int*)getInput(2);
        int* shift_arr = (int*)getInput(3);
        int* vs_size_arr = (int*)getInput(4);

        // put everything to BRAM
        init_workload(reach_arr, reach_length_arr,
                      shift_arr, vs_size_arr,
                      reach_arr_len, reach_length_arr_len,
                      shift_arr_len, vs_size_arr_len);

        // Not sure how to deal with start_end_arr (this should be in DRAM)
        // I think it's in the DRAM now, so just leave it untouched?
        // But how the following task, FindOverlapping, accesses it?
        // Or, don't bring this array in for initialization,
        // instead, attach the entire array for all the following queries?


    }
}
