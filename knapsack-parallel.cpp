#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <thread>
#include "core/utils.h"
using namespace std;

int n, S;
vector<int> s, v;
int num_threads;

CustomBarrier barrier(4);

/*
  Prints the indexes of items included in the dynamic table
 */
void print_items(vector<vector<int>> &dp)
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

void parallel_part(vector<vector<int>> dp, int start, int end) {
    for(int i = n; i >= 0; i--) {
        for(int j = start; j < end; j++) {
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
                // cout << start << ", end: " << end << ", rslt: " << dp[i][j] << endl;
                delete[] choices;
            }
        }
        barrier.wait();
    }
}

/*
 * Solution is based on Dynamic Programming Paradigm
 */
int knapsack_parallel() {
    // matrix of maximum values obtained after all intermediate combinations of items
    vector<vector<int>> dp(n+1, vector<int>(S+1)); 
    // dp[i][j] is the maximum value that can be obtained by using a subset of the items (i...n−1) (last n−i items) which weighs at most j pounds
    
    int work = (S+1)/num_threads;
    int rem_work = (S+1)%num_threads;
    vector<thread> threads;

    // top-down approach
    
        // cout << i << endl;
        int start,end = 0;
        for(int t = 0; t < num_threads; t++){
            start = end;
            if(t<rem_work){
                end = start+work+1;
            }
            else{
                end = start+work;
            }
            threads.push_back(thread(parallel_part, dp, start, end));
        }
        for(int t = 0; t < num_threads; t++){
            threads[t].join();
        }
        threads.~vector();


    print_items(dp);

    int result = dp[0][S]; 
    dp.~vector();   
    return result;
}

int main(int argc, char **argv) {
    /* if(argc!=2){
        cout << "Please enter one input file!" << "\n";
        return 0;
    } */

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
    s = {1, 2, 3, 4, 5, 6, 7, 8,9,10,11,12,13,14,15,16,17,18,19,20,21,22};
    v = {1,2,3,4,5,6,7,8,9,0,0,9,8,7,6,5,4,3,2,1,1,2};
    
    n = s.size();
    S = 60;
    num_threads = 4;

    printf("Starting knapsack solving...\n"); 

    int max_val = knapsack_parallel();

    cout << "Maximum value: " << max_val << endl;

    s.~vector();
    v.~vector();

    return 0;
}
