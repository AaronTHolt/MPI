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
n=2 arcs, np=2
3/2 =1.5
ceil(1.5) = 2
Each process gets 2 arcs
*/


float f(float x)
{ 
    return x*x - x; 
}

float simpson(float a, float b, int n, int verbose, int world_rank)
{
    float integral_edges, integral_4s, integral_2s;
    double step;
    int i;
    step = (b-a)/n;
    integral_edges = 0.0;
    integral_2s = 0.0;
    integral_4s = 0.0;
    int interval[n];

    //1+4+2+4+...+1
    //Edges
    integral_edges = (f(a) + f(b));
    //4's
    for (i=1; i<n; i=i+2)
    {
    	integral_4s += 4*f(a + i*step);
    	if (verbose == 1)
	    {
	    	if (world_rank == 0)
	    	{
	    		interval[i] = 4;
	    	}
	    }
    }
    //2's
    for (i=2; i<n; i=i+2)
    {
    	integral_2s += 2*f(a + i*step);
    	if (verbose == 1)
	    {
	    	if (world_rank == 0)
	    	{
	    		interval[i] = 2;
	    	}
	    }
    }

    //All process
    if (world_rank == 0)
    {
    	if (verbose == 1)
    	{
    		interval[0] = 1;
			interval[n-1] = 1;
	    	printf("Interval for all processes = { ");
			for (i=0; i<n; i++)
			{
				printf("%d ", interval[i]);
			}
			printf("}\n");
    	}
    }
   	
    return (step / 3) * (integral_4s + integral_2s + integral_edges);
}



int main(int argc, char *argv[]) 
{
	int i, j; //counters for loops
	int n = 1000; //Default number of arcs
	//a and b are lower and upper bounds respectively
	//Process n is number of areas per process
	float a, b, step, process_n; 
	int verbose = 0; //Default verbose off
	char option_n[] = "-n"; 
	char option_v[] = "-verbose";
	char option_v2[] = "-v";


	//Argument parsing. 
	for (i=1; i< argc; i++) 
	{
		//Get n value if present
		if (sscanf (argv[i], "%i", &n)!=1) {}
		//Verbose?
		if (strcmp(argv[i], option_v)==0) {
			// printf("%s\n", argv[i]);
			verbose = 1;
		}
		if (strcmp(argv[i], option_v2)==0) {
			// printf("%s\n", argv[i]);
			verbose = 1;
		}
	}
	// printf("N val = %d\n", n);

	//Start MPI section	
	

    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);


    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Get the name of the processor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    if (world_rank == 0)
    {
        a = 0; //lower bound
        b = 1; //upper bound
        //number of arcs per process, round up in case
        //of indivisible amount
        process_n = ceil( ((float)n) / ((float)world_size) );
        //Update n so when I print it at the end it's correct
        n = process_n * world_size;
    }

    //Broadcast relevant information to all processes
    MPI_Bcast(&a, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&b, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&process_n, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&verbose, 1, MPI_INT, 0, MPI_COMM_WORLD);

    float local_a, local_b, local_result;
    int interval[(int)(process_n/world_size)]; //To print out intervals

    //Process specific start point
    local_a = ((b - a) / world_size) * world_rank + a;
    //Process specific end point
    local_b = ((b - a) / world_size) * (world_rank + 1) + a;

    //Show numeric intervals each process did
    if (verbose == 1)
    {
    	if (world_rank==0)
	    {
	    	float start, end;
	    	for (i=0; i<world_size; i++)
	    	{
				start = ((b - a) / world_size) * i + a;
	    		end = ((b - a) / world_size) * (i + 1) + a;
	    		printf("Process %d from %f to %f \n", i, start, end);
	    	}
	    }
    }
    

    //Calculate local results
    local_result = simpson(local_a, local_b, process_n, verbose, world_rank);
    // printf("result = %f,  process %d \n", local_result, world_rank);

    //Send results, adding along the way
    //Account for 1 process case
    if (world_size > 1)
    {
        //Start sending with last process
        if (world_rank == (world_size-1))
        {
            MPI_Send(&local_result, 1, MPI_FLOAT, world_rank-1, 0, MPI_COMM_WORLD); 
        }
        else
        {
            //temp variable so the receive doesn't overwrite
            float temp = local_result;
            //Receive intermediate result
            MPI_Recv(&local_result, 1, MPI_FLOAT, world_rank+1, 0, MPI_COMM_WORLD, 
                MPI_STATUS_IGNORE);
            local_result += temp;  //Add intermediate result to current total
            //Send new intermediate result along
            if (world_rank != 0)
            {
                MPI_Send(&local_result, 1, MPI_FLOAT, world_rank-1, 0, MPI_COMM_WORLD);
            }
            //Display final result
            else
            {
                printf("Final result with n = %d:  %f \n", n, local_result);
            }
        }
    }
    else
    {
        printf("Final result with n = %d: %f \n", n, local_result);
    }

    MPI_Finalize();
}