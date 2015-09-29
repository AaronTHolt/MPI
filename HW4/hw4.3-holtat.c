#include "stdlib.h"
#include "argp.h"
#include "mpi.h"
#include "stdio.h"
#include "math.h"
#include "string.h"

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


int main (int argc, char **argv)
{
    struct arguments arguments;

    /* Parse our arguments; every option seen by parse_opt will
       be reflected in arguments. */
    argp_parse (&argp, argc, argv, 0, 0, &arguments); 

    //matrix size, mxm
    int m;
    m = 4;
    if (sscanf (arguments.args[0], "%i", &m)!=1) {}
    // printf("%d",m);

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

    //Create new column derived datatype
    MPI_Datatype column;
    //count, blocklength, stride, oldtype, *newtype
    MPI_Type_hvector(m, 1, m*sizeof(double), MPI_DOUBLE, &column);
    MPI_Type_commit(&column);

    //Create new row derived datatype
    MPI_Datatype row;
    //count, blocklength, stride, oldtype, *newtype
    MPI_Type_hvector(m, 1, sizeof(double), MPI_DOUBLE, &row);
    MPI_Type_commit(&row);

    int q; //q^2=p
    q = (int)sqrt(world_size);

    // Check constraints
    if (m%q!=0 || q*q!=world_size)
    {
    	if (world_rank == 0)
    	{
    		printf("Assume that P=q2, where q is an integer s.t. q evenly divides n.\n");
    	}
    	exit(0);
    }

    //row communicator
    MPI_Comm my_row_comm;
    int my_row;
    my_row = world_rank/q;
    MPI_Comm_split(MPI_COMM_WORLD, my_row, world_rank, &my_row_comm);

    //column communicator
    MPI_Comm my_col_comm;
    int my_col;
    my_col = world_rank%q;
    MPI_Comm_split(MPI_COMM_WORLD, my_col, world_rank, &my_col_comm);

    // printf("WR=%d, Row=%d, Col=%d\n",world_rank,my_row,my_col);

    //Allocate matrices
	//Individual Matrix Size = n/q x n/q
    int i_size = m/q;
    if (world_rank == 0)
    {
    	printf("i_size = %dx%d\n",i_size,i_size);
    }

    //Dynamically allocate matrix parts
    double *m_A;
    m_A = (double*) malloc(i_size*i_size*sizeof(double));
    double *m_B;
    m_B = (double*) malloc(i_size*i_size*sizeof(double));
    double *m_C;
    m_C = (double*) malloc(i_size*i_size*sizeof(double));

    //Fill matrix parts with data
    

    free(m_A);
    free(m_B);
    free(m_C);

    MPI_Finalize();
    exit (0);
}    