#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include "core/problemInput.h"
#include "core/get_time.h"
#include "core/types.h"
#define DEFAULT_N          4
#define DEFAULT_MAX_WEIGHT 60
using namespace std;

/*
  Prints the indexes of items included in the dynamic table
 */
static void display_items(int n,vector<vector<long long>> &dp ,vector <long> &s, vector <long> &v, int S)
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
           cout<<i+1<<", ";
           result-= v[i];
           S-=s[i];
       }
    }
    cout<<endl;
}

/*
 * Solution is based on Dynamic Programming Paradigm
 */
static long long knapsack_serial(int n, vector<long> &s, vector<long> &v, int S) {
    timer time;
    double time_taken = 0.0;
    // matrix of maximum values obtained after all intermediate combinations of items
    vector<vector<long long>> dp(n+1, vector<long long>(S+1, 0));

    // dp[i][j] is the maximum value that can be obtained by using a subset of the items (i...n−1) (last n−i items) which weighs at most j pounds
    time.start();
    // top-down approach
    for(int i = n; i >= 0; i--) {
        for(int j  = 0;j <= S; j++) {
            if(i==n) {
                // no items to add when bag is full
                dp[i][j] = 0; // initial condition
            }
            else {
                int* choices = new int[2] (); 
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

    int result = dp[0][S]; 
    time_taken = time.stop();
    cout<<"Time taken (in seconds): " << time_taken << std::setprecision(TIME_PRECISION) << endl;
    
    display_items(n, dp, s, v, S);
    return dp[0][S];
}

int main(int argc, char **argv) {
    
    ProblemInput problemInstance; 
    

    int capacity = problemInstance.ProblemInput_SetCapacity(5000);

    printf("Starting knapsack solving...\n"); 
    
    
    int max_val = knapsack_serial(  problemInstance.ProblemInput_GetNumItems(),
                                    problemInstance.weights, 
                                    problemInstance.values, 
                                    capacity);

    cout << "Maximum value: " << max_val << endl;
    

    return 0;
}
