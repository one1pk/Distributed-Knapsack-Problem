#include "problemDescription.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>

using namespace std;
ProblemInput::ProblemInput(const string& filename) {
    
    
    ifstream input_file(filename);
   
    string line;
    getline(input_file, line);
    istringstream line_stream(line);
    weights.emplace_back(istream_iterator<int>(line_stream), istream_iterator<int>());

    getline(input_file, line);
    istringstream line_stream(line);
    values.emplace_back(istream_iterator<int>(line_stream), istream_iterator<int>());
    
    getline(input_file, line);
    capacity = stoi(line);

    num_items = weights.size();
}


ProblemInput::~ProblemInput() {
    weights.~vector();
    values.~vector();
}