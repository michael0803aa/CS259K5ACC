#include <vector>
using namespace std;

vector<T> alternating(vector<T> seq, bool includeFirst){
	vector<int> inds;
	if(includeFirst)
		for(int i = 0; i < seq.size(); i++)
			inds.push_back(i);	// 0 to seq.size()-1 (inclusive)
	else
		for(int i = 1; i < seq.size()+1; i++)
			inds.push_back(i);	// 0 to seq.size() (inclusive)

	vector<int> result;
	for(int i = 0; i < seq.size(); i++)
		if(inds[i] % 2 == 0)	// take the elements with even indx
			result.push_back(seq[i]);
	return result;
}
