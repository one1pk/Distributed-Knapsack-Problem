#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <thread>
#include "core/utils.h"
#include "core/problemInput.h"
#include "core/types.h"

using namespace std;

int n, S;
vector<long> s, v;
int num_threads;

CustomBarrier barrier(4);

// for debugging only
void print_dp(vector<vector<int>> &dp){
    for (const auto &row : dp)
    {
        for (const auto &s : row)
            cout << s << ' ';
        cout << endl;
    }
}

/*
  Prints the indexes of items included in the dynamic table
 */
static void display_items(vector<vector<int>> &dp)
{
    int result = dp[0][S];
    int j = S;

    cout<<"Items included (by index): "<<endl;
    for(int i=0; i<n && result>0; i++)
    {
       // if result from dp[i+1][w] -> item is not included 
       // or from v[i+1]+K[i+1][w-s[i-1]] -> if this then this item is included 
       if(result == dp[i+1][j]){
           continue;
       }
       else {
           cout<<i+1<<", ";
           result-= v[i];
           j-=s[i];
       }
    }
    cout<<endl;
}

void parallel_part(int t, vector<vector<int>>& dp, int start, int end) {
    for(int i = n; i >= 0; i--) {
        for(int j = start; j < end; j++) {
            if(i==n) {
                // no items to add when bag is full
                dp[i][j] = 0; // initial condition
            }
            else {
                // int* choices = new int[2](); 
                vector<int> choices(2);
                // 1st choice: Don't add item i to knapsack 
                choices[0] = dp[i + 1][j]; // resuse optimal solution of i+1
                if(j >= s[i]) {
                    // 2nd choice: Add item i to knapsack
                    choices[1] = dp[i+1][j-s[i]]+v[i]; // add value of item i to optimal solution of i+1 that weighs at most j-s[i] 
                }
                // assuming no negative values
                dp[i][j] = max(choices[0], choices[1]); 
                // delete[] choices;
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
    
    int start,end = 0;
    for(int t = 0; t < num_threads; t++){
        start = end;
        if(t<rem_work){
            end = start+work+1;
        }
        else{
            end = start+work;
        }
        threads.push_back(thread(parallel_part, t, ref(dp), start, end));
    }
    for(int t = 0; t < num_threads; t++){
        threads[t].join();
    }
    
    display_items(ref(dp));

    int result = dp[0][S]; 
    return result;
}

int main(int argc, char **argv) {
    /* if(argc!=2){
        cout << "Please enter one input file!" << "\n";
        return 0;
    } */

    ProblemInput problemInstance; 
    
    num_threads = 4;
    S = problemInstance.ProblemInput_SetCapacity(1500);
    n = problemInstance.ProblemInput_GetNumItems();
    s = problemInstance.weights;
    v = problemInstance.values;

    printf("Starting knapsack solving...\n"); 
    
    int max_val = knapsack_parallel();

    cout << "Maximum value: " << max_val << endl;

    return 0;
}
