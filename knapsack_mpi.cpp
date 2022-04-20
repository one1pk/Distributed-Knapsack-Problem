#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <mpi.h>
#include "core/utils.h"
#include "core/problemInput.h"
#include "core/get_time.h"
#define ROOT_RANK  0
using namespace std;

int n, S;
vector<int> s, v;

int world_size;
int world_rank;

/*
 *Maps a column of the table to the rank of the process that is working on that column
 */
static int getRankFromColumn(uint weight)
{
    return weight % world_size;
}

/*
  Prints the indexes and values of items included in the dynamic table
 */
void display_items(vector<vector<int>> &dynamic_tbl)
{
    int result = dynamic_tbl[0][S];
    int j = S;
    string indexes, values;
    for(int i=0; i<n && result>0; i++) {
        /*
        *if result from dynamic_tbl[i+1][w] -> item is not included 
        *or from v[i+1]+K[i+1][w-s[i-1]] -> if this then this item is included 
        */
       if(result == dynamic_tbl[i+1][j]) {
           continue;
       }
       else {
           indexes +=  to_string(i+1) + ", ";
           values += to_string(v[i+1]) + ", ";
           result-= v[i];
           j-=s[i];
       }
    }
    cout << "Item Indexes: \n" << indexes << "\n Values: \n" << values;
}


void fill_table(int p, vector<vector<int>>& dynamic_tbl, double * time) 
{
    timer timer;
    timer.start(); 
    MPI_Request request;
    for(int i = n; i >= 0; i--) { 
        for(int j = world_rank; j < S+1; j += world_size) {
            
            if(i == n) {
                dynamic_tbl [i][j] = 0; //base case, no items to add
            } else {
               
                vector<int> choices(2);
                // 1st choice: Don't add item i to knapsack 
                choices[0] = dynamic_tbl[i+1][j];
                // 2nd choice: Add item i to knapsack
                if(j >= s[i]) {
                    
                    MPI_Recv (  &choices[1], 
                                1, 
                                MPI_INT, 
                                getRankFromColumn(j-s[i]), 
                                i+1, 
                                MPI_COMM_WORLD, 
                                MPI_STATUS_IGNORE);
                    choices[1] += v[i];     
                }
                /*compare available choices*/
                dynamic_tbl[i][j] = max(choices[0], choices[1]); 
            }
            /*send the result to processes that is waiting on the (j+s[i-1]) item*/
            if( (j+s[i-1]) <= S) {
            MPI_Isend(  &dynamic_tbl[i][j], 
                        1, 
                        MPI_INT, 
                        getRankFromColumn(j+s[i-1]),
                        i,
                        MPI_COMM_WORLD,
                        &request
                        );
            }     
        }   
    }    
    *time = timer.stop();
}


/*
 * Solution is based on Dynamic Programming Paradigm
 */
int knapsack_parallel(int world_rank) {
    
    timer global_timer;
    double global_time_taken = 0.0;
    double local_time_taken = 0.0;
    if (world_rank == ROOT_RANK) {
        global_timer.start();
    }
    uint rows = n+1;
    uint columns = S+1;
    // matrix of maximum values obtained after all intermediate combinations of items
    vector<vector<int>> dynamic_tbl(rows, vector<int>(columns)); 
    // dynamic_tbl[i][j] is the maximum value that can be obtained by using a subset of the items (i...n−1) (last n−i items) which weighs at most j pounds
   
    fill_table (world_rank, dynamic_tbl, &local_time_taken);
    printf("%d, %g\n", world_rank, local_time_taken);

    //collecting results from all processes
    MPI_Request request;
    int * temp = (int*) malloc (sizeof(int) * rows);
    if(world_rank != ROOT_RANK){
        for(int j = world_rank; j<columns; j+=world_size) {
            // send results to root rank, one column at a time
            for(int i=0; i < rows; i++) {
                    temp[i] = dynamic_tbl[i][j];
            }
            if(world_rank != ROOT_RANK) {
                
                MPI_Send(  temp, 
                            rows, 
                            MPI_INT, 
                            ROOT_RANK,
                            j,
                            MPI_COMM_WORLD
                            );
            }
        }
    }else{
        
        for(int j = 0; j<columns; j++){
            // receive filled columns at root rank
            if(getRankFromColumn(j) != ROOT_RANK) {
                MPI_Recv(   temp,
                            rows,
                            MPI_INT,
                            getRankFromColumn(j),
                            j,
                            MPI_COMM_WORLD,
                            MPI_STATUS_IGNORE
                            );
                for(int i=0; i<rows; i++)
                {
                    dynamic_tbl[i][j] = temp[i];
                }
            }
            
        }
    }
    free(temp);

    MPI_Barrier(MPI_COMM_WORLD);
    if(world_rank == ROOT_RANK) {
        global_time_taken = global_timer.stop();
        cout << "Total time taken (in seconds) : " << std::setprecision(5)<< global_time_taken << "\n";
        display_items(dynamic_tbl);
    }
    
    int result = dynamic_tbl[0][S]; 
    return result;
}

int main(int argc, char **argv) {
   
    cxxopts::Options options(
      "0-1 Knapsack MPI solver",
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
    S = problemInstance.ProblemInput_SetCapacity(capacity);
    n = problemInstance.ProblemInput_GetNumItems();
    s = problemInstance.weights;
    v = problemInstance.values;
    
    MPI_Init(NULL, NULL);
    // Get the number of processes
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    
    // Get the rank of the process
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    if (world_rank == ROOT_RANK) {
        cout << "Starting knapsack solving..."<<"\n"; 
        cout << "Number of processes : " << world_size << "\n";
        cout << "Number of items : "<< n <<"\n";
        cout << "Capacity : "<< capacity <<"\n";
        cout << "rank, time_taken\n";
    }
    int max_val = knapsack_parallel(world_rank);
    if(world_rank == ROOT_RANK){
        cout << "\nMaximum value: " << max_val << endl;
    }
    MPI_Finalize();
    
    return 0;
}
