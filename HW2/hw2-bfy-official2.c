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

	    MPI_Allreduce(buffer_1, buffer_local, size, MPI_DOUBLE, MPI_SUM,
           MPI_COMM_WORLD);
	    

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

