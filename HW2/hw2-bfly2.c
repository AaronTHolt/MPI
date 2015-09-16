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
run with $mpiexec -np NP ./a.out -type SIZE
where NP is process number
-type is -h or -l for high to low and low to high
SIZE is buffer size
*/


int main(int argc, char *argv[]) 
{

	//n is nearest power of 2 processes
	//if 3 processes are used, n will be 4.
	int n; 
	//number of iterations needed to reduce
	int iterations;
	//Size of buffer
	int size;


	//High-Low or Low-High
	//1 = h->l
	//0 = l->h  default
	int type = 1;
	//counters
    int i, j, k, jj, kk;

	char option_h2l[] = "-h";
	char option_H2L[] = "-H";
	char option_l2h[] = "-l";
	char option_L2H[] = "-L"; 



	//Argument parsing. 
	for (j=1; j<argc; j++) 
	{
		//Get tag (operation type) value if present
		if (sscanf (argv[j], "%i", &size)!=1) {}

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

	// printf("Size = %d\n", size);

	//For now, hardcode tag (operation)
	int tag = 0; //tag = 0 => addition

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
    
    
    //first masks to be used with l-h and h-l
	int bitmask;

    //Find out how many iterations are needed
    //Find bitmask
	n = pow(2, ceil(log(world_size)/log(2)));
	iterations = log(n)/log(2);

    //Timing
    double total_time;
    total_time = 0;
    double starttime, endtime;

    int deliver_to; 	//who to send to
    int receive_from;   //who to receive from

    //Dynamically allocate arrays
    double *buffer_1;	  //buffer to send
    double *buffer_local; //store local results
    buffer_1 = (double*) malloc(size*sizeof(double)+1);
    buffer_local = (double*) malloc(size*sizeof(double)+1);
    buffer_1[size] = '\0';
    buffer_local[size] = '\0';
    signed int *receive; 		//Which processes should send
    signed int *send;			//Which processes should receive
    send = malloc(n*sizeof(int)+1);
    receive = malloc(n*sizeof(int)+1);

    // printf("%f\n", (double)sizeof(buffer_1));

    // Account for iterations where 1 process needs to send twice
	signed int extra_send[2];
	extra_send[0] = -1;
	extra_send[1] = -1;

    //fill arrays 
    for (j=0; j<n; j++)
    {
    	send[j] = (int)0;
    	receive[j] = (int)0;
    	// printf("%d ", send[j]);
    	// printf("\n");
    }
    for (j=0; j<size; j++)
    {
    	buffer_1[j] = (double)1.0;
    	buffer_local[j] = (double)1.0;
    	// printf("%f ", buffer_1[j]);
    	// printf("\n");
    }

    //10 warmup + 10 timed
    for (kk=0; kk<20; kk++)
    {
    	//Starting bitmasks
	    if (type == 1)
	    {
	        bitmask = n/2;
	    }
	    else
	    {
	        bitmask = 1;
	    }

	    for (k=0; k<size; k++)
		{
			buffer_1[k] = 1.0;
		}
	    for (k=0; k<size; k++)
		{
			buffer_local[k] = 1.0;
		}


	    if (kk>=10)
	    {
	    	starttime = MPI_Wtime();
	    }
	    

	    //Run to 2*iterations to account for deadlock
	    //if everything sends/receives at once
	    //Alternate sending and receiving
	    for (i=0; i<2*iterations; i++)
	    {
	    	// if (world_rank == 0)
	    	// {
	    	// 	printf("i=%d, imod2=%d, bitmask=%d\n",i, i%2, bitmask);

	    	// }

			// printf("Here TOP, world rank %d, iteration %d!\n", world_rank, i);
	  //   	MPI_Barrier(MPI_COMM_WORLD);
			

	    	
	    	//zero arrays
	    	for (j=0; j<world_size; j++)
		    {
		    	send[j] = -1;
		    	receive[j] = -1;
		    }
		    extra_send[0] = -1;
		    extra_send[1] = -1;

	    	//Calculate who sends
	    	if (type == 1 || type == 0)
	    	{
	    		for (jj = 0; jj<world_size; jj++)
	    		{
	    			deliver_to = jj^bitmask;

	    			//On even count, only deliver if deliver_to>your_rank
	    			if ((deliver_to > jj) && (i%2 == 2 || i%2 == 0))
	    			{
	    				if (deliver_to<world_size)
	    				{
	    					send[jj] = deliver_to;
	    				}
	    			}
	    			

	    			//On odd count, only deliver if deliver_to<your_rank
	    			if ((deliver_to < jj) && (i%2 == 1) && (deliver_to>=0))
	    			{
	    				send[jj] = deliver_to;
	    			}
	    			else if (i%2 == 1)
	    			{
	    				send[jj] = -1;
	    			}

	    		}

	    		//Take care of odd process numbers (ie np=5...)
	    		//Only do these after 1st iteration, on even count
	    		if (i>1)
				{
					// printf("HERE, iteration %d\n", i);
					if (world_rank+2*bitmask>=world_size && 
						world_rank+bitmask<world_size &&
						(i%2 == 2 || i%2 == 0))
					{
						// printf("HERE22, iteration %d\n", i);
						extra_send[0] = world_rank; //from
						extra_send[1] = world_rank + bitmask; //to
					}
				}

				int normal_deliver;
				normal_deliver = world_rank ^ bitmask;
				if (extra_send[1]==normal_deliver)
				{
					extra_send[0] = -1;
					extra_send[1] = -1;
				}

				if (i>1)
				{
					int normal_receive;
					normal_receive = world_rank ^ bitmask;
					if (normal_receive>=world_size)
					{
						normal_receive = world_rank - bitmask;
						extra_send[0] = normal_receive;
						extra_send[1] = world_rank;
					}
				}
				
	    	}

	    	//Debug
	  //   	if (world_rank == 0)
			// {
			// 	for (k = 0; k<world_size; k++)
			// 	{
			// 		printf("%d ", send[k]);
			// 	}
			// 	printf(" iteration %d \n", i);
			// }
			// printf("Extra send from %d, to %d, iteration %d \n", 
			// 		extra_send[0], extra_send[1], i);

			// printf("Here BEFORE SEND, world rank %d, iteration %d!\n", world_rank, i);
			// MPI_Barrier(MPI_COMM_WORLD);
			

	    	//Send
	    	if (send[world_rank]>=0)
	    	{
	    		// printf("    Process %d delivers to %d on iteration %d \n", 
	      //                                   world_rank, send[world_rank], i);
	    		MPI_Send(buffer_1, size, MPI_DOUBLE, send[world_rank], tag, MPI_COMM_WORLD);
	    	}

	    	//receive
	    	for (j=0; j<world_size; j++)
	    	{
	    		if (send[j]==world_rank)
	    		{
	    		// 	printf("Process %d receives from %d on iteration %d \n", 
	    		// 							world_rank, j, i);
	    			MPI_Recv(buffer_local, size, MPI_DOUBLE, j, tag, MPI_COMM_WORLD,
	                                            MPI_STATUS_IGNORE);
	    			for (k=0; k<size; k++)
	    			{
	    				buffer_local[k] = buffer_local[k] + buffer_1[k];
	    			}
	    		}
	    	}


	    	if (i%2 == 0 && type == 1)
	    	{
	    		if (extra_send[0]==world_rank)
				{
					
					// printf("    Process %d delivers to %d on iteration %d \n", 
	    //                                     world_rank, extra_send[1], i);
					MPI_Send(buffer_1, size, MPI_DOUBLE, extra_send[1], tag, MPI_COMM_WORLD);
					
				}

		    	if (extra_send[1]==world_rank)
				{
					
					// printf("Process %d receives from %d on iteration %d \n", 
	    // 									world_rank, j, i);
					MPI_Recv(buffer_local, size, MPI_DOUBLE, extra_send[0], tag, MPI_COMM_WORLD,
	                                        MPI_STATUS_IGNORE);
					for (k=0; k<size; k++)
	    			{
	    				buffer_local[k] = buffer_local[k] + buffer_1[k];
	    			}
					
				}
	    	}

	  //   	printf("Here AFTER SEND, world rank %d, iteration %d!\n", world_rank, i);
			// MPI_Barrier(MPI_COMM_WORLD);
	    	

	    	//Update bitmask, buffer_local on evens
	    	if (i%2 == 1)
	    	{
	    		if (type == 1)
		        {
		            bitmask = bitmask / 2;
		        }
		        else
		        {
		            bitmask = bitmask * 2;
		        }

		        for (k=0; k<size; k++)
	    		{
	    			buffer_1[k] = buffer_local[k];
	    		}
	    		// MPI_Barrier(MPI_COMM_WORLD);
	    		// printf("Process %d has data %f on iteration %d \n", 
	    		// 	world_rank, buffer_local[0], i);
	    	}
	    }
		if (kk>=10)
	    {
	    	endtime = MPI_Wtime();
	    	total_time = total_time + endtime - starttime;
		}
    }

    MPI_Barrier(MPI_COMM_WORLD);
    if (world_rank == 0)
    {
    	printf("P0 has result %f\n", buffer_local[0]);
    	printf("Average Time = %f\n", total_time/10);
    }
    MPI_Barrier(MPI_COMM_WORLD);

    

 //    MPI_Barrier(MPI_COMM_WORLD);
 //    // printf("Process %d has data %f on iteration %d \n", 
 //    // 			world_rank, buffer_local[0], i);
 //    if (world_rank == 0)
 //    {
 //    	printf("P0 ARRAY : ");
 //    	for (i=0; i<size; i++)
	//     {
	//     	printf("%f\n", buffer_local[i]);
	//     }
 //    }
    
	// MPI_Barrier(MPI_COMM_WORLD);



    //Match malloc with free
    free (buffer_1);
    free (buffer_local);
    free (send);
    free (receive);

    return 0;

    MPI_Finalize();
}

