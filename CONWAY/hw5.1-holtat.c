#include "stdlib.h"
#include "argp.h"
#include "mpi.h"
#include "stdio.h"
#include "math.h"
#include "string.h"
#include "unistd.h"

// Include global variables. Only this file needs the #define
#define __MAIN 
#include "globals.h"
#undef __MAIN

// User includes
#include "pprintf.h"
#include "pgm.h"

//Aaron Holt
//HPSC
//MPI Fox'
//Assume that P=q2, where q is an integer s.t. q evenly divides n.



const char *argp_program_version =
    "argp-ex3 1.0";
const char *argp_program_bug_address =
    "<bug-gnu-utils@gnu.org>";

/* Program documentation. */
static char doc[] =
    "A program with options and arguments using argp";

/* A description of the arguments we accept. */
static char args_doc[] = "0=Serial,1=Block,2=Checker Iterations";

/* The options we understand. */
static struct argp_option options[] = {
    {"verbose",  'v', 0,      0,  "Produce verbose output" },
    { 0 }
};

/* Used by main to communicate with parse_opt. */
struct arguments
{
    char *args[2];                
    int verbose;
};

/* Parse a single option. */
static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
    /* Get the input argument from argp_parse, which we
     know is a pointer to our arguments structure. */
    struct arguments *arguments = state->input;
    switch (key)
        {
        case 'v':
            arguments->verbose = 2;
            break;

        case ARGP_KEY_ARG:
            if (state->arg_num >= 2)
            /* Too many arguments. */
            argp_usage (state);
            arguments->args[state->arg_num] = arg;
            break;

        case ARGP_KEY_END:
            if (state->arg_num < 2)
            /* Not enough arguments. */
            argp_usage (state);
            break;

        default:
            return ARGP_ERR_UNKNOWN;
        }
    return 0;
}

/* Our argp parser. */
static struct argp argp = { options, parse_opt, args_doc, doc };

//Takes in current cell location, and all neighboring data
//outputs integer of alive neighbor cells
int count_neighbors(int info[9], int* section, 
            int* top, int* bot, int* left, int* right)
            // int topleft, int topright, int botleft, int botright)
{
    int i,j,rsize,csize,topleft,topright,botright,botleft;
    i = info[0];
    j = info[1];
    // wr = info[2];
    rsize = info[3];
    csize = info[4];
    topleft = info[5];
    topright = info[6];
    botleft = info[7];
    botright = info[8];
    
    int total_around = 0;
    // printf("wr=%d, i=%d,j=%d\n",wr,i,j);
    // printf("wr=%d, top[j]=%d\n",wr,top[j]);
    
    //top center//
    //on top edge?
    if (i == 0)
    {
        //alive?
        if (top[j] == 0)
        {
            total_around += 1;
        }
        // printf("HERE@\n");
    }
    //in middle somewhere
    else if (section[(i-1)*rsize + j] == 0)
    {
        total_around += 1;
    }
    

    //bottom center//
    //on bot edge?
    if (i == (csize-1))
    {
        if (bot[j] == 0)
        {
            total_around += 1;
        }
    }
    else if (section[(i+1)*rsize + j] == 0)
    {
        total_around += 1;
    }

    //right//
    //on right edge?
    if(j == (rsize-1))
    {
        if(right[i] == 0)
        {
            total_around += 1;
        }
    }
    else if (section[i*rsize+j+1] == 0)
    {
        total_around += 1;
    }

    //left//
    //on left edge?
    if(j == 0)
    {
        if(left[i] == 0)
        {
            total_around += 1;
        }
    }
    else if (section[i*rsize+j-1] == 0)
    {
        total_around += 1;
    }

    //topleft//
    //on topleft corner?
    if (i==0 && j==0)
    {
        if (topleft == 0)
        {
            total_around += 1;
        }
    }
    //on top row?
    else if (i == 0)
    {
        if (top[j-1] == 0)
        {
            total_around += 1;
        }
    }
    //on left edge?
    else if (j == 0)
    {
        if (left[i-1] == 0)
        {
            total_around += 1;
        }
    }
    //in center?
    else if (section[(i-1)*rsize+j-1] == 0)
    {
        total_around += 1;
    }

    //topright//
    //topright corner?
    if (i==0 && j==rsize-1)
    {
        if (topright == 0)
        {
            total_around += 1;
        }
    }
    //on top row?
    else if (i == 0)
    {
        if (top[j+1] == 0)
        {
            total_around += 1;
        }
    }
    //on right edge?
    else if (j == rsize-1)
    {
        if (right[i-1] == 0)
        {
            total_around += 1;
        }
    }
    //in center?
    else if (section[(i-1)*rsize+j+1] == 0)
    {
        total_around += 1;
    }

    //botright//
    //botright corner?
    if (i==csize-1 && j==rsize-1)
    {
        if (botright == 0)
        {
            total_around += 1;
        }
    }
    //on bot row?
    else if (i == csize-1)
    {
        if (bot[j+1] == 0)
        {
            total_around += 1;
        }
    }
    //on right edge?
    else if (j == rsize-1)
    {
        if (right[i+1] == 0)
        {
            total_around += 1;
        }
    }
    //in center?
    else if (section[(i+1)*rsize+j+1] == 0)
    {
        total_around += 1;
    }

    //botleft//
    //botleft corner?
    if (i==csize-1 && j==0)
    {
        if (botleft == 0)
        {
            total_around += 1;
        }
    }
    //on bot row?
    else if (i == csize-1)
    {
        if (bot[j-1] == 0)
        {
            total_around += 1;
        }
    }
    //on left edge?
    else if (j == 0)
    {
        if (left[i+1] == 0)
        {
            total_around += 1;
        }
    }
    //in center?
    else if (section[(i+1)*rsize+j-1] == 0)
    {
        total_around += 1;
    }

    return total_around;
}

