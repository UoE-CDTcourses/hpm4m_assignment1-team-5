//
// parallelised matrix multiplier
// Size of NxN matrices, N, needs to be multiple of number of processes used (size)
//

#include <iostream>
#include <mpi.h>
#include <vector>
#include <cmath>

using namespace std;
 
// prints out a matrix, stored as a 1d vector, as a 2d matrix
void print(const vector<int>& self){
    int n = int(sqrt(self.size()));
    for (int i=0; i<n; i++) {
        for (int j=0; j<n; j++) {
            cout << self[i*n+j] << "  ";
        }
        cout << std::endl;
    }
}

// multiplies all of the rows of the matrix A stored on the process, by the matrix B
vector<int> vecMatMul(const vector<int>& a, const vector<int>& B, int rowsProc){
  
  int n = a.size()/rowsProc;
  vector<int> result(a.size());
  int temp;
  
  for (int k=0; k<rowsProc; k++) {   //loop over the number of rows per process
    for (int i=0; i<n; i++) {        //loop over each column in B
      temp = 0;
      for (int j=0; j<n; j++) {      //do the matrix multiplication
        temp = temp + a[j+k*n]*B[i+j*n];    //awkward indexing!
      }
      result[i+k*n] = temp;
    }
  }
  
  return result;
  
}

int main(int argc, char* argv[]){
  
  int rank, size, ierr;
  MPI_Comm comm;

  comm  = MPI_COMM_WORLD;      //store the global communicator in comm variable

  MPI_Init(NULL,NULL);         //initialise MPI
  MPI_Comm_rank(comm, &rank);  //process identifier stored in rank variable
  MPI_Comm_size(comm, &size);  //number of processes stored in size variable
  
  int N = atoi(argv[1]);       //take in 1 command line argument = size of the matrices N
  int rowsProc = int(N/size);  //number of rows per process
  int Nproc = rowsProc*N;      //size of vector needed to store the processes' submatrices of A
  
  vector<int> B(N*N);
  vector<int> a(N*rowsProc);
  
  //generate rows of A on each process
  for (int k=0; k<rowsProc; k++) {    //loop over number of rows per process
    for (int j=1; j<N+1; j++) {   //loop over the row
      a[k*N+(j-1)] = (N-j+(rank*rowsProc+k+1)+1)*(rank*rowsProc+k+1);
    }
  }
    
  //Generate the matrix B on root process
  if (rank==0) {
    for(int i=1; i<N+1;i++){
      for (int j=1; j<N+1; j++) {
        B[N*(i-1)+j-1] = (j+i)*(N-j+1);
      }
    }
  }
  
  //Broadcast B matrix to all other processes
  MPI_Bcast(B.data(), B.size(), MPI_INT, 0, comm);
  
  //multiply vector on each process by B
  vector<int> Cpartial = vecMatMul(a,B,rowsProc);
  
  //Gather the c from each process to root process
  vector<int> C(N*N);     //need to create a buffer on all processes
  if (rank==0) {          //buffer can equal to NULL except on the root process
    C.resize(N*N);
  }

  MPI_Gather(Cpartial.data(), Nproc, MPI_INT, C.data(), Nproc, MPI_INT, 0, comm);

  //Print the final matrix from the root process
  if (rank==0) {
    print(C);
  }
  
  MPI_Finalize();   //need to finalise MPI

}
