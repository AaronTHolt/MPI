#include "mpi.h"
#include <stdio.h>

int main(int argc, char** argv) {
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


    //Send something in messages to prove they were sent from a specific process
    int number;

    //Process 0
    if (world_rank == 0)
    {
        //Initilize number to be the process number
        number = 0;
        //Send to process 1 if world size>1
        //Send to self if world size is 1
        if (world_size > 1)
        {
            MPI_Send(&number, 1, MPI_INT, 1, 0, MPI_COMM_WORLD); 
            //Recv from last process.
            MPI_Recv(&number, 1, MPI_INT, world_size-1, 0, MPI_COMM_WORLD, 
                MPI_STATUS_IGNORE);  
        }
        printf("Process %d received from process %d \n", world_rank, number);
    }
    else if (world_rank != 0)
    {
        //Receive from process rank-1
        MPI_Recv(&number, 1, MPI_INT, world_rank-1, 0, MPI_COMM_WORLD, 
            MPI_STATUS_IGNORE);
        printf("Process %d received from process %d \n", world_rank, number);

        //If its the last processor send to process 0,
        //Otherwise send to rank+1
        number = world_rank;
        if (world_rank == world_size-1)
        {
            MPI_Send(&number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
        else
        {
            MPI_Send(&number, 1, MPI_INT, world_rank+1, 0, MPI_COMM_WORLD);
        }
    }

    // Finalize the MPI environment.
    MPI_Finalize();
}
