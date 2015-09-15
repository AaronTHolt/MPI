#include "mpi.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "string.h"
#include <time.h>

//Aaron Holt



int main(int argc, char *argv[]) 
{


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

    double data[8*8*8*8*8*4] = { 1 };
    double global_sum[8*8*8*8*8*4];
    

	// if (world_rank == 0)
 //    {
	//     printf("n = %d, iterations = %d, mask = %d, type = %d \n", n, 
	//     	iterations, bitmask, type);
 //    }
    int k;
    double total_time;
    total_time = 0;
    double starttime, endtime;

    

    for (k=0; k<=11; k++)
    {
        starttime = MPI_Wtime();
        MPI_Allreduce(&data, &global_sum, 8*8*8*8*8, MPI_DOUBLE, MPI_SUM,
           MPI_COMM_WORLD);

        //Timing
        endtime = MPI_Wtime();
        total_time = total_time + endtime - starttime;
    }

    total_time = total_time / 11.0;
    if (world_rank == 0)
    {
        printf("%f\n\n", total_time);
    }
    

    MPI_Finalize();
}