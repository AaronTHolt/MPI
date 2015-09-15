#include "mpi.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "string.h"

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


    printf("Process %d has data = %f \n", world_rank, data);


    MPI_Finalize();
}