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


	//High-Low or Low-High
	//1 = h->l
	//0 = l->h   default
	int type = 0;
	int j; //counter in arg parse loop

	char option_h2l[] = "-h";
	char option_H2L[] = "-H";
	char option_l2h[] = "-l";
	char option_L2H[] = "-L"; 



	//Argument parsing. 
	for (j=1; j<argc; j++) 
	{
		//Get tag (operation type) value if present
		if (sscanf (argv[j], "%i", &tag)!=1) {}

		//High to low or low to high
		if ((strcmp(argv[j], option_h2l)==0) || (strcmp(argv[j], option_H2L)==0)) {
			// printf("%s\n", argv[j]);
			type = 1;
		}
		if ((strcmp(argv[j], option_l2h)==0) || (strcmp(argv[j], option_L2H)==0)) {
			// printf("%s\n", argv[j]);
			type = 0;
		}
	}

	//For now, hardcode tag (operation)
	tag = 0; //tag = 0 => addition

	//data to add up, each process will have its own data
	double data;

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

    //Initialize data on all processes
    data = 1;

    //first masks to be used with l-h and h-l
	int bitmask;

    //Find out how many iterations are needed
    //Find bitmask
	n = pow(2, ceil(log(world_size)/log(2)));
	iterations = log(n)/log(2);
	//find bitmask
	if (type == 1)
    {
    	bitmask = n/2;
    }
    else
    {
    	bitmask = 1;
    }

	if (world_rank == 0)
    {
	    // printf("n = %d, iterations = %d, mask = %d, type = %d \n", n, 
	    // 	iterations, bitmask, type);
    }

    // Variables
    int i; //counter
    int deliver_to;	//who to send to
    int receive_from;	//who to receive from
    int delivered = 0; //In reduce, a process should only deliver once

    for (i = 0; i<iterations; i++)
    {
    	/////////////////// High to Low//////////////////////
    	if (type == 1)
    	{
    		// Check which processes should be sending
    		if ((world_rank & bitmask) == bitmask)
	    	{
	    		if (delivered == 0)
	    		{
	    			deliver_to = world_rank ^ bitmask;
		    		// printf("Process %d delivers to %d on iteration %d \n", 
		    		// 	world_rank, deliver_to, i);
		    		MPI_Send(&data, 1, MPI_DOUBLE, deliver_to, tag, MPI_COMM_WORLD);
		    		delivered = 1;
	    		}
	    	}
	    	else
	    	{
	    		//Check which processes should be receiving
	    		if (world_rank < bitmask)
	    		{
	    			double local_result;
		    		local_result = data;
		    		receive_from = world_rank ^ bitmask;

		    		//Account for a process that doesn't need to do anything
		    		//during a send
		    		if (receive_from < world_size)
		    		{
		    			// printf("Process %d receives from %d \n", world_rank, receive_from);
			    		MPI_Recv(&data, 1, MPI_DOUBLE, receive_from, tag, MPI_COMM_WORLD,
			    			MPI_STATUS_IGNORE);
			    		// Sum data
			    		data = local_result + data;
		    		}
	    		}
	    	}
    	}
    	/////////////Low to high//////////////////////
    	else if (type == 0)
    	{
    		// Check which processes should be sending
    		if ((world_rank & bitmask) == bitmask)
	    	{
	    		if (delivered == 0)
	    		{
	    			deliver_to = world_rank ^ bitmask;
		    		// printf("Process %d delivers to %d on iteration %d \n", 
		    		// 	world_rank, deliver_to, i);
		    		MPI_Send(&data, 1, MPI_DOUBLE, deliver_to, tag, MPI_COMM_WORLD);
		    		delivered = 1;
	    		}
	    	}
	    	else
	    	{
	    		//Check which processes should be receiving
	    		if ((world_rank % (int)pow(2,(i+1))) == 0)
	    		{
	    			double local_result;
		    		local_result = data;
		    		receive_from = world_rank ^ bitmask;

		    		//Account for a process that doesn't need to do anything
		    		//during a send
		    		if (receive_from < world_size)
		    		{
		    			// printf("Process %d receives from %d on iteration %d \n", 
		    			// 	world_rank, receive_from, i);
			    		MPI_Recv(&data, 1, MPI_DOUBLE, receive_from, tag, MPI_COMM_WORLD,
			    			MPI_STATUS_IGNORE);
			    		// Sum data
			    		data = local_result + data;
		    		}
	    		}
	    	}
    	}
    	

    	//Debug
    	// if (world_rank == 0)
    	// {
    	// 	printf("Type = %d, Mask = %d, iteration = %d \n", type, bitmask, i);
    	// }

    	//Update bitmask
    	if (type == 1)
    	{
    		bitmask = bitmask / 2;
    	}
    	else
    	{
    		bitmask = bitmask * 2;
    	}
    }
    
    if (world_rank == 0)
    {
    	printf("Final Result is %f \n", data);
    }


    MPI_Finalize();
}