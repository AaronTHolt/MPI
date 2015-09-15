#include "mpi.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "string.h"
#include <time.h>

//Aaron Holt

int main(int argc, char *argv[]) 
{

	//n is nearest power of 2 processes
	//if 3 processes are used, n will be 4.
	int n; 
	//number of iterations needed to reduce
	int iterations;
	//tag to determine operation
	int tag;

	//For now, hardcode tag (operation)
	tag = 0; //tag = 0 => addition

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


    //Timing
    // clock_t t;
    // t = clock();
    // double total_time;
    // total_time = 0;
    double total_time;
    total_time = 0;
    double starttime, endtime;
    starttime = MPI_Wtime();

    for (int j=0; j<=11; j++)
    {
        
        /// REDUCE
        double data, local_result;
        int received;
        data = 1.0;
        received = 0;

        if (world_rank == 0)
        {
            received = 1;
            MPI_Send(&data, 1, MPI_DOUBLE, world_rank+1, tag, MPI_COMM_WORLD);
        }

        while (1)
        {
            if (received == 0)
            {
                local_result = data;
                MPI_Recv(&data, 1, MPI_DOUBLE, world_rank-1, tag, MPI_COMM_WORLD,
                                MPI_STATUS_IGNORE);
                data = local_result + data;
                if (world_rank == world_size-1)
                {
                    break;
                }
                MPI_Send(&data, 1, MPI_DOUBLE, world_rank+1, tag, MPI_COMM_WORLD);
                break;
            }
            break;
        }
        // printf("Process %d has data = %f \n", world_rank, data);

        /// BCAST
        received = 0;
        if (world_rank == world_size-1)
        {
            received = 1;
            MPI_Send(&data, 1, MPI_DOUBLE, world_rank-1, tag, MPI_COMM_WORLD);
        }
        while (1)
        {
            if (received == 0)
            {
                MPI_Recv(&data, 1, MPI_DOUBLE, world_rank+1, tag, MPI_COMM_WORLD,
                                MPI_STATUS_IGNORE);
                if (world_rank == 0)
                {
                    break;
                }
                MPI_Send(&data, 1, MPI_DOUBLE, world_rank-1, tag, MPI_COMM_WORLD);
                break;
            }
            break;
        }
        // printf("Process %d has data = %f \n", world_rank, data);

        
        // t = clock() - t;
        // double time_taken = ((double)t)/CLOCKS_PER_SEC;
        // double global_time;
        // MPI_Allreduce(&time_taken, &global_time, 1,
        //           MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
        // total_time += global_time;
        // if (world_rank <= 1)
        // {
        //     printf("Iteration %d, My time %f, total_time %f\n",
        //         j, endtime-starttime, total_time);
        // }

    }
    //Timing
    endtime = MPI_Wtime();
    total_time = total_time + endtime - starttime;
    total_time = total_time / 11.0;
    if (world_rank == 0)
    {
        printf("%f\n\n", total_time);
    }
    
    

    


    // printf("Process %d has data = %f \n", world_rank, data);


    MPI_Finalize();
}