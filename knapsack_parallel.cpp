#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>
#include <vector>
#include "core/problemInput.h"
#include "core/get_time.h"
#include "core/types.h"
#include "core/utils.h"

using namespace std;
typedef struct worker
{
    uint thread_id;

}worker;
/*
  Prints the indexes of items included in the dynamic table
 */
static void display_items(int n,vector<vector<long long>> &dp ,vector <long> &s, vector <long> &v, int S)
{
    int result = dp[0][S];
    
    cout<<"Items included (by index): [";
    for(int i=0; i<n && result>0; i++)
    {
       // if result from dp[i+1][w] -> item is not included 
       // or from v[i+1]+K[i+1][w-s[i-1]] -> if this then this item is included 
       if(result == dp[i+1][S]){
           continue;
       }
       else {
           cout<<i+1<<", ";
           result-= v[i];
           S-=s[i];
       }
    }
    cout<<" ]"<<endl;
}

void parallel_part(int t, vector<vector<int>>& dp, int start, int end) {

    for(int i = n; i >= 0; i--) {
        for(int j  = 0;j <= S; j++) {
            if(i==n) {
                // no items to add when bag is full
                dp[i][j] = 0; // initial condition
            }
            else {
                int* choices = new int[2] (); 
                // 1st choice: Don't add item i to knapsack 
                choices[0] = dp[i + 1][j]; // reuse optimal solution of i+1
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

    time_taken = time.stop();
    cout<<"Time taken (in seconds): " << time_taken << std::setprecision(TIME_PRECISION) << endl;
    display_items (n, dp, s, v, S);
    return dp[0][S];
}
static void knapsack_parallel(worker* worker_thread)
{
    
}


int main(int argc, char **argv) {
    
    // Initialize command line arguments
  cxxopts::Options options("Parallel Knapsack solver");
  options.add_options(
      "custom",
      {
          {"capacity", "Maximum weight allowed in the knapsack",         
           cxxopts::value<uint>()->default_value(DEFAULT_CAPACITY)},
	        
           {"nThreads", "Number of threads",
           cxxopts::value<uint>()->default_value(DEFAULT_NUMBER_OF_WORKERS)}
      });
    auto cl_options = options.parse(argc, argv);

    uint n_threads=cl_options["nThreads"].as<uint>();
    uint capacity = cl_options["capacity"].as<uint>();
    std::cout << "Capacity : " << capacity << "\n";;
    std::cout <<"Number of threads : "<<n_threads<<"\n";

    worker workers [n_threads];
    std::thread threads[n_threads];
    for(int i=0; i < n_threads; i++)
    {
        threads[i] = thread(knapsack_parallel, &workers[i]);
    }
    for(int i=0; i < n_threads; i++)
    {
        threads[i].join();
    }

    ProblemInput problemInstance; 
    


    printf("Starting knapsack solving...\n"); 
    
    
    int max_val = knapsack_parallel(  problemInstance.ProblemInput_GetNumItems(),
                                    problemInstance.weights, 
                                    problemInstance.values, 
                                    capacity);

    cout << "Maximum value: " << max_val << endl;
    

    return 0;
}
