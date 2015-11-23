#include "stdlib.h"
#include "argp.h"
#include "mpi.h"
#include "stdio.h"
#include "math.h"
#include "string.h"
#include "unistd.h"


struct data{
	int example_num;
	char **question;
	char *cat;
};

// struct numeric_data{
// 	int example_num;
// 	unsigned char feature_vec[150];
// 	char *cat;
// };

// struct data *data_create(int num, char **q, char *c){
// 	struct data *instance = malloc(sizeof(struct data));

// 	instance->example_num = num;
// 	instance->question = q;
// 	instance->cat = c;

// 	return instance;

// }

// struct hashed_q{
// 	int 
// };

struct data make_example(char csv_line[]);
void print_data(struct data *instance);
char **parse_question(char *question, int num_words);
unsigned long str_hash(unsigned char *str);
void add_to_word_list(char **question, char **word_list, int *cur_index);

const char *argp_program_version =
    "argp-ex3 1.0";
const char *argp_program_bug_address =
    "<bug-gnu-utils@gnu.org>";

/* Program documentation. */
static char doc[] =
    "A program with options and arguments using argp";

/* A description of the arguments we accept. */
static char args_doc[] = "0=Serial,1=Parallel NumClusters DataPercent";

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
    int total_examples = 4;

    //Allocate space for data
    char *csv_line = malloc(sizeof(char)*1500);

    // //store all data
    // struct data *all_data;
    // all_data = malloc(sizeof(struct data)*total_examples*1000);
    // for (ii=0; ii<total_examples; ii++){
    // 	all_data[ii].question = malloc(sizeof(char*)*200);
    // 	for (jj=0; jj<200; jj++){
    // 		all_data[ii].question[jj] = malloc(sizeof(char)*20);
    // 	}
    // }

    //store vocabulary list
    char **word_list;
    word_list = malloc(sizeof(char*)*10000); //assumes 10000 total vocab
    for (ii=0; ii<10000; ii++){
    	word_list[ii] = malloc(sizeof(char)*20);  //assumes max word length of 20
    }


    //Read in csv file
    FILE *f = fopen("train2.csv", "r");
    if (f == NULL){
    	printf("Failed to open file \n");
    	return -1;
    }


    //parse question into individual words, create vocabulary list
    int vocab_count = 0;
    for (i=0; i<total_examples; i++){
    	struct data instance;
    	//line in csv to buffer
    	fgets(csv_line, 1500, f);
    	
    	// if (verbose>0){
    	// 	printf("%s\n", csv_line);
    	// }

    	//csv line to 3 individual parts
    	if (i>0)
    	{
    		// instance.example_num = NULL;
    		// instance.question = NULL;
    		// instance.cat = NULL;
    		instance = make_example(csv_line);
    		// all_data[i-1] = instance;
    		// print_data(&all_data[i-1]);

    		//add to vocabulary
    		add_to_word_list(instance.question, word_list, &vocab_count);

    		//
    		
    	}
    }



    //close file
    fclose(f);


    //free malloc calls
    free(word_list);


}


void add_to_word_list(char **question, char **word_list, int *cur_index){
	int num_words = 200;
	int ii, jj, add_word;

	// printf("Cur_index = %i\n", *cur_index);

	for (ii=0; ii<num_words; ii++){
		if (question[ii] == NULL){
			break;
		}

		if (strlen(question[ii])<4){
			continue;
		}

		add_word = 0;
		for (jj=0; jj<*cur_index; jj++){
			if (!strcmp(question[ii], word_list[jj])){
				add_word = 1;
				break;
			}
		}

		if (add_word == 0){
			word_list[*cur_index] = question[ii];
			// printf("%s ", question[ii]);
			(*cur_index)++;
		}
	}
}

struct data make_example(char csv_line[]){
	
	struct data instance;

	int num_words = 200;

	char *tok;
	char *tok_copy; //problem with tok getting overwritten in parse_question
	char **parsed_question = malloc(sizeof(char*)*num_words);
	int i;
	for (i=0; i<num_words; i++){
		parsed_question[i] = malloc(sizeof(char)*20);
	}

	// printf("CSV_LINE = %s\n", csv_line);

	tok = strtok(csv_line, "|");
	sscanf(tok, "%i", &instance.example_num);

	tok = strtok(NULL, "|");
	tok_copy = (char *)tok;

	tok = strtok(NULL, "|");
	instance.cat = tok;

	// printf("tok = %s", tok);
	parsed_question = parse_question(tok_copy, num_words);
	instance.question = parsed_question;

	// for (i=0; i<10; i++){
	// 	printf("%s ", instance.question[i]);
	// }
	// printf("\n");

	// print_data(&instance);

	return instance;
}


void print_data(struct data *instance){
	printf("example_num = %i\n", instance->example_num);
	int p = 0;
	printf("Question = ");
	for (p=0; p<10; p++){
		printf("%s ", (instance->question)[p]);
	}
	printf("\n");
	printf("Category = %s\n", instance->cat);
}

char **parse_question(char *question, int num_words){

	char **parsed;
	parsed = malloc(sizeof(char*)*num_words);
	int i;
	for (i=0; i<num_words; i++){
		parsed[i] = malloc(sizeof(char)*20);
	}
	char *tok2;

	tok2 = strtok(question, " \t");
	parsed[0] = tok2;

	i = 1;
	while (tok2 != NULL){
		tok2 = strtok(NULL, " \t");
		// printf("%s\n", parsed[i-1]);
		parsed[i] = tok2;
		i++;
	}

	//terminate
	parsed[i] = NULL;

	return parsed;

}




// //djb2 hash function from http://www.cse.yorku.ca/~oz/hash.html
// //takes a string and returns an unsigned long int
// unsigned long str_hash(unsigned char *str){

// 	unsigned long hash = 5381;
// 	int c;

// 	while (c = *str++){
// 		hash = ((hash<<5) + hash) + c;
// 	}

// 	return hash;
// }