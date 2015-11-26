#include "stdlib.h"
#include "argp.h"
#include "mpi.h"
#include "stdio.h"
#include "math.h"
#include "string.h"
#include "unistd.h"


//stores data in text form
struct data{
	int example_num;
	char **question;
	char *cat;
};

//data is sparse (each example has ~100/150,000 points)
//using 'numeric_data' and 'feature_count' structs to store sparse data
struct feature_count{
	unsigned int feature_num;
	unsigned char count;
};

//stores data in numeric form
struct numeric_data{
	int example_num;
	struct feature_count *array_of_features;
	int cat;
};

//tree to hold vocabulary (features)
typedef struct feature_tree{
	unsigned int feature_num;
	char *feature;
	struct feature_tree *left;
	struct feature_tree *right;
}feature_tree;

//Insert a word into the tree
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
		temp->feature_num = 0;
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


//Free's the tree
void free_feature_tree(feature_tree *root){
	if (root){
		free_feature_tree(root->left);
		free_feature_tree(root->right);
		free(root->feature);
		free(root);
	}
}

//counts total number of features
int count_features(feature_tree *root){
	int count = 0;
	if (root == NULL){
		return 0;
	}
	count += 1 + count_features(root->left) + count_features(root->right);
	return count;
}

//print out features and their current numbers in order
void print_inorder(feature_tree *root){
	if (root){
		print_inorder(root->left);
		printf("%s %u\n", root->feature, root->feature_num);
		print_inorder(root->right);
	}
}

//takes in tree root and pointer to int, numbers all features in order
void number_features(feature_tree *root, unsigned int *cur_index){
	if (root){
		number_features(root->left, cur_index);
		root->feature_num = *cur_index;
		*cur_index += 1;
		// printf("%s %u\n", root->feature, *cur_index);
		number_features(root->right, cur_index);
	}
}

unsigned int get_feature_number(feature_tree **root, char *word){
	//nothing there
	if (!(*root)){
		return 0;
	}

	//equal to
	if (strcmp(word, (*root)->feature) == 0){
		return (*root)->feature_num;
	}
	//less than goes left
	else if (strcmp(word, (*root)->feature) < 0){
		get_feature_number(&(*root)->left, word);
	}
	//greater than goes right
	else if (strcmp(word, (*root)->feature) > 0){
		get_feature_number(&(*root)->right, word);
	}
}


