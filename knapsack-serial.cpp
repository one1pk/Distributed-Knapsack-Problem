#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#define DEFAULT_N          4
#define DEFAULT_MAX_WEIGHT 60
using namespace std;

/*
  Prints the indexes of items included in the dynamic table
 */
void print_items(int n,vector<vector<int>> &dp ,vector <int> &s, vector <int> &v, int S)
{
    int result = dp[0][S];
    
    cout<<"Items included (by index): "<<endl;
    for(int i=0; i<n && result>0; i++)
    {
       // if result from dp[i+1][w] -> item is not included 
       // or from v[i+1]+K[i+1][w-s[i-1]] -> if this then this item is included 
       if(result == dp[i+1][S]){
           continue;
       }
       else {
           cout<<i+1<<" ";
           result-= v[i];
           S-=s[i];
       }
    }
    cout<<endl;
}

/*
 * Solution is based on Dynamic Programming Paradigm
 */
int knapsack_serial(int n, vector<int> &s, vector<int> &v, int S) {
    // matrix of maximum values obtained after all intermediate combinations of items
    vector<vector<int>> dp(n+1, vector<int>(S+1)); 
    // dp[i][j] is the maximum value that can be obtained by using a subset of the items (i...n−1) (last n−i items) which weighs at most j pounds
    
    // top-down approach
    for(int i = n; i >= 0; i--) {
        for(int j = 0; j <= S; j++) {
            if(i==n) {
                // no items to add when bag is full
                dp[i][j] = 0; // initial condition
            }
            else {
                int* choices = new int[2]; 
                // 1st choice: Don't add item i to knapsack 
                choices[0] = dp[i + 1][j]; // resuse optimal solution of i+1
                if(j >= s[i]) {
                    // 2nd choice: Add item i to knapsack
                    choices[1] = dp[i+1][j-s[i]]+v[i]; // add value of item i to optimal solution of i+1 that weighs at most j-s[i] 
                }
                // assuming no negative values
                dp[i][j] = max(choices[0], choices[1]); 
                delete[] choices;
            }
        }
    }
    print_items(n,dp,s,v,S);

    int result = dp[0][S]; 
    dp.~vector();   
    return result;
}

int main(int argc, char **argv) {
    if(argc!=2){
        cout << "Please enter one input file!" << "\n";
        return 0;
    }

    // cxxopts::Options options(
    //   "page_rank_pull",
    //   "Calculate page_rank using serial and parallel execution");
    // options.add_options(
    //   "",
    //   {
    //       {"nThreads", "Number of Threads",
    //        cxxopts::value<uint>()->default_value(DEFAULT_NUMBER_OF_THREADS)},
    //       {"nIterations", "Maximum number of iterations",
    //        cxxopts::value<uint>()->default_value(DEFAULT_MAX_ITER)},
    //       {"inputFile", "Input of values and weights",
    //        cxxopts::value<std::string>()->default_value(
    //            "/inputs/input.in")},
    //   }
    // );

    // auto cl_options = options.parse(argc, argv);
    // uint n_threads = cl_options["nThreads"].as<uint>();
    // uint max_iterations = cl_options["nIterations"].as<uint>();
    // std::string input_file_path = cl_options["inputFile"].as<string>();

    vector<int> weights, values;
    int num_items, capacity;
    capacity = DEFAULT_MAX_WEIGHT;
    
    

    printf("Starting knapsack solving...\n"); 

    int max_val = knapsack_serial(num_items, weights, values, capacity);

    cout << "Maximum value: " << max_val << endl;

    weights.~vector();
    values.~vector();

    return 0;
}
