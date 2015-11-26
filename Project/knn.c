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

struct numeric_data{
	int example_num;
	unsigned char *feature_vec;
	char *cat;
};

typedef struct feature_tree{
	unsigned int feature_num;
	char *feature;
	struct feature_tree *left;
	struct feature_tree *right;
}feature_tree;

void insert_word(feature_tree **root, char *word){
	if (word == NULL){
		return;
	}

	if (strlen(word)<4){
		return;
	}

	if (!(*root)){
		feature_tree *temp = NULL;
		temp = (feature_tree *)malloc(sizeof(feature_tree));
		temp->feature_num = -1;
		temp->feature = malloc(sizeof(char)*20);
		strcpy(temp->feature, word);
		temp->left = NULL;
		temp->right = NULL;
		*root = temp;
		// printf("root feat = %s\n", root.feature);
		return;
	}

	// printf("%s, %s\n", root->feature, word);
	
	//less than goes left
	if (strcmp(word, (*root)->feature) < 0){
		insert_word(&(*root)->left, word);
	}
	//greater than goes right
	else if (strcmp(word, (*root)->feature) > 0){
		insert_word(&(*root)->right, word);
	}
}

void free_feature_tree(feature_tree *root){
	if (root){
		free_feature_tree(root->left);
		free_feature_tree(root->right);
		free(root->feature);
		free(root);
	}
}

int count_features(feature_tree *root){
	int count = 0;
	if (root == NULL){
		return 0;
	}
	count += 1 + count_features(root->left) + count_features(root->right);
	return count;
}

void print_inorder(feature_tree *root){
	if (root){
		print_inorder(root->left);
		printf("%s %u\n", root->feature, root->feature_num);
		print_inorder(root->right);
	}
}

void number_features(feature_tree *root, unsigned int *cur_index){
	if (root){
		number_features(root->left, cur_index);
		root->feature_num = *cur_index;
		*cur_index += 1;
		// printf("%s %u\n", root->feature, *cur_index);
		number_features(root->right, cur_index);
	}
}



