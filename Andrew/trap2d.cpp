//
// parallelised matrix multiplier
// Size of NxN matrices, N, needs to be multiple of number of processes used (size)
//

#include <iostream>
#include <mpi.h>
#include <cmath>
#include <stdlib.h>
#include <chrono>

using namespace std;

//return the fucntion to be integrated
double f(double x,double y){
  return x*sin(x*x) + y*sin(y*y);
}

//calculate the integral of f over a square using the trapezoidal rule
double trap(double x_lower, double x_upper, double y_lower, double y_upper, double h){
  return (f(x_lower,y_lower)+f(x_lower,y_upper)+f(x_upper,y_lower)+f(x_upper,y_upper))*h*h/4.0;
}

//serial implementation of integration
double serial(double upper, int N){
  double h = upper/double(N);
  double result = 0;
  for (int j=0; j<N; j++) {
    for (int i=0; i<N; i++) {
      result += trap(i*h,(i+1)*h,j*h,(j+1)*h,h);
    }
  }
  return result;
}

//parallelised implementation dividing the region in strips
double strips(double upper, int N, int rank, int size){
  double h = upper/double(N);
  int Nproc = int(N/size);
  double result = 0;
  for (int j=Nproc*rank; j<Nproc*(rank+1); j++) {
    for (int i=0; i<N; i++) {
      result += trap(i*h,(i+1)*h,j*h,(j+1)*h,h);
    }
  }
  return result;
}

//parallelised implementation dividing the region in squares
double squares(double upper, int N, int rank, int size){
  double h = upper/double(N);
  int k = int(sqrt(size));   //number of squares in each axis
  int Nproc = int(N/k);      //number of points in each axis
  
  int y_index = floor(rank/k);    //index of square in y direction
  int x_index = rank%k;           //index of square in x direction
  
  double result = 0;
  for (int j=Nproc*y_index; j<Nproc*(y_index+1); j++) {
    for (int i=Nproc*x_index; i<Nproc*(x_index+1); i++) {
      result += trap(i*h,(i+1)*h,j*h,(j+1)*h,h);
    }
  }
  return result;
}

//return the analytic result
double analytic(double a, double b){
  return 0.5*(-b*cos(a*a)-a*cos(b*b)+a+b);
}

//return the absolute error
double error(double num, double an){
  return abs(num-an)/an;
}

int main(){
  
  int rank, size, ierr;
  MPI_Comm comm;

  comm  = MPI_COMM_WORLD;      //store the global communicator in comm variable

  MPI_Init(NULL,NULL);         //initialise MPI
  MPI_Comm_rank(comm, &rank);  //process identifier stored in rank variable
  MPI_Comm_size(comm, &size);  //number of processes stored in size variable
  
  int N = pow(10,3);           //set the resolution here
  double ser, str, sq;
  double anal;
  if(rank==0){                 //calculate the analytic answer on the root process
    anal = analytic(100,100);
    cout << "N = " << N << endl;
    cout << "Analytic = " << anal << endl;
    cout << "NumericalType   Result    Time    Abs Error" << endl;
    
    //--------Serial-----------
    auto start = chrono::high_resolution_clock::now();
    ser = serial(100,N);
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
    cout << "Serial " << ser << " " << duration.count() << " " << error(ser, anal) << endl;
  }

  
  //---------Strips------------
  auto start2 = chrono::high_resolution_clock::now();
  str = strips(100,N,rank,size);
  double strR;
  MPI_Reduce(&str, &strR, 1, MPI_DOUBLE, MPI_SUM, 0, comm);
  auto stop2 = chrono::high_resolution_clock::now();
  auto duration2 = chrono::duration_cast<chrono::microseconds>(stop2 - start2);
  if (rank==0) {
    cout << "Strips " << strR << " " << duration2.count() << " " << error(strR, anal) << endl;
  }
  
  //----------Squares----------
  auto start3 = chrono::high_resolution_clock::now();
  sq = squares(100,N,rank,size);
  double sqR;
  MPI_Reduce(&sq, &sqR, 1, MPI_DOUBLE, MPI_SUM, 0, comm);
  auto stop3 = chrono::high_resolution_clock::now();
  auto duration3 = chrono::duration_cast<chrono::microseconds>(stop3 - start3);
  if (rank==0) {
    cout << "Squares " << sqR << " " << duration3.count() << " " << error(sqR, anal) << endl;
  }
  
  
  MPI_Finalize();   //need to finalise MPI

}