struct data make_example(char csv_line[]);
void print_data(struct data *instance);
void print_num_data(struct numeric_data *instance);
char **parse_question(char *question, int num_words);
unsigned long str_hash(unsigned char *str);
void add_to_word_list(char **question, char **word_list, int *cur_index);
void add_to_cat_list(char *cat, char **cat_list, int *cur_index);
void words_to_num(struct numeric_data *num_data, struct data *all_data, 
	                               feature_tree **vocab, int num_words);


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
    // int max_vocab = 200000;
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

    //store numeric version of data for algorithms
    struct numeric_data *num_data;
    num_data = malloc(sizeof(struct numeric_data)*total_examples);
    for (ii=0; ii<total_examples; ii++){
    	num_data[ii].array_of_features = malloc(sizeof(struct feature_count)*num_words);
    	for (jj=0; jj<num_words; jj++){
    		num_data[ii].array_of_features[jj].feature_num = 0;
    		num_data[ii].array_of_features[jj].count = 0;
    	}
    }
    

    // //store vocabulary list (char** points to array of char* of length 20)
    // char **word_list;
    // word_list = malloc(sizeof(char*)*max_vocab); //assumes max_vocab total vocab
    // for (ii=0; ii<max_vocab; ii++){
    // 	// word_list[ii] = malloc(sizeof(char)*max_word_len);  //assumes max word length of 20
    // 	word_list[ii] = calloc(max_word_len, sizeof(char));  //assumes max word length of 20
    // }

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
    int category_count = 1;

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
		        if ((tok2 != NULL) && (strlen(tok2)>3)){
		            strncpy(all_data[i-1].question[j], tok2, 19);
		            all_data[i-1].question[j][max_word_len-1] = 0;

		            //add to tree
    				insert_word(&vocab, all_data[i-1].question[j]);
    				j++;
		        }
				
			} //end while

    		// all_data[i-1] = instance;
    		// print_data(&all_data[i-1]);

    		////add to vocabulary (using array, VERY slow with lots of data)
    		// add_to_word_list(all_data[i-1].question, word_list, &vocab_count);
    		
    		//add to category list
    		add_to_cat_list(all_data[i-1].cat, cat_list, &category_count);

    	} //end if
    } //end for

    //assign unique number to each feature
    //first feature is feature 1, feature 0 is for errors etc.
    unsigned int mm = 1;
    number_features(vocab, &mm);

    //Some of the csv rows aren't read in properly with fgets
    printf("Bad iterations = %i/%i\n", bad_iterations, i);
    printf("Feature count = %i\n", count_features(vocab));

    print_inorder(vocab);

    // for (ii=0; ii<40; ii++){
    // 	printf("%s", cat_list[ii]);
    // }

    ////turn data into numeric features////
    for (i=0; i<total_examples; i++){
    	num_data[i].example_num = all_data[i].example_num;
    	num_data[i].cat = get_cat_index(cat_list, all_data[i].cat);
    	words_to_num(&num_data[i], &all_data[i], &vocab, num_words);
    }

    print_num_data(&num_data[0]);
    
    // printf("vocab->right = %s \n", vocab->feature);
    // print_data(&all_data[0]);
    // print_data(&all_data[29000]);
    // printf("%s, %u\n", "1829", get_feature_number(&vocab, "1829"));

    //close file
    fclose(f);


    ////free malloc calls////
    //free feature tree
    free_feature_tree(vocab);

    //free numeric data
    for (ii=0; ii<total_examples; ii++){
    	free(num_data[ii].array_of_features);
    }
    free(num_data);

    // //free vocab list
    // for (ii=0; ii<max_vocab; ii++){
    //     free(word_list[ii]);  
    // }
    // free(word_list);

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

int get_cat_index(char **cat_list, char *cat){
	int index;
	for (index = 0; index<40; index++){
		if (strcmp(cat_list[index], cat) == 0){
			return index;
		}
	}
	return 0;
}

void words_to_num(struct numeric_data *num_data, struct data *all_data, 
	                               feature_tree **vocab, int num_words){
	int ii, jj;
	unsigned int cur_feature;
	for (ii=0; ii<num_words; ii++){
		//get feature number for every word
		cur_feature = get_feature_number(vocab, all_data->question[ii]);

		//If no more feature break
		if (cur_feature == 0){
			break;
		}
		// printf("%s %u  ", all_data->question[ii], cur_feature);

		//if valid feature number, add to feature array
		if (cur_feature > 0){
			//check if feature is already in array
			//add to count if it is, otherwise att
			for (jj=0; jj<num_words; jj++){
				if (cur_feature == num_data->array_of_features[jj].feature_num){
					num_data->array_of_features[jj].count += 1;
					break;
				}
				else if (num_data->array_of_features[jj].feature_num == 0){
					num_data->array_of_features[jj].feature_num = cur_feature;
					num_data->array_of_features[jj].count += 1;
					break;
				}

			} 
		}
	}
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

void print_num_data(struct numeric_data *instance){
	if (instance->array_of_features->feature_num == 0){
		printf("Bad example\n");
		return;
	}

	printf("example_num = %i\n", instance->example_num);
	printf("Data array = ");
	int p = 0;
	for (p=0; p<40; p++){
		printf("%u-%u ", instance->array_of_features[p].feature_num, 
			              instance->array_of_features[p].count);
	}
	printf("\nCategory = %i\n", instance->cat);

}
