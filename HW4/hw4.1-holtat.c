#include <stdlib.h>
#include <argp.h>
#include "stdio.h"
#include "math.h"
#include "mpi.h"
#include "string.h"

//Aaron Holt
//HPSC
//MPI Serial Matrix Multiply


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

    // printf("m x n = %d x %d\n", m, m);

    //Timing variables
    double total_time;
    total_time = 0;
    double starttime, endtime;
    double alltime[50] = {0};

    int i, j, k;
    double matrix_A[m][m];
    double matrix_B[m][m];
    double matrix_C[m][m];

    // printf("Initial Matrices A and B: \n");
    // for(i=0;i<m;i++)
    // {
    //     for(j=0;j<m;j++)
    //     {
    //         matrix_A[i][j] = j;
    //         matrix_B[i][j] = j;
    //         matrix_C[i][j] = 0;
    //         printf("%d ", j);
    //     }
    //     printf("\n");
    // }

     //Timing
    //10 warmup, 40 test
    int kk;
    for (kk=0; kk<60; kk++)
    {

        if (kk>=10)
        {
            starttime = MPI_Wtime();
        } 

        for(i=0;i<m;i++)
        {
            for(j=0;j<m;j++)
            {
                matrix_A[i][j] = j;
                matrix_B[i][j] = j;
                matrix_C[i][j] = 0;
            }
        }


        //For rows
        for(i=0;i<m;i++)
        {
            //for columns
            for(j=0;j<m;j++)
            {
                //dot product
                for(k=0;k<m;k++)
                {
                    matrix_C[i][j] = matrix_C[i][j] + 
                        matrix_A[i][k]*matrix_B[k][j];
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

    // printf("Result Matrix C: \n");
    // for(i=0;i<m;i++)
    // {
    //     for(j=0;j<m;j++)
    //     {

    //         printf("%6.2f ", matrix_C[i][j]);
    //     }
    //     printf("\n");
    // }     



    exit (0);
}