#ifndef _PROBLEM_DESCR_
#define _PROBLEM_DESCR_
#include <vector>
#include <iostream>
#include <fstream>
#define INPUT_FILE_PATH "input_files/input.in"
#define MAX_FILE_LENGTH 1024
using namespace std;
class ProblemInput{
private: 
    int num_items, capacity;
public:
    vector<int> values;
    vector<int> weights;
    ProblemInput(void)
    {
        char path[MAX_FILE_LENGTH];
        int index_position = 0;
        snprintf(path, MAX_FILE_LENGTH, INPUT_FILE_PATH);
        ifstream infile;
        if (infile.fail()) { 
            cout<<"ERROR OPENING INPUT FILE, terminating program ... "<<endl;
            exit(1); 
        }
       
        infile.open(path);
        /*Read the number of items*/
        infile >> num_items;
        values.assign(num_items, 0);
        weights.assign(num_items, 0);
        while (!infile.eof()) {
            infile >> values[index_position];
            infile >> weights[index_position];
            index_position++;
        }
        infile.close();
    }

    int ProblemInput_SetCapacity(uint capacity)
    {
        this->capacity = capacity;
        return this->capacity;
    }
    int ProblemInput_GetNumItems(void)
    {
        return this->num_items;
    }

};
#endif