struct data make_example(char csv_line[]);
void print_data(struct data *instance);
char **parse_question(char *question, int num_words);
unsigned long str_hash(unsigned char *str);
void add_to_word_list(char **question, char **word_list, int *cur_index);
void add_to_cat_list(char *cat, char **cat_list, int *cur_index);

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
    int total_examples = 2;
    //max words per question
    int num_words = 300;
    //max word length
    int max_word_len = 20;
    //max vocab count
    int max_vocab = 200000;
    //data read in poorly
    int bad_iterations = 0;

    //Allocate space for data
    char *csv_line = malloc(sizeof(char)*1500);

    //store all data
    //array of structs
    //struct.question->array of char*
    //struct.cat->char*
    //struct.example_num->int
    struct data *all_data;
    all_data = malloc(sizeof(struct data)*total_examples);
    for (ii=0; ii<total_examples; ii++){
    	all_data[ii].question = malloc(sizeof(char*)*num_words);
    	for (jj=0; jj<num_words; jj++){
    		// all_data[ii].question[jj] = malloc(sizeof(char)*max_word_len);
    		all_data[ii].question[jj] = calloc(max_word_len, sizeof(char));
    	}
    	all_data[ii].cat = malloc(sizeof(char)*max_word_len);
    }

    //store vocabulary list (char** points to array of char* of length 20)
    char **word_list;
    word_list = malloc(sizeof(char*)*max_vocab); //assumes max_vocab total vocab
    for (ii=0; ii<max_vocab; ii++){
    	// word_list[ii] = malloc(sizeof(char)*max_word_len);  //assumes max word length of 20
    	word_list[ii] = calloc(max_word_len, sizeof(char));  //assumes max word length of 20
    }

    //alternate vocab store tree
    feature_tree *vocab;
    vocab = NULL;

    //store category list
    char **cat_list;
    cat_list = malloc(sizeof(char*)*40);  //assumes 20 max categories
    for (ii=0; ii<40; ii++){
    	cat_list[ii] = malloc(sizeof(char)*max_word_len);
		strncpy(cat_list[ii], "\0", 1);	
    }

    //Read in csv file
    FILE *f = fopen("train_pruned2.csv", "r");
    if (f == NULL){
    	printf("Failed to open file \n");
    	return -1;
    }

    //parse question into individual words, create vocabulary list
    int vocab_count = 0;
    int category_count = 0;

    for (i=0; i<total_examples; i++){
    	// printf("Iteration = %i\n", i);

    	//line in csv to buffer
    	if (fgets(csv_line, 1500, f) == NULL){
            printf("Fgets error!\n");
            exit(0);
        }

    	//csv line to 3 individual parts
    	if (i>0)
    	{
    		// instance = make_example(csv_line);
    		int num_words = 300;

			char *tok;
			char *tok_copy; //problem with tok getting overwritten in parse_question
			// char **parsed_question = malloc(sizeof(char*)*num_words);

			// printf("CSV_LINE = %s\n", csv_line);

			tok = strtok(csv_line, "|");
			if (tok == NULL){
				all_data[i-1].example_num = -1;
				bad_iterations++;
				continue;
			}
			sscanf(tok, "%i", &all_data[i-1].example_num);

			

			tok = strtok(NULL, "|");
			if (tok == NULL){
				all_data[i-1].example_num = -1;
				bad_iterations++;
				continue;
			}
			tok_copy = (char *)tok;

			

			tok = strtok(NULL, "|");
			if (tok == NULL){
				all_data[i-1].example_num = -1;
				bad_iterations++;
				continue;
			}
			strncpy(all_data[i-1].cat, tok, 19);
			all_data[i-1].cat[max_word_len-1] = 0;


			char *tok2;
			tok2 = strtok(tok_copy, " \t");

		    strncpy(all_data[i-1].question[0], tok2, 19);
		    all_data[i-1].question[0][max_word_len-1] = 0;

		    //add to tree
    		insert_word(&vocab, all_data[i-1].question[0]);

			j = 1;
			while (tok2 != NULL){
				if (j>=num_words){
					break;
				}
				tok2 = strtok(NULL, " \t");
		        if (tok2 != NULL){
		            strncpy(all_data[i-1].question[j], tok2, 19);
		            all_data[i-1].question[j][max_word_len-1] = 0;

		            //add to tree
    				insert_word(&vocab, all_data[i-1].question[j]);
		        }
				j++;
			} //end while

    		// all_data[i-1] = instance;
    		// print_data(&all_data[i-1]);

    		//add to vocabulary
    		// add_to_word_list(all_data[i-1].question, word_list, &vocab_count);
    		
    		//add to category list
    		add_to_cat_list(all_data[i-1].cat, cat_list, &category_count);

    	} //end if
    } //end for

    printf("Bad iterations = %i/%i\n", bad_iterations, i);
    // printf("Feature count array = %i\n", vocab_count);
    printf("Feature count tree = %i\n", count_features(vocab));

    for (ii=0; ii<40; ii++){
    	printf("%s", cat_list[ii]);
    }

    // for (ii=0;ii<41;ii++){
    // 	printf("%s ", word_list[ii]);
    // }
    unsigned int mm = 0;
    // printf("mm = %u\n", mm);
    number_features(vocab, &mm);
    // print_inorder(vocab);

    // printf("vocab->right = %s \n", vocab->feature);
    // print_data(&all_data[0]);
    // print_data(&all_data[29000]);


    //close file
    fclose(f);

    //free feature tree
    free_feature_tree(vocab);


    ////free malloc calls////
    //free vocab list
    for (ii=0; ii<max_vocab; ii++){
        free(word_list[ii]);  
    }
    free(word_list);

    //free category list
    for (ii=0; ii<40; ii++){
        free(cat_list[ii]);  
    }
    free(cat_list);

    //free all_data list
    for (ii=0; ii<total_examples; ii++){
        
        for (jj=0; jj<num_words; jj++){
            free(all_data[ii].question[jj]);
        }
        free(all_data[ii].question);
        free(all_data[ii].cat);
    }
    free(all_data);

    //free var used to rean in csv
    free(csv_line);

}

void add_to_cat_list(char *cat, char **cat_list, int *cur_index){
	int ii, jj, add_cat;


	if (cat == NULL){
		return;
	}

	// printf("%s ", cat);

	add_cat = 0;
	for (jj=0; jj<*cur_index; jj++){
		if (!strcmp(cat, cat_list[jj])){
			// printf()
			add_cat = 1;
			return;
		}
	}

	

	if (add_cat == 0){
		strncpy(cat_list[*cur_index], cat, 19);
		cat_list[*cur_index][19] = 0;
		(*cur_index)++;
	}

}


void add_to_word_list(char **question, char **word_list, int *cur_index){
	int num_words = 300;
	int ii, jj, add_word;

	// printf("Cur_index = %i\n", *cur_index);

	for (ii=0; ii<num_words; ii++){
		if (question[ii] == NULL){
			continue;
		}

        // printf("%s\n", question[ii]);

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
			// printf("%i\n", *cur_index);
			strncpy(word_list[*cur_index], question[ii], 19);
			word_list[*cur_index][19] = 0;
			// printf("%s ", question[ii]);
			(*cur_index)++;
		}
	}
}

void print_data(struct data *instance){
	if (instance->example_num == -1){
		printf("Bad example\n");
		return;
	}
	printf("example_num = %i\n", instance->example_num);
	int p = 0;
	printf("Question = ");
	for (p=0; p<10; p++){
		printf("%s ", (instance->question)[p]);
	}
	printf("\n");
	printf("Category = %s\n", instance->cat);
}
