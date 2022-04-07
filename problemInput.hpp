#ifndef _PROBLEM_DESCR
#define _PROBLEM_DESCR
#include <vector>
#include <iostream>
using namespace std;
class ProblemInput{
public:
    int num_items,capacity;
    vector<int> values;
    vector<int> weights;
    ProblemInput(const string& filename);
    ~ProblemInput();
};
#endif