int count_buggies(int rsize, int csize, int* matrix)
{
    int i,j,count;
    count = 0;
    for (i=0;i<csize;i++)
    {
        for (j=0;j<rsize;j++)
        {
            if (matrix[i*rsize+j]>0)
            {
                count += 1;
            }
        }
    }
    return count;
}

void print_matrix(int rsize, int csize, int* matrix)
{
	int i,j;
	for (i=0;i<csize;i++)
    {
    	for (j=0;j<rsize;j++)
    	{
    		// printf("so %d\n", (int)sizeof(t_A));
    		printf("%3d ", matrix[i*rsize+j]);
    	}
    	// printf("\nROW=%d\n",i);
        printf("\n");
	}
	// printf("\n");
}


int main (int argc, char **argv)
{
    struct arguments arguments;

    /* Parse our arguments; every option seen by parse_opt will
       be reflected in arguments. */
    argp_parse (&argp, argc, argv, 0, 0, &arguments); 

    int run_type;
    run_type = 0; //default is serial
    if (sscanf (arguments.args[0], "%i", &run_type)!=1) {}

    int iterations;
    iterations = 0; //default is serial
    if (sscanf (arguments.args[1], "%i", &iterations)!=1) {}

    //counters
    int i,j,k,x,y;

    //verbose?
    int verbose;
    verbose = arguments.verbose;
    // printf("VERBOSE = %i",verbose);

    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Get the name of the processor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    // if (rank == 0)
    // {
    //     printf("Type,Iterations=%d,%d\n",run_type, iterations);
    // }
    if (world_size>1 && run_type ==0)
    {
        printf("Runtype and processors count not consistant\n");
        exit(0);
    }
    if (world_size==1 && run_type>0)
    {
        printf("Runtype and processors count not consistant\n");
        exit(0);
    }

     //serial
    if (world_size == 1 && run_type == 0)
    {

        ncols=1;
        nrows=1;
    }
    //Blocked
    else if (world_size>1 && run_type == 1)
    {
        ncols = 1;
        nrows = world_size;
        my_col = 0;
        my_row = rank;
    }
    //Checker
    else if (world_size>1 && run_type == 2)
    {
        ncols = (int)sqrt(world_size);
        nrows = (int)sqrt(world_size);

        // my_col = 
        // my_row = 
    }

    
    //////////////////////READ IN INITIAL PGM////////////////////////////////
    if(!readpgm("life.pgm"))
    {
        printf("WR=%d,HERE2\n",rank);
        if( rank==0 )
        {
            pprintf( "An error occured while reading the pgm file\n" );
        }
        MPI_Finalize();
        return 1;
    }

    // Count the life forms. Note that we count from [1,1] - [height+1,width+1];
    // we need to ignore the ghost row!
    i = 0;
    for(y=1; y<local_height+1; y++ )
    {
        for(x=1; x<local_width+1; x++ )
        {
            if( field_a[ y * field_width + x ] )
            {
                i++;
            }
        }
    }
    // pprintf( "%i local buggies\n", i );

    int total;
    MPI_Allreduce( &i, &total, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD );
    if( rank==0 )
    {
        pprintf( "%i total buggies\n", total );
        // total = count_buggies(local_width+2,local_height+2,field_a);
        // printf("COUNT 2 = %d\n", total);
        // total = count_buggies(local_width,local_height,field_a);
        // printf("COUNT 3 = %d\n", total);
    }
    

    // //row communicator
    // MPI_Comm my_row_comm;
    // int my_row1;
    // my_row1 = rank/q;
    // MPI_Comm_split(MPI_COMM_WORLD, my_row, rank, &my_row_comm);

    // //column communicator
    // MPI_Comm my_col_comm;
    // int my_col1;
    // my_col1 = rank%q;
    // MPI_Comm_split(MPI_COMM_WORLD, my_col, rank, &my_col_comm);

    // printf("WR=%d, Row=%d, Col=%d\n",rank,my_row,my_col);

    //Row and column size per processor
    int rsize, csize; 
    rsize = local_width;
    csize = local_height;


    if (rank == 0 && verbose == 2)
    {
        printf("rsize,csize,NP = %d, %d, %d\n",rsize,csize,world_size);
    }
    

    // //serial
    // if (world_size == 1 && run_type == 0)
    // {

    // }
    // //Blocked
    // else if (world_size>1 && run_type == 1)
    // {

    //     if (csize%world_size>0 || world_size>csize)
    //     {
    //         if (rank == 0)
    //         {
    //             printf("Number of processors must evenly divide\n"
    //              "the number of rows\n");
    //         }
    //         exit(0);
    //     }
    // }
    // //Checker
    // else if (world_size>1 && run_type == 2)
    // {

    //     if (csize%world_size>0 || world_size>csize)
    //     {
    //         if (rank == 0)
    //         {
    //             printf("Number of processors must evenly divide\n"
    //              "the number of rows\n");
    //         }
    //         exit(0);
    //     }
    // }

    // printf("Myrow=%d,Mycol=%d\n",my_row,my_col);


    

    // if (rank == 0)
    // {
    //     print_matrix(900,225,field_a);
    // }
    

    MPI_Barrier(MPI_COMM_WORLD);


    //////////////////ALLOCATE ARRAYS, CREATE DATATYPES/////////////////////

    // //Create new column derived datatype
    // MPI_Datatype column;
    // //count, blocklength, stride, oldtype, *newtype
    // MPI_Type_hvector(csize, 1, rsize*sizeof(int), MPI_INT, &column);
    // MPI_Type_commit(&column);

    //Create new row derived datatype
    MPI_Datatype row;
    //count, blocklength, stride, oldtype, *newtype
    MPI_Type_hvector(rsize, 1, sizeof(int), MPI_INT, &row);
    MPI_Type_commit(&row);

    //allocate arrays and corner storage
    int *section;
    int *neighbors;
    //to use
    int *top;
    int *bot;
    int *left;
    int *right;
    //to send
    int *ttop;
    int *tbot;
    int *tleft;
    int *tright;
    section = (int*)malloc(rsize*csize*sizeof(int)+1);
    neighbors = (int*)malloc(rsize*csize*sizeof(int)+1);
    top = (int*)malloc(rsize*sizeof(int)+1);
    bot = (int*)malloc(rsize*sizeof(int)+1);
    left = (int*)malloc(csize*sizeof(int)+1);
    right = (int*)malloc(csize*sizeof(int)+1);
    ttop = (int*)malloc(rsize*sizeof(int)+1);
    tbot = (int*)malloc(rsize*sizeof(int)+1);
    tleft = (int*)malloc(csize*sizeof(int)+1);
    tright = (int*)malloc(csize*sizeof(int)+1);
    int topleft,topright,botleft,botright;
    
    
    topleft = 255;
    topright = 255;
    botleft = 255;
    botright = 255;

    
    // printf("Rsize,Lsize,Fsize=%i %i %i,Csize,Lsize,Fsize=%i %i %i\n",rsize,local_width,field_width,csize,local_height,field_height);

    //Serial
    int count = 0;
    if (world_size == 1 && run_type == 0)
    {
        for (i=0;i<csize;i++)
        {
            for (j=0;j<rsize;j++)
            {
                section[i*rsize + j] = 255;
                
                if (field_a[(i+1)*(2+rsize) + j + 1])
                {
                    section[i*rsize + j] = 0;
                    count += 1;
                }
                else
                {
                    section[i*rsize + j] = 255;
                }

                top[j] = 255;
                bot[j] = 255;
                ttop[j] = 255;
                tbot[j] = 255;
            }
            right[i] = 255;
            left[i] = 255;
            tright[i] = 255;
            tleft[i] = 255;
        }
        // printf("COUNT 4 = %d\n", count);
    }

    //Blocked
    else if (world_size > 1 && run_type == 1)
    {
        //initialize
        for (i=0;i<csize;i++)
        {
            for (j=0;j<rsize;j++)
            {
                section[i*rsize + j] = 255;
                
                if (field_a[(i+1)*(2+rsize) + j + 1])
                {
                    section[i*rsize + j] = 0;
                    count += 1;
                }
                else
                {
                    section[i*rsize + j] = 255;
                }

                top[j] = 255;
                bot[j] = 255;
                ttop[j] = 255;
                tbot[j] = 255;
            }
            right[i] = 255;
            left[i] = 255;
            tright[i] = 255;
            tleft[i] = 255;
        }

        // MPI_Allreduce( &count, &total, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD );
        // if (rank == 0)
        // {
        //     printf("COUNT 4 = %d\n", total);
        // }
        
    }

    ////////////CHECK IF EQUAl//////////////////////
    for (i=0;i<csize;i++)
    {
        for (j=0;j<rsize;j++)
        {

        }
    }
    
    
            // //*data,count,type,from,tag,comm,mpi_status
            // MPI_Recv(&matrix[i][0], m, MPI_DOUBLE, from, 0, MPI_COMM_WORLD, 
            //     MPI_STATUS_IGNORE);
            // //*data,count,type,to,tag,comm
            // MPI_Send(&matrix[0][i], 1, column, to, 0, MPI_COMM_WORLD);


    ///////// CUSTOM INPUITS //////////////////
    // if (world_size == 0 && run_type == 0)
    // {
    //     //blinker
    //     int offset = 0;
    //     section[rsize+offset] = 0;
    //     section[rsize+1+offset] = 0;
    //     section[rsize+2+offset] = 0;
        
    //     //block
    //     int offset2 = 5;
    //     section[offset2] = 0;
    //     section[offset2+1] = 0;
    //     section[rsize+offset2] = 0;
    //     section[rsize+offset2+1] = 0;
    // }
    // else if (world_size>0 && run_type == 1)
    // {
    //     //blinker 
    //     int offset = 0;
    //     if (rank == 2)
    //     {
    //         section[offset] = 0;
    //         section[offset+1] = 0;
    //         section[offset+2] = 0;
    //     }

    //     //block
    //     int offset2 = 5;
    //     if (rank == 1)
    //     {
    //         section[rsize*(csize-1)+offset2] = 0;
    //         section[rsize*(csize-1)+offset2+1] = 0;
    //     }
    //     if (rank == 2)
    //     {
    //         section[offset2] = 0;
    //         section[offset2+1] = 0;
    //     }
        
    // }
    

    

    // if (rank > 0){
    //     printf("STARTING\n");
    //     print_matrix(rsize, 1, top);
    //     print_matrix(rsize, csize, section);
    //     print_matrix(rsize, 1, bot);
    //     printf("\n");
    // }
    
    int send_to;
    int receive_from;
    int info[9];
    info[2] = rank;
    info[3] = rsize;
    info[4] = csize;
    info[5] = topleft;
    info[6] = topright;
    info[7] = botleft;
    info[8] = botright;

    int current_count;

    //Gameplay
    for (k=0;k<iterations;k++)
    {
        if (k%100==0)
        {
            if (verbose == 2)
            {
                current_count = rsize*csize-count_buggies(rsize,csize,section);
                MPI_Allreduce( &current_count, &total, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD );
                if (rank == 0)
                {
                    printf("Iteration=%5d,Count=%6d\n", k,total);
                }
            }
        }

        //BLOCKED
        if (run_type == 1)
        {
            //change bot (send top) to account for middle area
            //alternate to avoid locking
            send_to = rank - 1;
            receive_from = rank + 1;

            //figure out what to send
            //top and bottom
            for (i=0;i<rsize;i++)
            {
                ttop[i] = section[i];
                tbot[i] = section[rsize*(csize-1)+i];
            }
            // if (rank == 1)
            // {
            //     print_matrix(rsize,1,top);
            // }
            //left n right
            for (i=0;i<csize;i++)
            {
                tleft[i] = section[0 + rsize*i];
                tright[i] = section[rsize-1 + rsize*i];
            }

            if (rank%2==0)
            {
                if (send_to<world_size && send_to>=0)
                {
                    MPI_Send(ttop, 1, row, send_to, 0, MPI_COMM_WORLD);
                }
                if (receive_from<world_size && receive_from >= 0)
                {
                    MPI_Recv(bot, 1, row, receive_from, 0, MPI_COMM_WORLD,
                        MPI_STATUS_IGNORE);
                }
            }
            else if (rank%2==1)
            {

                if (receive_from<world_size && receive_from >= 0)
                {
                    MPI_Recv(bot, 1, row, receive_from, 0, MPI_COMM_WORLD,
                        MPI_STATUS_IGNORE);
                }
                if (send_to<world_size && send_to>=0)
                {
                    MPI_Send(ttop, 1, row, send_to, 0, MPI_COMM_WORLD);
                }
            }

            //change top to account for middle area
            //alternate to avoid locking
            send_to = rank + 1;
            receive_from = rank - 1;


            if (rank%2==0)
            {
                // printf("%d, %d, %d\n", rank, send_to, receive_from);
                if (send_to<world_size && send_to>=0)
                {
                    MPI_Send(tbot, 1, row, send_to, 0, MPI_COMM_WORLD);
                }
                
                if (receive_from<world_size && receive_from >= 0)
                {
                    MPI_Recv(top, 1, row, receive_from, 0, MPI_COMM_WORLD,
                        MPI_STATUS_IGNORE);
                }
            }
            else if (rank%2==1)
            {
                // printf("%d, %d, %d\n", rank, send_to, receive_from);
                if (receive_from<world_size && receive_from >= 0)
                {
                    //*data,count,type,from,tag,comm,mpi_status
                    MPI_Recv(top, 1, row, receive_from, 0, MPI_COMM_WORLD,
                        MPI_STATUS_IGNORE);
                }

                if (send_to<world_size && send_to>=0)
                {
                    //*data,count,type,to,tag,comm
                    MPI_Send(tbot, 1, row, send_to, 0, MPI_COMM_WORLD);
                }
            }
        }

 
        // if (rank == 1){
        //     print_matrix(rsize, 1, top);
        //     print_matrix(rsize, csize, section);
        //     print_matrix(rsize, 1, bot);
        //     printf("\n");
        // }
        // printf("wr=%d,iteration=%d,maxval=%d, 11\n", rank, k,(csize-1)*rsize-1+rsize);
        
        //count neighbor
        for (i=0;i<csize;i++)
        {
            for (j=0; j<rsize; j++)
            {
                info[0] = i;
                info[1] = j;
                neighbors[i*rsize+j] = count_neighbors(info, section, 
                                    top, bot, left, right);
            }
        }
        // printf("wr=%d,iteration=%d, 22\n", rank, k);
        //update cells
        current_count = 0;
        for (i=0;i<csize;i++)
        {
            for (j=0; j<rsize; j++)
            {
                //cell currently alive
                if (section[i*rsize+j] == 0)
                {
                    //2 or 3 neighbors lives, else die
                    if (neighbors[i*rsize+j] < 2 || 
                        neighbors[i*rsize+j] > 3)
                    {
                        section[i*rsize+j] = 255;
                    }
                }
                else
                {
                    //Exactly 3 neighbors spawns new life
                    if (neighbors[i*rsize+j] == 3)
                    {
                        section[i*rsize+j] = 0;
                    }
                }
            }
        }
        // printf("wr=%d,iteration=%d, 33\n", rank, k);
        

        // printf("Neighbors\n");
        // print_matrix(rsize, csize, neighbors);
        // printf("\n");
        // printf("Iteration %d\n", k);
        // print_matrix(rsize, csize, section);

        
        
    }

    // printf("HERE\n");
    MPI_Barrier(MPI_COMM_WORLD);
    sleep(0.5);
    //free malloc stuff
    if( field_a != NULL ) free( field_a );
    if( field_b != NULL ) free( field_b );
    free(section);
    free(neighbors);
    free(top);
    free(bot);
    free(left);
    free(right);
    // printf("HERE\n");

    MPI_Finalize();

    // printf("HERE\n");
    exit (0);
}    