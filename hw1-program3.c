#include "mpi.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "string.h"

//Aaron Holt
//HW1 Program 2: trapezoidal rule
//Run on 6core AMD64 using MPICH

/*
In this program I assume a is the lower bound and b is the upper bound.
In the even of a factional number of arcs for each process, I just round up
to the nearest number of arcs that is evenly disivible.
Example:
3 arcs, n=2
3/2 =1.5
ceil(1.5) = 2
Each process gets 2 arcs
*/


float f(float x)
{ 
    return x*x; 
}

float Arc(float a, float b, int n)
{
    float integral, lower_bound, upper_bound;
    double step;
    int i;
    // integral = (f(a) + f(b))/2.0;
    step = (b-a)/n;
    integral = 0.0;

    for (i=0; i<=n-1; i++)
    {
        lower_bound = a + step*i;
        upper_bound = a + step*(i+1);
    
        integral += (f(lower_bound)+f(upper_bound))/2;
    }
    return integral*step;
}



int main(int argc, char *argv[]) 
{
	int i;
	int n = 1000; //Default number of arcs
	int verbose = 0; //Default verbose off
	char option_n[] = "-n"; 
	char option_v[] = "-v";

	//Argument parsing. 
	for (i=1; i< argc; i++) 
	{
		//Get -n value
		if (sscanf (argv[i], "%i", &n)!=1) {}
		//Verbose?
		if (strcmp(argv[i], option_v)==0) {
			// printf("%s\n", argv[i]);
			verbose = 1;
		}
	}
	// printf("N val = %d\n", n);


	


}