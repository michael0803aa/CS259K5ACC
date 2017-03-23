#include <ctime>
#include <vector>
#include <cstdlib>
#include "TestBench.h"
using namespace std;

TestBench::TestBench(int numContig, int seed){
    srand(seed); // initialize seed
    this->numContig = numContig;
    initVs_size(); // initialize vs_size_arr
    initStart_end(); // initialize start_end_arr
    
    
}

void TestBench::initVs_size(){
    vs_size_arr = new int[numContig];
    start_end_arr_len = 0;
    for(int i = 0; i < numContig; i++){
        vs_size_arr[i] = (rand() % MAX_REF_PER_CONTIG + 1) * 2;
        start_end_arr_len += vs_size_arr[i];
    }
}

void TestBench::initStart_end(){
    // initialize start_end_arr
    start_end_arr = new int[start_end_arr_len];
    int itrvlStart = 4;
    int contigStartIdx = 0;
    for(int i = 0; i < numContig; i++){
        int vs_size = vs_size_arr[i];
        for(int j = contigStartIdx; j < vs_size; j += 2){
            start_end_arr[j] = itrvlStart;
            start_end_arr[j+1] = itrvlStart + (rand() % MAX_ITRVL_LEN + 1);
            itrvlStart += rand() % MAX_HOP_INTRA_CONTIG + 1;
        }
        itrvlStart += HOP_INTER_CONTIG;
        contigStartIdx += vs_size;
    }
}