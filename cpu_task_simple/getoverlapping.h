#ifndef __GET_OVERLAPPING__
#define __GET_OVERLAPPING__
#include <vector>
using std::vector;

int overlaps(int* query, int* refContig);

int firstPotentiallyReaching(int pos, int reach*, int reach_len, int shift, int vs_size);

void getOverlapping(int* query, int* vs, int* reach, int vs_size, int reach_len, int shift, vector<int>& res);


#endif
