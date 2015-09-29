#include "stdlib.h"
#include "argp.h"
#include "mpi.h"
#include "stdio.h"
#include "math.h"
#include "string.h"
#include "unistd.h"

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




//local matrix multiply
void lmm(int m, double* m_A, double* m_B, double* m_C)
{
	int i,j,k;
	//For rows
    for(i=0;i<m;i++)
    {
        //for columns
        for(j=0;j<m;j++)
        {
            //dot product
            for(k=0;k<m;k++)
            {
                m_C[i*m+j] = m_C[i*m+j] + 
                    m_A[i*m+k]*m_B[k*m+j];
            }
        }
    }
}

void print_matrix(int i_size, double* matrix)
{
	int i,j;
	for (i=0;i<i_size;i++)
    {
    	for (j=0;j<i_size;j++)
    	{
    		// printf("so %d\n", (int)sizeof(t_A));
    		printf("%6.2f ", matrix[i*i_size+j]);
    	}
    	printf("\n");
	}
	printf("\n");
}


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

    //counters
    int i, j, k;

    //Allocate matrices
	//Individual Matrix Size = n/q x n/q
    int i_size = m/q;
    // if (world_rank == 0)
    // {
    // 	printf("i_size = %dx%d\n",i_size,i_size);
    // }

    // //Dynamically allocate matrix parts
    // double **m_A = malloc(sizeof(*m_A)*i_size);
    // double **m_B = malloc(sizeof(*m_B)*i_size);
    // double **m_C = malloc(sizeof(*m_C)*i_size);
    // double **t_A = malloc(sizeof(*t_A)*i_size);
    // double **t_B = malloc(sizeof(*t_B)*i_size);
    // if (m_A)
    // {
    // 	for(i=0;i<i_size;i++)
    // 	{
    // 		m_A[i] = malloc(sizeof(*m_A)*i_size);
    // 		m_B[i] = malloc(sizeof(*m_B)*i_size);
    // 		m_C[i] = malloc(sizeof(*m_C)*i_size);
    // 		t_A[i] = malloc(sizeof(*t_A)*i_size);
    // 		t_B[i] = malloc(sizeof(*t_B)*i_size);
    // 	}
    // }

    //Dynamically allocate arrays
    double *m_A;
    m_A = (double*) malloc(i_size*i_size*sizeof(double));
    double *m_B;
    m_B = (double*) malloc(i_size*i_size*sizeof(double));
    double *m_C;
    m_C = (double*) malloc(i_size*i_size*sizeof(double));
    double *t_A;
    t_A = (double*) malloc(i_size*i_size*sizeof(double));
    double *t_B;
    t_B = (double*) malloc(i_size*i_size*sizeof(double));

    //Find distance between blocks
    int stride = i_size*i_size*sizeof(double);

    //Fill matrix parts with data
    for (i=0;i<i_size;i++)
    {
    	for (j=0;j<i_size;j++)
    	{
    		m_A[i*i_size+j] = j;
    		m_B[i*i_size+j] = j;
    		m_C[i*i_size+j] = 0;
    		t_A[i*i_size+j] = 0;
    		t_B[i*i_size+j] = 0;
    	}
    }

    // //Debug: printing initial part of matrix
    // if (world_rank == 0)
    // {
    // 	printf("Initial matrix part for each process:\n");
    // 	print_matrix(i_size, m_A);
    // }

    int dest;
    dest = (my_row + q - 1) % q;

    int source;
    source = (my_row + 1) % q; 

    int stage;
    int k_bar;
    int bcast_root;
    MPI_Status status;


    for (stage=0;stage<q;stage++)
    {
    	k_bar = (my_row + stage) % q;
    	bcast_root = (my_row + stage) % q;

    	if (bcast_root == my_col)
    	{
    		//int MPI_Bcast( void *buffer, int count, MPI_Datatype datatype,  
            //  	int root, MPI_Comm comm )
    		MPI_Bcast(m_A, i_size*i_size, MPI_DOUBLE, bcast_root,
    					my_row_comm);

    		//local matrix multiply
    		lmm(i_size, m_A, m_B, m_C);
    	}
    	else
    	{
    		MPI_Bcast(t_A, i_size*i_size, MPI_DOUBLE, bcast_root,
    					my_row_comm);

    		// //debug printing matrices
    		// if (world_rank == 1)
    		// {	
    		// 	print_matrix(i_size, t_A);
    		// }

    		// if (world_rank == 1)
    		// {
    		// 	print_matrix(i_size, m_C);
    		// }

    		//local matrix multiply
    		lmm(i_size, t_A, m_B, m_C);

    		// if (world_rank == 1)
    		// {
    		// 	print_matrix(i_size, m_C);
    		// }
    	}

    	MPI_Sendrecv_replace(m_B, i_size*i_size, MPI_DOUBLE, dest, 0,
    					source, 0, my_col_comm, &status);

    }

    // MPI_Barrier(MPI_COMM_WORLD);
    // if (world_rank == 1)
    // {
    // 	printf("Final results for each process:\n");
    // }
    // MPI_Barrier(MPI_COMM_WORLD);
    // for (i=0;i<world_size;i++)
    // {
    // 	if (world_rank == i)
    // 	{
    // 		printf("WorldRank=%d, Row=%d, Col=%d\n",world_rank,my_row,my_col);
    // 		print_matrix(i_size, m_C);
    // 		// printf("\n");
    // 	}
    // 	MPI_Barrier(MPI_COMM_WORLD);
    // }
    

  //   /////////////VERIFY////////////////////
  //   //Only works on smaller matrices, seg faults around 600x600

  //   sleep(1);
  //   MPI_Barrier(MPI_COMM_WORLD);
  //   //verify with serial mult
  //   if (world_rank == 0)
  //   {
		// double matrix_A[m][m];
	 //    double matrix_B[m][m];
	 //    double matrix_C[m][m];

	 //    // printf("Initial Matrices A and B: \n");
	 //    for(i=0;i<i_size*q;i++)
	 //    {
	 //        for(j=0;j<i_size*q;j++)
	 //        {
	 //            matrix_A[i][j] = j%i_size;
	 //            matrix_B[i][j] = j%i_size;
	 //            matrix_C[i][j] = 0;
	 //            // printf("%d ", j%i_size);
	 //        }
	 //        // printf("\n");
	 //    }
	 //    // printf("\n");

	 //    //For rows
	 //    for(i=0;i<m;i++)
	 //    {
	 //        //for columns
	 //        for(j=0;j<m;j++)
	 //        {
	 //            //dot product
	 //            for(k=0;k<m;k++)
	 //            {
	 //                matrix_C[i][j] = matrix_C[i][j] + 
	 //                    matrix_A[i][k]*matrix_B[k][j];
	 //            }
	 //        }
	 //    }

	 //    // printf("Serial Results:\n");
	 //    // for(i=0;i<m;i++)
	 //    // {
	 //    //     for(j=0;j<m;j++)
	 //    //     {
	 //    //         printf("%6.2f ", matrix_C[i][j]);
	 //    //     }
	 //    //     printf("\n");
	 //    // }

	 //    // Test if it works for process 0.
	 //    // printf("Result Matrix C (on proc 0): \n");
	 //    int t = 1;
	 //    for(i=0;i<i_size;i++)
	 //    {
	 //        for(j=0;j<i_size;j++)
	 //        {
	 //        	if (matrix_C[i][j] != m_C[i*i_size+j])
	 //        	{
	 //        		printf("NOT EQUAL\n");
	 //        		int t = 0;
	 //        		break;
	 //        	}
	 //            // printf("%6.2f ", matrix_C[i][j]);
	 //        }
	 //        // printf("\n");
	 //    }
	 //    if (t == 1)
	 //    {
	 //    	printf("Proccess 0 has same data as serial result\n");
	 //    }
  //   }

    //////////////////////////////
    //// end verify //////////////




    free(m_A);
    free(m_B);
    free(m_C);
    free(t_A);
    free(t_B);


    MPI_Finalize();
    exit (0);
}    