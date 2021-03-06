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
    // if (world_rank == 0)
    // {
    //     printf("Size = %d\n", size);
    // }

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

    //Timing variables
    double total_time;
    total_time = 0;
    double starttime, endtime;
    double alltime[50] = {0};

    int kk;
    for (kk=0; kk<60; kk++)
    {

        
    
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
                
            }
            
        }

        if (kk>=10)
        {
            starttime = MPI_Wtime();
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

            }


            //*send_buf,send_count,send_type,*recv_buf,recv_count,recv_type,comm
            MPI_Allgather(col_chunk,size,MPI_DOUBLE,
                col,size,MPI_DOUBLE,MPI_COMM_WORLD);


            //for every row
            for(j=0;j<m;j++)
            {
                matrix_C[j*m+i] = 0.0;
                //dot product
                for(k=0;k<m;k++)
                {   

                    matrix_C[j*m+i] += col[k]*matrix_B[j*m+k];
                }
            }
        }

        if (kk>=10)
        {
            endtime = MPI_Wtime();
            total_time = total_time + endtime - starttime;
            alltime[kk-10] = endtime - starttime;
        }
    }



    MPI_Barrier(MPI_COMM_WORLD);
    // printf("Data %d, world rank %d\n", buffer[0], world_rank);

    if (world_rank == 0)
    {
        int i;
        for(i=0; i<50; i++)
        {
            if (i < 49)
            {
                printf("%2.9f,", alltime[i]);
            }
            else
            {
                printf("%2.9f", alltime[i]);
            }
        }
        printf("\n%2.9f\n", total_time/50);
    }
    MPI_Barrier(MPI_COMM_WORLD);

    free(matrix_A);
    free(matrix_B);
    free(matrix_C);
    free(col);
    free(col_chunk);

    MPI_Finalize();
    exit (0);
}