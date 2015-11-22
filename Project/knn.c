#include "stdlib.h"
#include "argp.h"
#include "mpi.h"
#include "stdio.h"
#include "math.h"
#include "string.h"
#include "unistd.h"


struct data{
	int ft_num;
	char *question;
	char *cat;
};

struct data make_example(char csv_line[], struct data instance);
void print_data(struct data *instance);

const char *argp_program_version =
    "argp-ex3 1.0";
const char *argp_program_bug_address =
    "<bug-gnu-utils@gnu.org>";

/* Program documentation. */
static char doc[] =
    "A program with options and arguments using argp";

/* A description of the arguments we accept. */
static char args_doc[] = "0=Serial,1=Parallel  Clusters";

/* The options we understand. */
static struct argp_option options[] = {
    {"verbose",  'v', 0,      0,  "Produce verbose output" },
    { 0 }
};

/* Used by main to communicate with parse_opt. */
struct arguments
{
    char *args[4];                
    int verbose;
    int animation;
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
            if (state->arg_num >= 3)
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

int main (int argc, char **argv)
{
    struct arguments arguments;

    /* Parse our arguments; every option seen by parse_opt will
       be reflected in arguments. */
    argp_parse (&argp, argc, argv, 0, 0, &arguments); 

    int run_type;
    run_type = 0; //default is serial
    if (sscanf (arguments.args[0], "%i", &run_type)!=1) {}

    int clusters;
    clusters = 0; //default is serial
    if (sscanf (arguments.args[1], "%i", &clusters)!=1) {}

    int percent_data;
	percent_data = 100; //default is 100% of data
	if (sscanf(arguments.args[2], "%i", &percent_data)!=1) {}

    //verbose?
    int verbose;
    verbose = arguments.verbose;

    printf("Runtype, Clusters, Verbose, PercentDataUsed = %i, %i, %i, %i\n",
                          run_type, clusters, verbose, percent_data);

    //define a bunch of counters!
    int i, j, k, m, n, ii, jj, kk;

    //number of examples to read in
    int total_examples = 3;

    //Allocate space for data
    char *csv_line = malloc(sizeof(char)*1500);
    struct data *all_data;
    all_data = malloc(sizeof(struct data)*total_examples);



    //Read in csv file
    FILE *f = fopen("train2.csv", "r");
    if (f == NULL){
    	printf("Failed to open file \n");
    	return -1;
    }



    for (i=0; i<total_examples; i++){

\
    	//line in csv to buffer
    	fgets(csv_line, 1500, f);
    	if (verbose>0){
    		printf("%s\n", csv_line);
    	}

    	// printf("Here2, iteration i=%i\n", i);
    	//csv line to 3 individual parts
    	if (i>0)
    	{
    		struct data instance;
    		all_data[i-1] = make_example(csv_line, instance);
    	}
    }


    // struct data *instance1;
    // instance1 = make_example(demo_csv_line);
    // // print_data(instance1);
    // printf("\n");



}

struct data make_example(char csv_line[], struct data instance){
	

	char *tok;
	const char *errstr;

	// printf("CSV_LINE = %s\n", csv_line);

	tok = strtok(csv_line, "|");
	sscanf(tok, "%i", &instance.ft_num);

	tok = strtok(NULL, "|");
	instance.question = tok;

	tok = strtok(NULL, "|");
	instance.cat = tok;

	print_data(&instance);

	return instance;
}

void print_data(struct data *instance){
	printf("ft_num = %i\n", instance->ft_num);
	printf("Question = %s\n", instance->question);
	printf("Category = %s\n", instance->cat);
}


