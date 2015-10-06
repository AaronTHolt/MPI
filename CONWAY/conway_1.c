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
int count_neighbors(int i, int j, int* section, int msize,
            int* top, int* bot, int* left, int* right, 
            int topleft, int topright, int botleft, int botright)
{
    int total_around = 0;

    //top center//
    //on top edge?
    if (i == 0)
    {
        //alive?
        if (top[j] == 0)
        {
            total_around += 1;
        }
    }
    //in middle somewhere
    else if (section[(i-1)*msize + j] == 0)
    {
        total_around += 1;
    }

    //bottom center//
    //on bot edge?
    if (i == (msize-1))
    {
        if (bot[j] == 0)
        {
            total_around += 1;
        }
    }
    else if (section[(i+1)*msize + j] == 0)
    {
        total_around += 1;
    }

    //right//
    //on right edge?
    if(j == (msize-1))
    {
        if(right[i] == 0)
        {
            total_around += 1;
        }
    }
    else if (section[i*msize+j+1] == 0)
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
    else if (section[i*msize+j-1] == 0)
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
    else if (section[(i-1)*msize+j-1] == 0)
    {
        total_around += 1;
    }

    //topright//
    //topright corner?
    if (i==0 && j==msize-1)
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
    else if (j == msize-1)
    {
        if (right[i-1] == 0)
        {
            total_around += 1;
        }
    }
    //in center?
    else if (section[(i-1)*msize+j+1] == 0)
    {
        total_around += 1;
    }

    //botright//
    //botright corner?
    if (i==msize-1 && j==msize-1)
    {
        if (botright == 0)
        {
            total_around += 1;
        }
    }
    //on bot row?
    else if (i == msize-1)
    {
        if (bot[j+1] == 0)
        {
            total_around += 1;
        }
    }
    //on right edge?
    else if (j == msize-1)
    {
        if (right[i+1] == 0)
        {
            total_around += 1;
        }
    }
    //in center?
    else if (section[(i+1)*msize+j+1] == 0)
    {
        total_around += 1;
    }

    //botleft//
    //botleft corner?
    if (i==msize-1 && j==0)
    {
        if (botleft == 0)
        {
            total_around += 1;
        }
    }
    //on bot row?
    else if (i == msize-1)
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
    else if (section[(i+1)*msize+j-1] == 0)
    {
        total_around += 1;
    }

    return total_around;
}

void print_matrix(int i_size, int* matrix)
{
	int i,j;
	for (i=0;i<i_size;i++)
    {
    	for (j=0;j<i_size;j++)
    	{
    		// printf("so %d\n", (int)sizeof(t_A));
    		printf("%6.2d ", matrix[i*i_size+j]);
    	}
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

    printf("Type,Iterations=%d,%d\n\n",run_type, iterations);

    //verbose?
    int verbose;
    verbose = arguments.verbose;

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

    // //Create new column derived datatype
    // MPI_Datatype column;
    // //count, blocklength, stride, oldtype, *newtype
    // MPI_Type_hvector(m, 1, m*sizeof(double), MPI_DOUBLE, &column);
    // MPI_Type_commit(&column);

    // //Create new row derived datatype
    // MPI_Datatype row;
    // //count, blocklength, stride, oldtype, *newtype
    // MPI_Type_hvector(m, 1, sizeof(double), MPI_DOUBLE, &row);
    // MPI_Type_commit(&row);

    

    // // Check constraints
    // if (run_type == 2)
    // {
    //     int q; //q^2=p
    //     q = (int)sqrt(world_size);
    //     if (m%q!=0 || q*q!=world_size)
    //     {
    //         if (world_rank == 0)
    //         {
    //             printf("Assume that P=q2, where q is an integer s.t. q evenly divides n.\n");
    //         }
    //         exit(0);
    //     }        
    // }
    

    // //row communicator
    // MPI_Comm my_row_comm;
    // int my_row;
    // my_row = world_rank/q;
    // MPI_Comm_split(MPI_COMM_WORLD, my_row, world_rank, &my_row_comm);

    // //column communicator
    // MPI_Comm my_col_comm;
    // int my_col;
    // my_col = world_rank%q;
    // MPI_Comm_split(MPI_COMM_WORLD, my_col, world_rank, &my_col_comm);

    // printf("WR=%d, Row=%d, Col=%d\n",world_rank,my_row,my_col);


    //Dynamically allocate arrays
    //Serial
    int msize; 
    int *section;
    int *neighbors;
    int *top;
    int *bot;
    int *left;
    int *right;
    int topleft,topright,botleft,botright;

    if (world_size == 1 && run_type == 0)
    {
        msize = 3; //for now hardcode matrix size
        
        section = (int*) malloc(msize*msize*sizeof(int));
        neighbors = (int*) malloc(msize*msize*sizeof(int));
        top = (int*) malloc(msize*sizeof(int));
        bot = (int*) malloc(msize*sizeof(int));
        left = (int*) malloc(msize*sizeof(int));
        right = (int*) malloc(msize*sizeof(int));
        
        topleft = 255;
        topright = 255;
        botleft = 255;
        botright = 255;
    }
    //Blocked
    //Checkerboard

    int i,j,k;
    for (i=0;i<msize;i++)
    {
        for (j=0;j<msize;j++)
        {
            section[i*msize + j] = 255;
        }
        top[i] = 255;
        bot[i] = 255;
        right[i] = 255;
        left[i] = 255;
    }

    //blinker
    section[3] = 0;
    section[4] = 0;
    section[5] = 0;

    printf("STARTING\n");
    print_matrix(msize, section);
    printf("\n");

    //Gameplay
    for (k=0;k<iterations;k++)
    {
        //count neighbor
        for (i=0;i<msize;i++)
        {
            for (j=0; j<msize; j++)
            {
                neighbors[i*msize+j] = count_neighbors(i, j, section, msize,
                         top, bot, left, right,
                         topleft, topright, botleft, botright);
            }
        }

        //update cells
        for (i=0;i<msize;i++)
        {
            for (j=0; j<msize; j++)
            {
                //cell currently alive
                if (section[i*msize+j] == 0)
                {
                    //2 or 3 neighbors lives, else die
                    if (neighbors[i*msize+j] < 2 || 
                        neighbors[i*msize+j] > 3)
                    {
                        section[i*msize+j] = 255;
                    }
                }
                else
                {
                    //Exactly 3 neighbors spawns new life
                    if (neighbors[i*msize+j] == 3)
                    {
                        section[i*msize+j] = 0;
                    }
                }
            }
        }
        printf("Neighbors\n");
        print_matrix(msize, neighbors);
        printf("Update\n");
        print_matrix(msize, section);
        printf("\n");
    }


    //free malloc stuff
    free(section);
    free(neighbors);
    free(top);
    free(bot);
    free(left);
    free(right);

    MPI_Finalize();
    exit (0);
}    