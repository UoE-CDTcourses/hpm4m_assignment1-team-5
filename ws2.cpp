
#include <stdio.h>
#include "mpi.h"
#include <iostream>
using namespace std;

int main()
{
    int rank, nproc, sum, N = 4;
    int A[N][N];
    int B[N][N];
    int C[N][N];
    int AA[N],CC[N];


    //initialize MPI
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    //generate matrix B in process rank 0
    if (rank == 0){
        for(int i = 0; i < N; i++){
            for(int j = 0; j < N; j++){
                B[i][j]= (j+i+2)*(N-(j+1)+1);
            }
        }
    }

    //broadcast matrix B to all processes
    MPI_Bcast(B, N*N, MPI_INT, 0, MPI_COMM_WORLD);
    

    //perform in all processes
    for (int k = 0; k < N; k++){
        //generate kth row of matrix A
        if (rank == k){
            for(int i = 0; i < N; i++){
                AA[i]=(N-(i+1)+(k+1)+1)*(k+1);
            }
        
            //perform vecotr multiplication 
            for (int i = 0; i < N; i++){
                sum = 0;
                for (int j = 0; j < N; j++){
                    sum = sum + AA[j] * B[j][i];            
                }
                CC[i] = sum;
            }
        }
    }

    //gather results from all processes
    MPI_Gather(CC, N, MPI_INT, C, N, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);        
    //end MPI
    MPI_Finalize();

    //output results into a file
    if (rank == 0){          
        FILE *f = fopen("result.txt", "w");
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                fprintf(f, "%d\t", C[i][j]); 
            }       
            fprintf(f, "\n"); 
        }     
        fclose(f); 
    }
}
