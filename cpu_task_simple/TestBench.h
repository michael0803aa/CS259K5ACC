class TestBench{
private:
    void initVs_size();
    void initStart_end();
public:
    TestBench(int numContig, int seed = 1792);
    static const int MAX_REF_PER_CONTIG = 20;
    static const int MAX_HOP_INTRA_CONTIG = 10;
    static const int HOP_INTER_CONTIG = 2000;
    static const int MAX_ITRVL_LEN = 40;
    int numContig;
    int start_end_arr_len;
    int* vs_size_arr;
    int* shift_arr;
    int* reach_length_arr;
    int* reach_arr;
    int* start_end_arr;

};
