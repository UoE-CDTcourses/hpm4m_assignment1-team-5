#include <iostream>
#include <mpi.h>
using namespace std;

//define functions A and B to compute matrix elements 
//(A_func(0,0,n) computes A_(1,1)

int A_func(int i,int j,int n){
 int Aij = (n - j + i + 1) * (i+1); 
 return Aij;
}

int B_func(int i,int j,int n){
 int Bij = (j + i + 2) * (n - j) ;
 return Bij;
}


 int main()
 {
  int id;
  int nproc;

//initialise MPI

  MPI_Init(NULL,NULL);
  MPI_Comm_rank( MPI_COMM_WORLD, &id);
  MPI_Comm_size (MPI_COMM_WORLD, &nproc);
  
  int n = nproc;


  int B[n][n];
  int C[n];
  int D[n][n];
  
  //process 0 calculates entire matrix B
  
  if (id ==0){
  for (int i=0; i<n; i++){
    for (int j=0; j<n; j++){
       B[i][j]=B_func(i,j,n);
      }
   }
   }

  //broadcast B to all processes and wait until all received B

    MPI_Bcast(B,n*n,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    
  //each process computes a row of A and calculates the product with B 
  //saved in C

    for (int i = 0; i<n; i++){
      int A[n];
      for (int j = 0; j<n; j++){
      A[j]=A_func(id,j,n);
      }
     int sum = 0;
     for (int j =0; j<n; j++){
       sum += A[j]*B[j][i];
    }
     C[i]=sum;
     sum =0;
    }

  //all processes send their C to process 0 and wait each process is finishied
    MPI_Send(C,n,MPI_INT,0,id,MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

  //process 0 receives all C vector of all processes and 
  //saves results of process i in matrix row i   
    if(id == 0){
     for (int i = 0; i<n; i++){
	 MPI_Recv(C,n,MPI_INT,i,i,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
         for (int j =0; j<n; j++){
	 D[i][j]=C[j];
	 }
   		 
    }

   //print result  
   printf(" D=\n");                     
   for ( int i = 0; i < n; i++ ){
      for ( int j = 0; j < n; j++ ) {
        printf(" %d",D[i][j]);
      }
      printf("\n");
   }
    
    }

   //end MPI calculations
    MPI_Finalize();
 return 0; 
 }
