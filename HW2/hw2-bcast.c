#include "mpi.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "string.h"
#include <time.h>

//Aaron Holt
//instructions
/*
Compile with $mpicc hw2-fly2.c -lm
run with $mpiexec -np NP ./a.out -type
where NP is process number
-type is -h or -l for high to low and low to high
*/



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

    double data;

    if (world_rank == 0)
    {
    	data = 12345.6789;
    }

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

	// if (world_rank == 0)
 //    {
	//     printf("n = %d, iterations = %d, mask = %d, type = %d \n", n, 
	//     	iterations, bitmask, type);
 //    }

    // Variables
    int i; //counter
    int deliver_to;	//who to send to
    int receive_from;	//who to receive from
    int received;

    if (world_rank == 0)
    {
    	received = 1; //In bcast, each process should receive only once
    }
    else
    {
    	received = 0;
    }
    

    for (i = 0; i<iterations; i++)
    {
    	/////////////////// High to Low//////////////////////
    	if (type == 1)
    	{
    		// Check which processes should be sending
    		//can only send if you've received
    		if (received == 1)
    		{
    			deliver_to = world_rank + pow(2,i);
    			if (deliver_to < world_size)
    			{
    				// printf("Process %d delivers to %d on iteration %d \n", 
	    			// 	world_rank, deliver_to, i);
	    			MPI_Send(&data, 1, MPI_DOUBLE, deliver_to, tag, MPI_COMM_WORLD);
    			}
    		}
    		else
	    	{
	    		//Check which processes should be receiving
	    		if (received == 0)
	    		{
	    			//Account for a processes that doesn't need to do anything
			    	//for current iteration
	    			if (world_rank < pow(2,i+1))
		    		{
			    		receive_from = world_rank - pow(2,i);
		    			// printf("Process %d receives from %d \n", world_rank, receive_from);
			    		MPI_Recv(&data, 1, MPI_DOUBLE, receive_from, tag, MPI_COMM_WORLD,
			    			MPI_STATUS_IGNORE);
			    		received = 1;
		    		}
	    		}
	    	}
    	}
    	/////////////Low to high//////////////////////
    	else if (type == 0)
    	{
    		// Check which processes should be sending
    		//can only send if you've received
    		if (received == 1)
    		{
    			deliver_to = world_rank + (int)pow(2, iterations-i-1);
    			if (deliver_to < world_size)
    			{
    				// printf("Process %d delivers to %d on iteration %d \n", 
	    			// world_rank, deliver_to, i);
	    			MPI_Send(&data, 1, MPI_DOUBLE, deliver_to, tag, MPI_COMM_WORLD);
    			}
    		}

    		//Check which processes should be receiving
    		else if (received == 0)
    		{
    			//Account for a processes that doesn't need to do anything
		    	//for current iteration
    			if (world_rank % (int)pow(2, iterations-i-1) == 0)
	    		{
	    			double local_result;
		    		receive_from = world_rank - (int)pow(2, iterations-i-1);

		    		if (receive_from>=0)
		    		{
		    			// printf("Process %d receives from %d on iteration %d. \n", 
	    				// world_rank, receive_from, i);
			    		MPI_Recv(&local_result, 1, MPI_DOUBLE, receive_from, tag, MPI_COMM_WORLD,
			    			MPI_STATUS_IGNORE);
			    		data = local_result;
			    		received = 1;

		    		}
	    		}
    		}
	  
    	}
    	

    	//Debug
    	// if (world_rank == 0)
    	// {
    	// 	printf("Type = %d, pow = %d, iteration = %d, n/2 = %d \n", 
    	// 		type, (int)pow(2, iterations-i-1), i, n/2);
    	// }

    }
    

    printf("Process %d has data = %f \n", world_rank, data);


    MPI_Finalize();
}