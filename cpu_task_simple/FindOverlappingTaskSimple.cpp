#include <stdio.h>
#include <stdlib.h>
#include <string>
#define LOG_HEADER "FindOverlappingSimple"
#include <glog/logging.h>
#include "Task.h"
#include "getoverlapping.h"
#include <vector>
using namespace blaze;
using std::vector;

class FindOverlappingTaskSimple : public Task{
public:
    // extends the base class constructor
    // to indicate how many input blocks
    // are required
    // the term "block" is decribed on p.24 of blaze slides
    // I "assume" the block number is the number of arrays
    // being passed in
    // Since we have 6 input array, blocks = 6
    FindOverlappingTestSimple() : Task(6) {;}

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
        // the last 3 length should equal the total number of contigs (25)

        // start processing queries
        vector<int> result;
        int query_id = 0;
        for(int* p_query = queries; i < queries+query_data_len; p_query += 3){
            result.push_back(query_id++);
            int contig_id = p_query[0];
            // get the reference intervals for that contig
            int* contig_interval_start = start_end_arr;
            int reach_arr_start = reach_arr;
            for(int i = 0; i < contig_id; i++){
                contig_interval_start += vs_size_arr[i]*2;
                reach_arr_start += reach_len[i];
            }
            getOverlapping(p_query+1, contig_interval_start, reach_arr_start,
                vs_size_arr[contig_id], reach_length_arr[contig_id],
                shift_arr[contig_id], result);
            result.push_back(-1);
        }
        // collect result to the data transfer reigion (by getOutput)
        int output_size = result.size();
        int* output_buffer = (int*)getOutput(0, output_size, 1, sizeof(int));
        for(int i = 0; i < output_size; i++)
            output_buffer[i] = result[i];
    }
}
