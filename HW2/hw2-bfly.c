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
	//0 = l->h  default
	int type = 1;
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

    data = 1.0;
    
    //first masks to be used with l-h and h-l
	int bitmask;

    //Find out how many iterations are needed
    //Find bitmask
	n = pow(2, ceil(log(world_size)/log(2)));
	iterations = log(n)/log(2);
	//find bitmask


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
        if (type == 1)
        {
            bitmask = n/2;
        }
        else
        {
            bitmask = 1;
        }
            // Variables
        int i; //counter
        int deliver_to; //who to send to
        int receive_from;   //who to receive from


        for (i = 0; i<iterations; i++)
        {
            /////////////////// High to Low//////////////////////
            if (type == 1)
            {
                // Check which processes should be sending
                deliver_to = world_rank ^ bitmask;
                // deliver_to = world_rank + pow(2,i);
                if (deliver_to < world_size)
                {
                    // printf("Process %d delivers to %d on iteration %d \n", 
        //                 world_rank, deliver_to, i);
                    MPI_Send(&data, 1, MPI_DOUBLE, deliver_to, tag, MPI_COMM_WORLD);
                }

                //Deal with odd process numbers
                if ((i > 0) && ((world_rank + 2*bitmask)>=world_size))
                {
                    deliver_to = world_rank + bitmask;
                    if (deliver_to < world_size)
                    {
                        // printf("Process %d delivers to %d on iteration %d \n", 
                        //     world_rank, deliver_to, i);
                        MPI_Send(&data, 1, MPI_DOUBLE, deliver_to, tag, MPI_COMM_WORLD);
                    }
                }

                //Check which processes should be receiving
                receive_from = world_rank ^ bitmask;
                if (receive_from < world_size)
                {
                    double local_result;
                    local_result = data;
                    // receive_from = world_rank - pow(2,i);
                    // printf("Process %d receives from %d \n", world_rank, receive_from);
                    MPI_Recv(&data, 1, MPI_DOUBLE, receive_from, tag, MPI_COMM_WORLD,
                        MPI_STATUS_IGNORE);
                    data = local_result + data;
                }
                //Account for odd process numbers
                else if (receive_from >= world_size)
                {
                    if (i > 0)
                    {
                        receive_from = world_rank - bitmask;
                        if (receive_from < world_size)
                        {
                            double local_result;
                            local_result = data;
                            // receive_from = world_rank - pow(2,i);
                            // printf("Process %d receives from %d \n", world_rank, receive_from);
                            MPI_Recv(&data, 1, MPI_DOUBLE, receive_from, tag, MPI_COMM_WORLD,
                                MPI_STATUS_IGNORE);
                            data = local_result + data;
                        }
                    }
                }               
            }
            /////////////Low to high//////////////////////
            else if (type == 0)
            {
                // Check which processes should be sending
                deliver_to = world_rank ^ bitmask;
                // deliver_to = world_rank + pow(2,i);
                if (deliver_to < world_size)
                {
                    // printf("Process %d delivers to %d on iteration %d \n", 
                    //     world_rank, deliver_to, i);
                    MPI_Send(&data, 1, MPI_DOUBLE, deliver_to, tag, MPI_COMM_WORLD);
                }

                //Deal with odd process numbers
                if ((i > 0) && ((world_rank + 2*bitmask)>=world_size))
                {
                    deliver_to = world_rank + bitmask;
                    if (deliver_to < world_size)
                    {
                        // printf("Process %d delivers to %d on iteration %d \n", 
                        //     world_rank, deliver_to, i);
                        MPI_Send(&data, 1, MPI_DOUBLE, deliver_to, tag, MPI_COMM_WORLD);
                    }
                }

                //Check which processes should be receiving
                receive_from = world_rank ^ bitmask;
                if (receive_from < world_size)
                {
                    double local_result;
                    local_result = data;
                    // receive_from = world_rank - pow(2,i);
                    // printf("Process %d receives from %d \n", world_rank, receive_from);
                    MPI_Recv(&data, 1, MPI_DOUBLE, receive_from, tag, MPI_COMM_WORLD,
                        MPI_STATUS_IGNORE);
                    data = local_result + data;
                }
                //Account for odd process numbers
                else if (receive_from >= world_size)
                {
                    if (i > 0)
                    {
                        receive_from = world_rank - bitmask;
                        if (receive_from < world_size)
                        {
                            double local_result;
                            local_result = data;
                            // receive_from = world_rank - pow(2,i);
                            // printf("Process %d receives from %d \n", world_rank, receive_from);
                            MPI_Recv(&data, 1, MPI_DOUBLE, receive_from, tag, MPI_COMM_WORLD,
                                MPI_STATUS_IGNORE);
                            data = local_result + data;
                        }
                    }
                }    
            }


            //// Debug
            // if (world_rank == 0)
            // {
            //     printf("Type = %d, pow = %d, iteration = %d, n/2 = %d \n", 
            //         type, (int)pow(2, iterations-i-1), i, n/2);
            //     printf("Bitmask = %d\n", bitmask);
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
        

        // printf("Process %d has data = %f \n", world_rank, data);
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