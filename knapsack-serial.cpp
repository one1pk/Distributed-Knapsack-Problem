
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
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
    // top-down approach
    vector<vector<int>> dp(n+1, vector<int>(S+1)); 

    for(int i = n; i >= 0; i--) {
        for(int j = 0; j <= S; j++) {
            if(i==n) {
                // no items to add when bag is full
                dp[i][j] = 0; // initial condition
            }
            else {
                int* choices = new int[2];     
                choices[0] = dp[i + 1][j];
                if(j >= s[i]) {
                    choices[1] = dp[i+1][j-s[i]]+v[i];
                }
                //assuming no negative values
                dp[i][j] = max(choices[0], choices[1]); 
                delete[] choices;
            }
        }
    }
    int result = dp[0][S];
    print_items(n,dp,s,v,S);
    return result;
}

int main() {
    vector<int> weights, values;
    weights = {20, 10, 40, 30};
    values = {40, 100, 50, 60};

    int num_items, capacity;
    num_items = weights.size();
    capacity = DEFAULT_MAX_WEIGHT;

    printf("Starting knapsack solving...\n"); 

    
    cout << knapsack_serial(num_items, weights, values, capacity) << "\n";

    return 0;
}