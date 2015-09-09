#include "mpi.h"
#include <stdio.h>
#include <math.h>

//Aaron Holt
//HW1 Program 2: trapezoidal rule
//Run on 6core AMD64 using MPICH

/*
In this program I assume a is the lower bound and b is the upper bound.
In the even of a factional number of trapezoids for each process, I just round up
to the nearest number of trapezoids that is evenly disivible.
Example:
3 trapezoids, n=2
3/2 =1.5
ceil(1.5) = 2
Each process gets 2 trapezoids
*/


float f(float x)
{ 
    return x*x; 
}

float Trap(float a, float b, int n)
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


int main(int argc, char** argv) {

    float a, b, step, process_n;
    int n;

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
        a = 0;
        b = 1;
        n = 3;
        //number of traps per process, round up in case
        //of indivisible amount
        process_n = ceil( ((float)n) / ((float)world_size) );
        //Update n so when I print it at the end it's correct
        n = process_n * world_size;
    }

    // printf("n = %d,  process %d \n", n, world_rank);

    //Broadcast relevant information to all processes
    MPI_Bcast(&a, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&b, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&process_n, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);

    // printf("n = %f,  process %d \n", process_n, world_rank);

    float local_a, local_b, local_result;
    //Process specific start point
    local_a = ((b - a) / world_size) * world_rank + a;
    //Process specific end point
    local_b = ((b - a) / world_size) * (world_rank + 1) + a;

    // printf("a = %f, b = %f, process %d \n", local_a, local_b, world_rank);

    local_result = Trap(local_a, local_b, process_n);

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
                printf("Final result with %d trapezoids = %f \n", n, local_result);
            }
        }
    }
    else
    {
        printf("Final result with %d trapezoids = %f \n", n, local_result);
    }


    MPI_Finalize();
}




