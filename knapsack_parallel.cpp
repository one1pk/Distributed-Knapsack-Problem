#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <thread>
#include "core/utils.h"
#include "core/problemInput.h"
#include "core/get_time.h"
#define DEFAULT_THREADS "4"

using namespace std;

int n, S;
vector<int> s, v;
int num_threads;

CustomBarrier barrier(num_threads);

/*
  Prints the indexes and values of items included in the dynamic table
 */
void display_items(vector<vector<int>> &dynamic_tbl)
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

void fill_table(vector<vector<int>>& dynamic_tbl, int start, int end, double& time) {
    timer timer;
    timer.start(); 

    for(int i = n; i >= 0; i--) {
        for(int j = start; j < end; j++) {
            if(i==n) {
                // no items to add when bag is full
                dynamic_tbl[i][j] = 0; // initial condition
            }
            else {
                vector<int> choices(2);
                // 1st choice: Don't add item i to knapsack 
                choices[0] = dynamic_tbl[i + 1][j]; // resuse optimal solution of i+1
                if(j >= s[i]) {
                    // 2nd choice: Add item i to knapsack
                    choices[1] = dynamic_tbl[i+1][j-s[i]]+v[i]; // add value of item i to optimal solution of i+1 that weighs at most j-s[i] 
                }
                dynamic_tbl[i][j] = max(choices[0], choices[1]); 
            }
        }
        barrier.wait();
    }
    time = timer.stop();
}

/*
 * Solution is based on Dynamic Programming Paradigm
 */
int knapsack_parallel() {
    timer time;
    double global_time_taken = 0.0;
    vector<double> local_time_taken(num_threads);
    
    // matrix of maximum values obtained after all intermediate combinations of items
    vector<vector<int>> dynamic_tbl(n+1, vector<int>(S+1)); 
    
    time.start();
    // distribute columns of table among threads
    int work = (S+1)/num_threads;
    int rem_work = (S+1)%num_threads;
    vector<thread> threads;
    
    int start,end = 0;
    for(int t = 0; t < num_threads; t++){
        start = end;
        if(t<rem_work){
            end = start+work+1;
        }
        else{
            end = start+work;
        }
        threads.push_back(thread(fill_table, ref(dynamic_tbl), start, end, ref(local_time_taken[t])));
    }
    for(int t = 0; t < num_threads; t++){
        threads[t].join();
    }
    for(int t = 0; t < num_threads; t++){
        cout << t <<", "<< local_time_taken[t] << std::setprecision(5)<< "\n";
    }

    global_time_taken = time.stop();
    cout<<"Total time taken (in seconds): " << global_time_taken << std::setprecision(5) << endl;
    
    display_items(ref(dynamic_tbl));

    int result = dynamic_tbl[0][S]; 
    return result;
}

int main(int argc, char **argv) {

    cxxopts::Options options(
      "0-1 Knapsack Parallel solver",
      "Maximize profit from given items' weights and values with bounded capacity");
    options.add_options(
        "",
        {
            {"capacity", "Maximum allowed capacity",
            cxxopts::value<uint>()->default_value(DEFAULT_CAPACITY)},
            {"threads", "Number of threads",
            cxxopts::value<uint>()->default_value(DEFAULT_THREADS)}
        }
    );
    auto cl_options = options.parse(argc, argv);
    uint capacity = cl_options["capacity"].as<uint>();
    uint threads = cl_options["threads"].as<uint>(); 

    ProblemInput problemInstance; 
    
    num_threads = threads;
    barrier.num_of_workers_ = num_threads;
    S = problemInstance.ProblemInput_SetCapacity(1000);
    n = problemInstance.ProblemInput_GetNumItems();
    s = problemInstance.weights;
    v = problemInstance.values;

    printf("Starting knapsack solving...\n"); 
    cout << "Number of threads : " << num_threads << "\n";
    cout << "Number of items : "<< n <<"\n";
    cout << "Capacity : "<< capacity <<"\n";
    cout << "rank, time_taken\n";
    
    int max_val = knapsack_parallel();

    cout << "\nMaximum value: " << max_val << endl;

    return 0;
}
