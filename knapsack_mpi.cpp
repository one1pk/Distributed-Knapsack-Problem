#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <mpi.h>
//#include "core/utils.h"
#include "core/utils.h"
#include "core/problemInput.h"
#include "core/get_time.h"
#define ROOT_RANK  0
using namespace std;

int n, S;
vector<int> s, v;
int *startx;
int *endx;;

int world_size;
int world_rank;


/*
 *Maps the weight of an item to the processes' ranks that is working on that column
 */
static int getRankFromColumn(uint weight)
{
    return weight % world_size;
}
// for debugging only
// void print_dp(vector<vector<int>> &dynamicTbl){
//     string indexes, values;
//     for (const auto &row : dynamicTbl)
//     {
//         for (const auto &s : row)
//             indexes +=  s + ' ';
//             values += v[row] + ' ';
//         indexes += '\n';
//         values += '\n';
//     }
//     cout << "Item Indexes: \n" << indexes << "Values: \n" << values;
// }

/*
  Prints the indexes of items included in the dynamic table
 */
void display_items(vector<vector<int>> &dynamicTbl)
{
    int result = dynamicTbl[0][S];
    int j = S;
    cout<<"Items included (by index): "<<endl;
    for(int i=0; i<n && result>0; i++)
    {
        /*
        *if result from dynamicTbl[i+1][w] -> item is not included 
        *or from v[i+1]+K[i+1][w-s[i-1]] -> if this then this item is included 
        */
       if(result == dynamicTbl[i+1][j]){
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


void parallel_part(int p, vector<vector<int>>& dynamicTbl, double * time) 
{
    timer timer;
    timer.start(); 
    MPI_Request request;
    for(int i = n; i >= 0; i--) { 
        for(int j = world_rank; j < S+1; j += world_size) {
            
            if(i == n) {
                dynamicTbl [i][j] = 0; //base case, no items to add
            } else {
               
                vector<int> choices(2);
                // 1st choice: Don't add item i to knapsack 
                choices[0] = dynamicTbl[i+1][j];
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
                dynamicTbl[i][j] = max(choices[0], choices[1]); 
            }
            /*send the result to processes that is waiting on the (j+s[i-1]) item*/
            if( (j+s[i-1]) <= S) {
            MPI_Isend(  &dynamicTbl[i][j], 
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
    vector<vector<int>> dynamicTbl(rows, vector<int>(columns)); 
    // dynamicTbl[i][j] is the maximum value that can be obtained by using a subset of the items (i...n−1) (last n−i items) which weighs at most j pounds
   
    parallel_part (world_rank, dynamicTbl, &local_time_taken);
    cout << world_rank <<", "<< local_time_taken << std::setprecision(TIME_PRECISION)<< "\n";

    //collecting results from all processes
    MPI_Request request;
    //for(int i = n; i< rows; i--) {
        for(int j = world_rank; j<columns; j+=world_size) {
            // send results to root rank
            if(world_rank != ROOT_RANK) {
               
                MPI_Isend(  &dynamicTbl[][j], 
                            rows, 
                            MPI_INT, 
                            ROOT_RANK,
                            j,
                            MPI_COMM_WORLD,
                            &request
                         );
            }
            // receive at root rank
            else {
                if(getRankFromColumn(j) != ROOT_RANK) {
                    cout<< "receiving"<<endl;
                    MPI_Irecv( &dynamicTbl[i][j],
                                1,
                                MPI_INT,
                                getRankFromColumn(j),
                                j,
                                MPI_COMM_WORLD,
                                &request
                                );
                }
            }
        }
        MPI_Barrier(MPI_COMM_WORLD);
    //}
        
                   
    //cout<<"Rank: "<< world_rank << "finished"<<endl;

    MPI_Barrier(MPI_COMM_WORLD);
    if(world_rank == ROOT_RANK) {
        global_time_taken = global_timer.stop();
        cout << "Total time taken (in seconds) : " << std::setprecision(TIME_PRECISION)<< global_time_taken << "\n";
        display_items(dynamicTbl);
    }
    


    int result = dynamicTbl[0][S]; 
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
        std::cout << "Starting knapsack solving..."<<"\n"; 
        std::cout << "Number of processes : " << world_size << "\n";
        std::cout << "Number of items : "<<n<<"\n";
        std::cout << "Capacity : "<< capacity <<"\n";
        std::cout << "rank, time_taken\n";
    }
    int max_val = knapsack_parallel(world_rank);
    if(world_rank == ROOT_RANK){
        cout << "Maximum value: " << max_val << endl;
    }
    MPI_Finalize();
    
    return 0;
}
