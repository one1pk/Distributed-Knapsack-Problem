#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include "core/utils.h"
#include "core/problemInput.h"
#include "core/get_time.h"
using namespace std;

/*
  Prints the indexes and values of items included in the dynamic table
 */
static void display_items(int n,vector<vector<int>> &dynamic_tbl ,vector <int> &s, vector <int> &v, int S)
{
    int result = dynamic_tbl[0][S];
    int j = S;
    string indexes, values;

    for(int i=0; i<n && result>0; i++) {
       // if result from dynamic_tbl[i+1][w] -> item is not included 
       // or from v[i+1]+K[i+1][w-s[i-1]] -> if this then this item is included 
       if(result == dynamic_tbl[i+1][j]){
           continue;
       }
       else {
           indexes +=  to_string(i+1) + ", ";
           values += to_string(v[i+1]) + ", ";
           result-= v[i];
           j-=s[i];
       }
    }
    cout << "Item Indexes: \n" << indexes << "\nValues: \n" << values;
}

/*
 * Solution is based on Dynamic Programming Paradigm
 */
static int knapsack_serial(int n, vector<int> &s, vector<int> &v, int S) {
    timer time;
    double time_taken = 0.0;
    // matrix of maximum values obtained after all intermediate combinations of items
    vector<vector<int>> dynamic_tbl(n+1, vector<int>(S+1));
    // dynamic_tbl[i][j] is the maximum value that can be obtained by using a subset of the items (i...n−1) (last n−i items) which weighs at most j pounds
    
    time.start();
    // begin from base case i==n, when bag is full
    for(int i = n; i >= 0; i--) {
        for(int j  = 0;j <= S; j++) {
            if(i==n) {
                // no items to add when bag is full
                dynamic_tbl[i][j] = 0; // initial condition
            }
            else {
                vector<int> choices(2);

                // 1st choice: Don't add item i to knapsack 
                choices[0] = dynamic_tbl[i + 1][j]; // reuse optimal solution of i+1
                if(j >= s[i]) {
                    // 2nd choice: Add item i to knapsack
                    choices[1] = dynamic_tbl[i+1][j-s[i]]+v[i]; // add value of item i to optimal solution of i+1 that weighs at most j-s[i] 
                }
                // choose maximum profit out of both choices
                dynamic_tbl[i][j] = max(choices[0], choices[1]); 
            }
        }
    }

    time_taken = time.stop();
    cout<<"Time taken (in seconds): " << time_taken << std::setprecision(5) << endl;
    
    display_items(n, dynamic_tbl, s, v, S);

    // max value returned for the case where bag is empty (i=0) and max weight is the capacity of the bag (j=S)
    int result = dynamic_tbl[0][S]; 
    return dynamic_tbl[0][S];
}

int main(int argc, char **argv) {
    
    cxxopts::Options options(
      "0-1 Knapsack Serial solver",
      "Maximize profit from given items' weights and values with bounded capacity");
    options.add_options(
        "",
        {
            {"capacity", "Maximum allowed capacity",
            cxxopts::value<uint>()->default_value(DEFAULT_CAPACITY)}
            
        }
    );
    auto cl_options = options.parse(argc, argv);
    uint capacity = cl_options["capacity"].as<uint>(); 

    ProblemInput problemInstance; 
    
    printf("Starting knapsack solving...\n"); 
    cout << "Number of items : "<< problemInstance.ProblemInput_GetNumItems() <<"\n";
    cout << "Capacity : "<< capacity <<"\n";
    
    int max_val = knapsack_serial(  problemInstance.ProblemInput_GetNumItems(),
                                    problemInstance.weights, 
                                    problemInstance.values, 
                                    problemInstance.ProblemInput_SetCapacity(capacity));

    cout << "\nMaximum value: " << max_val << endl;
    
    return 0;
}
