#include <vector>
using std::vector;
// input query is an interger array with length 3
// 0: contig id, 1: start point, 2: end point


int overlaps(int* query, int* refContig){
    if(query[0] <= refContig[1] && refContig[1] <= query[1]
        || query[0] <= refContig[0] && refContig[0] <= query[1]) return 1;
    return 0;
}


int firstPotentiallyReaching(int pos, int reach*, int reach_len, int shift, int vs_size) {
    // binary search
    int lo = 0, hi = reach_len;
    while(lo < hi){
        int mid = lo + (hi-lo)/2;
        if(reach[mid] >= pos) hi = mid;
        else lo = mid + 1;
    }
    if(lo != reach_len) return lo << shift;
    return vs_size - 1;
}

void getOverlapping(int* query, int* vs, int* reach, int vs_size, int reach_len, int shift, vector<int>& res){
    // use index to skip early non-overlapping entries.
    int idx = firstPotentiallyReaching(query[0], reach, reachLength, shift, vs_size);

    for (int* p_interval = vs+idx*2; idx < vs_size; idx++, p_interval += 2) {
        // they are sorted by start location, so if this one starts too late
        // then all of the others will, too.
        // p_interval is the pointer to the reference interval we are checking
        if (p_interval[0] > query[1]) break;
        if (overlaps(query, p_interval)) res.push_back(idx);
    }
}
