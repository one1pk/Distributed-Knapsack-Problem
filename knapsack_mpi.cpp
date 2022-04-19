#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <mpi.h>
//#include "core/utils.h"
#include "core/problemInput.h"
#include "core/get_time.h"

using namespace std;

int n, S;
vector<int> s, v;
int *startx;
int *endx;;

int world_size;
int world_rank;
static int getRankFromWeight(uint weight)
{
    for(int i=0; i<world_size; i++)
    {
        // cout<<"start "<<startx[i]<<"end " << endx[i]<<endl;
        if(weight>=startx[i] && weight<endx[i])
        {
            return i;
        }
    }
    return -1;
}
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
void display_items(vector<vector<int>> &dp)
{
    int result = dp[0][S];
    int j = S;

    cout<<"Items included (by index): "<<endl;

    for(int i=0; i<n && result>0; i++)
    {
        /*
         *if result from dp[i+1][w] -> item is not included 
         *or from v[i+1]+K[i+1][w-s[i-1]] -> if this then this item is included 
         */
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

// TODO: implement send & receive
void parallel_part(int p, vector<vector<int>>& dp,int start, int end) {
   
    MPI_Request request;
    for(int i = n; i >= 0; i--) {
        for(int j = start; j < end; j++) {

            if(i == n) {
                dp [i][j] = 0; //base case, no items to add
            }
            else {
                vector<int> choices(2);
                choices[0] = dp[i+1][j];
                /*if the column is within my bounds*/
                if(j >= s[i]) {
                    if((j-s[i])>=start && (j-s[i]) < end)
                    {
                        choices[1] = dp[i+1][j-s[i]]+v[i];                    
                    }
                    else{
                        cout<<"receiving from process"<<world_rank<<"\n";
                        MPI_Recv (&choices[1], 1, MPI_INT, getRankFromWeight(j-s[i]), i+1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                        choices[1] += v[i];
                    }
                }
                dp[i][j] = max(choices[0], choices[1]); 
        
            }
            for(int k = j+1; k < S+1; k++){

                if(k == s[i-1] + j) {
                    if(!(getRankFromWeight(k) == world_rank))
                    {
                        //cout<<"sending from "<< world_rank<< " value: "<<dp[i][j] <<" to process: "<< mapSizeToProcess[k]<<"\n";
                        MPI_Isend(&dp[i][j], 1, MPI_INT, getRankFromWeight(k), i, MPI_COMM_WORLD, &request);
                    }
                }
            }
        }
	}
}
/*
 * Solution is based on Dynamic Programming Paradigm
 */
int knapsack_parallel(int world_rank) {
    timer time;
    double time_taken = 0.0;
    uint rows = n+1;
    uint columns = S+1;
    // matrix of maximum values obtained after all intermediate combinations of items
    vector<vector<int>> dp(rows, vector<int>(columns)); 
    // dp[i][j] is the maximum value that can be obtained by using a subset of the items (i...n−1) (last n−i items) which weighs at most j pounds
    // vector<int> mapSizeToProcess(columns);
    time.start();
    startx = new int [columns];
    endx = new int [columns]; 
    int work = (S+1)/world_size;
    int rem_work = (S+1)%world_size;
    
    uint min_columns_for_each_thread = columns /   world_size ;
    uint excess_columns = columns % world_size ;
    uint curr_column = 0; 
    for (uint i = 0; i < world_size; i++) {
        startx[i] = curr_column;
        if (excess_columns > 0) {
            endx[i] = curr_column + min_columns_for_each_thread +1 ;
            excess_columns--;
        } 
        else {
            endx[i] = curr_column + min_columns_for_each_thread ;
        }
        curr_column = endx[i];
    } 
    // for (int i=0; i < world_size; i++)
    // {
    //     for(int j = startx[i]; j < endx[i]; j++)
    //     {
    //         mapSizeToProcess[j] = i;
    //     }
    // }
    // if(world_rank==1){
    //     for(int i=0;i<columns;i++)
    //     {
    //         cout<<"i:"<<i<<" value="<<mapSizeToProcess[i]<<endl;
    //     }
    // }

    cout<<"Rank: "<< world_rank << "start:"<< startx[world_rank] << " end" << endx[world_rank]<<endl;
   
    parallel_part (world_rank, dp, startx[world_rank], endx[world_rank]);
    cout<<"Rank: "<< world_rank << "finished"<<endl;

    
    // if(world_rank == world_size-1)
    // {
    //    for(int i=0;i<n+1;i++)
    //     {
    //         for(int j=0;j<S+1;j++)
    //         {
    //             cout<<dp[i][j]<<", ";
    //         }
    //         cout<<endl;
    //     }
    // }
    int result = dp[0][S]; 
    delete[] startx;
    delete[] endx;
    return result;
}

int main(int argc, char **argv) {
    /* if(argc!=2){
        cout << "Please enter one input file!" << "\n";
        return 0;
    } */

    ProblemInput problemInstance; 
    S = problemInstance.ProblemInput_SetCapacity(1000);
    n = problemInstance.ProblemInput_GetNumItems();
    s = problemInstance.weights;
    v = problemInstance.values;

    MPI_Init(NULL, NULL);

    // Get the number of processes
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    // Get the rank of the process
   
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    printf("Starting knapsack solving...\n"); 
    
    int max_val = knapsack_parallel(world_rank);
    
    if(world_rank == world_size-1){
        cout << "Maximum value: " << max_val << endl;
    }
    MPI_Finalize();
    
    return 0;
}
