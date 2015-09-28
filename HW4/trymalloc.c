#include <stdlib.h>
#include <argp.h>
#include "mpi.h"
#include "stdio.h"
#include "math.h"
#include "string.h"

//Aaron Holt
//HPSC
//MPI Allgather Matrix Multiply


const char *argp_program_version =
    "argp-ex3 1.0";
const char *argp_program_bug_address =
    "<bug-gnu-utils@gnu.org>";

/* Program documentation. */
static char doc[] =
    "Argp example #3 -- a program with options and arguments using argp";

/* A description of the arguments we accept. */
static char args_doc[] = "MatrixSize";

/* The options we understand. */
static struct argp_option options[] = {
    {"verbose",  'v', 0,      0,  "Produce verbose output" },
    { 0 }
};

/* Used by main to communicate with parse_opt. */
struct arguments
{
    char *args[1];                /* m x m */
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
            arguments->verbose = 1;
            break;

        case ARGP_KEY_ARG:
            if (state->arg_num >= 1)
            /* Too many arguments. */
            argp_usage (state);
            arguments->args[state->arg_num] = arg;
            break;

        case ARGP_KEY_END:
            if (state->arg_num < 1)
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

// void serial_matrix_mult()


int main (int argc, char **argv)
{
    struct arguments arguments;

    /* Parse our arguments; every option seen by parse_opt will
       be reflected in arguments. */
    argp_parse (&argp, argc, argv, 0, 0, &arguments); 

    //matrix size, mxm
    int m;
    m = 5;
    if (sscanf (arguments.args[0], "%i", &m)!=1) {}

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

    // //No point in having more processes than matrix size...
    // if (world_size>m)
    // {
    //     if (world_rank == 0)
    //     {
    //         printf("Number of processes greater than matrix size, exiting\n");
    //     }
    //     exit(0);
    // }

    //size of data each process will send in allgather
    int size;
    size = (int)ceil( ((float)m/(float)world_size) );
    if (world_rank == 0)
    {
        printf("Size = %d\n", size);
    }

    //counters
    int a, b, i, j, k;

    //Dynamically allocate arrays
    double *matrix_A;
    matrix_A = (double*) malloc(m*m*sizeof(double));
    double *matrix_B;
    matrix_B = (double*) malloc(m*m*sizeof(double));
    double *matrix_C;
    matrix_C = (double*) malloc(m*m*sizeof(double));
    // double *matrix_D;
    // matrix_D = (double*) malloc(m*m*sizeof(double)+1);
    double *col_chunk;
    col_chunk = (double*) malloc(size*sizeof(double));
    double *col;
    col = (double*) malloc(size*world_size*sizeof(double));

    if (world_rank == 0)
    {
        printf("Initial Matrices A and B: \n");
    }
    
    for(i=0;i<size;i++)
    {
        col[i] = -1;
    }

    for(i=0;i<m;i++)
    {
        for(j=0;j<m;j++)
        {
            matrix_A[i*m+j] = j;
            matrix_B[i*m+j] = j;
            matrix_C[i*m+j] = 0.0;
            // col[j]=0;
            if (world_rank == 0)
            {
                printf("%6.2f ", matrix_A[i*m+j]);
            }
            
        }
        if (world_rank == 0)
        {
            printf("\n");    
        }
        
    }


    //Loop m(column) times followed by dot products such that every process
    //has a copy of the matrix
    //Assume square matrix

    //for every column
    for(i=0;i<m;i++)
    {
        //figure out whos sending what
        for(a=0;a<m;a++)
        {
            //find which part of column process sends
            if (a==world_rank*size)
            {
                for (b=0;b<size;b++)
                {
                    if ((a+b)<m)
                    {
                        col_chunk[b] = matrix_A[(a+b)*m+i];
                        // printf("WR = %d, col_chunk = %6.2f\n", world_rank, col_chunk[b]);
                    }
                    else
                    {
                        col_chunk[b] = -42;
                        // printf("WR = %d, col_chunk = %6.2f\n", world_rank, col_chunk[b]);
                    }
                }
                
            }
            // if (world_rank == 0)
            // {
            //     printf("%6.2f ", col[a]);
            // }
        }

        // if (world_rank == 0)
        // {
        //     for(k=0;k<size*world_size;k++)
        //     {
        //         printf("%6.1f", col[k]);
        //     }
        //     printf("\n");
        // }

        // MPI_Barrier(MPI_COMM_WORLD);

        //*send_buf,send_count,send_type,*recv_buf,recv_count,recv_type,comm
        MPI_Allgather(col_chunk,size,MPI_DOUBLE,
            col,size,MPI_DOUBLE,MPI_COMM_WORLD);




        if (world_rank == 0)
        {
            printf("\nColumn %d = \n",i);
            for(k=0;k<m;k++)
            {
                printf("%6.2f ", col[k]);
            }
            printf("\n");
        }



        //for every row
        for(j=0;j<m;j++)
        {
            matrix_C[j*m+i] = 0.0;
        //     // if (world_rank == 0)
        //     // {
        //     //     printf("Row %d = \n", j);
        //     // }
            //dot product
            // printf("%6.2f --- ", matrix_C[j][i]);
            for(k=0;k<m;k++)
            {   

                matrix_C[j*m+i] += col[k]*matrix_B[j*m+k];
        //         if (world_rank == 0)
        //         {
        //             // printf("%6.2f*%6.2f=%6.2f,%6.2f ", row_recv[k],
        //             //     matrix_B[j][k], row_recv[k]*matrix_B[j][k],matrix_C[j][i]);
        //             // printf("%6.2f ", matrix_C[j][i]);
        //         }
        //     }
        //     // if (world_rank == 0)
        //     // {
        //     //     printf("\n");
        //     // }
            }
        }
    }

    if (world_rank == 0)
    {
        printf("\nFinal Result\n");
        for(k=0;k<m;k++)
        {
            for(j=0;j<m;j++)
            {
                printf("%6.2f ", matrix_C[k*m+j]);
            }
            printf("\n");
        }
        
    }


    MPI_Finalize();
    exit (0);
}