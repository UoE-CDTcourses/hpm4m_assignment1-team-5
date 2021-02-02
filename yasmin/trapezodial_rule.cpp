#include<iostream>
#include <vector> 
using namespace std;
#include<math.h>


double trapezodial_rule(double ax, double bx, int n, double ay, double by,  double f(double,double))
{
  double hx;
  double hy;
  double sum_func;

  hx = (bx-ax) /n;
  hy = (by-ay) /n;
  sum_func = f(ax,ay)+f(bx,by);
  
  for (int j = 1; j<n; j+=1)
  {for (int i = 1; i<n; i += 1)
  {
    sum_func += 2*f(ax + i*hx,ay + j*hy);
  }
  }
  sum_func = 0.25*hx*hy*sum_func; 
  
  return sum_func;
 
 }
 
 double func( double x, double y)
 {
  double ret = x*sin(x*x) + y*sin(y*y);
  return ret;
 }
 
 int main()
 {
 	double ax = 0;
 	double ay = 0;
        double bx = 100;
        double by = 100;
        vector <double> res = {};
        for (int k = 1; k<10; k+=1){
        vector <double> res_temp = {trapezodial_rule(ax,bx,pow(10,k),ay,by,func)};
        res.insert(res.end(),res_temp.begin(),res_temp.end());
        }
        for(auto it: res){
        cout<<it<<" ";
    }
        return 0;
 }
