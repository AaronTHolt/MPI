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

    int a, i, j, k;
    double matrix_A[m][m];
    double matrix_B[m][m];
    double matrix_C[m][m];
    // double matrix_A_t[m][m]; //recv column as row
    double col[m];
    double row[m];

    //Create new column derived datatype for all_gather send
    MPI_Datatype column;
    //count, blocklength, stride, oldtype, *newtype
    MPI_Type_hvector(m, 1, m*sizeof(double), MPI_DOUBLE, &column);
    MPI_Type_commit(&column);

    // //Create new row derived datatype for all_gather recv
    // MPI_Datatype row;
    // //count, blocklength, stride, oldtype, *newtype
    // MPI_Type_hvector(m, 1, sizeof(double), MPI_DOUBLE, &row);
    // MPI_Type_commit(&row);

    if (world_rank == 0)
    {
        printf("Initial Matrices A and B: \n");
    }
    
    for(i=0;i<m;i++)
    {
        for(j=0;j<m;j++)
        {
            matrix_A[i][j] = j;
            matrix_B[i][j] = j;
            matrix_C[i][j] = 0.0;
            // matrix_A_t[i][j]=0.0;
            col[j]=0;
            row[j]=0;
            if (world_rank == 0)
            {
                printf("%6.2f ", matrix_A[i][j]);
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
        //fill columns manually because mpi derived types don't seem to work...
        for(a=0;a<m;a++)
        {
            col[a] = matrix_A[a][i];
            printf("FUCK");
            if (world_rank == 0)
            {
                printf("%6.2f ", col[a]);
            }
        }
        MPI_Barrier(MPI_COMM_WORLD);
        //*send_buf,send_count,send_type,*recv_buf,recv_count,recv_type,comm
        MPI_Allgather(&col[0],m,MPI_DOUBLE,
            &row[0],m,MPI_DOUBLE,MPI_COMM_WORLD);
        // MPI_Allgather(&matrix_A[0][i],1,column,
        //     &matrix_A_t[m][0],m,MPI_DOUBLE,MPI_COMM_WORLD);

        // if(i==1 && world_rank==0)
        // {
        //     printf("\nROW then COL 1 is f'd\n");
        //     printf("%6.2f, ",matrix_B[0][0]);
        //     printf("%6.2f, ",matrix_B[0][1]);
        //     printf("%6.2f, \n",matrix_B[0][2]);
        //     printf("%6.2f, ",row_recv[0]);
        //     printf("%6.2f, ",row_recv[1]);
        //     printf("%6.2f, \n\n",row_recv[2]);
        // }

        if (world_rank == 0)
        {
            printf("\nColumn %d = \n",i);
            for(k=0;k<m;k++)
            {
                printf("%6.2f->%6.2f ", matrix_A[k][i], row[k]);
            }
            printf("\n");
        }

        //for every row
        for(j=0;j<m;j++)
        {
            matrix_C[j][i] = 0.0;
            if (world_rank == 0)
            {
                printf("Row %d = \n", j);
            }
            //dot product
            // printf("%6.2f --- ", matrix_C[j][i]);
            for(k=0;k<m;k++)
            {   

                matrix_C[j][i] += row[k]*matrix_B[j][k];
                if (world_rank == 0)
                {
                    // printf("%6.2f*%6.2f=%6.2f,%6.2f ", row_recv[k],
                    //     matrix_B[j][k], row_recv[k]*matrix_B[j][k],matrix_C[j][i]);
                    // printf("%6.2f ", matrix_C[j][i]);
                }
            }
            if (world_rank == 0)
            {
                printf("\n");
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
                printf("%6.2f ", matrix_C[k][j]);
            }
            printf("\n");
        }
        
    }


    MPI_Finalize();
    exit (0);
}