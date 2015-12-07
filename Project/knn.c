#include "stdlib.h"
#include "stdarg.h"
#include "argp.h"
#include "mpi.h"
#include "omp.h"
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
    int total_features; //number of features for given example
	struct feature_count *array_of_features;
	int cat;
};

//Will store k nearest neighbors to example
struct distance_results{
	int example_num;
	double *distances;
	int *cat;
	int *example_nums;
	int correct_answer;
};

struct mode{
	int *count;
	int *cat;
};

//tree to hold vocabulary (features)
typedef struct feature_tree{
	unsigned int feature_num;
	char *feature;
	struct feature_tree *left;
	struct feature_tree *right;
}feature_tree;

struct data make_example(char csv_line[]);
void print_data(struct data *instance);
void print_num_data(struct numeric_data *instance);
char **parse_question(char *question, int num_words);
unsigned long str_hash(unsigned char *str);
void add_to_word_list(char **question, char **word_list, int *cur_index);
void add_to_cat_list(char *cat, char **cat_list, int *cur_index);
void words_to_num(struct numeric_data *num_data, struct data *all_data, 
	                               feature_tree **vocab, int num_words);
int count_features2(struct numeric_data *num_data);
double get_distance(struct numeric_data *s1, struct numeric_data *s2, int num_words);
void add_distance_to_results(struct distance_results *results, double distance,
											   int k, int cat, int example_num);
int calc_nearest_neighbor(struct distance_results *results, struct mode *mod, int k);
// int classify_point(struct)

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

const char *argp_program_version =
    "argp-ex3 1.0";
const char *argp_program_bug_address =
    "<bug-gnu-utils@gnu.org>";

/* Program documentation. */
static char doc[] =
    "A program with options and arguments using argp";

/* A description of the arguments we accept. */
static char args_doc[] = "0=Serial,1=Parallel kVal DataPercent";

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
            if (state->arg_num >= 4)
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

    //mpi = 0, omp = 1, hybrid = 2
    int run_type;
    run_type = 0; //default is serial
    if (sscanf (arguments.args[0], "%i", &run_type)!=1) {}
    if (run_type < 0 || run_type > 2){
        printf("Valid run_types are:\n MPI=0 OMP=1 HYBRID=2\n");
        exit(0);
    }

    // number of nearest neighbors
    int k;
    k = 1; //default is 1
    if (sscanf (arguments.args[1], "%i", &k)!=1) {}
    if (k < 1){
        printf("Need a positive integer value for k, I recommend between 1 and 50\n");
        exit(0);
    }

    //omp vars
    int num_threads;
    num_threads = 4;  //default
	if (sscanf(arguments.args[2], "%i", &num_threads)!=1) {}
    if (run_type == 1 || run_type == 2){
        if (num_threads < 1){
            printf("Need a positive integer value threads if using openmp\n");
            exit(0);
        }
    }
    else if (num_threads < 0){
        printf("Need a positive integer value threads if using openmp\n");
        exit(0);
    }

    //number of examples to read in
    int total_examples;
    total_examples = 10000;  //default
    if (sscanf(arguments.args[3], "%i", &total_examples)!=1) {}
    if (total_examples < 1){
        printf("Need a positive integer value for examples between 1 and 41639\n");
        exit(0);
    }
    else if (total_examples > 41639){
        total_examples = 41639;
    }

    //verbose?
    int verbose;
    verbose = arguments.verbose;
    if (verbose>0 && verbose<130){
        verbose = 1;
    }
    else{
        verbose = 0;
    }


    //define a bunch of counters!
    int i, j, m, n, ii, jj, kk;

    //max words per question
    int num_words = 300;

    //max word length
    int max_word_len = 20;
    //max vocab count
    // int max_vocab = 200000;

    //data read in poorly
    int bad_iter = 0;

    //Used to split into training and testing data (will train on example_num%train)
    int train = 10;

    //Debug
    int debug = 0;

    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Get the name of the processor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    if (rank == 0 && verbose>0){
        char *str;
        if (run_type == 0){
            str = "MPI Only";
            num_threads = 0;
        }
        else if (run_type == 1){
            str = "OpenMP Only";
        }
        else if (run_type == 2){
            str = "MPI/OpenMP Hybrid";
        }
        printf("RunType=%s, k=%i, Verbose=%i\nnum_processes=%i, num_threads=%i\ndata_used=%i/41639\n",
                         str, k, verbose, world_size, num_threads,  total_examples);
    }

     //Allocate space for data being read in with fgets
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

			char *tok;
			char *tok_copy; //problem with tok getting overwritten in parse_question
			// char **parsed_question = malloc(sizeof(char*)*num_words);

			// printf("CSV_LINE = %s\n", csv_line);

			tok = strtok(csv_line, "|");
			if (tok == NULL){
				// all_data[i-bad_iter-1].example_num = -1;
				bad_iter++;
				// i--;
				continue;
			}
			sscanf(tok, "%i", &all_data[i-bad_iter-1].example_num);

			

			tok = strtok(NULL, "|");
			if (tok == NULL){
				// all_data[i-bad_iter-1].example_num = -1;
				bad_iter++;
				// i--;
				continue;
			}
			tok_copy = (char *)tok;

			

			tok = strtok(NULL, "|");
			if (tok == NULL){
				// all_data[i-bad_iter-1].example_num = -1;
				bad_iter++;
				// i--;
				continue;
			}
			strncpy(all_data[i-bad_iter-1].cat, tok, 19);
			all_data[i-bad_iter-1].cat[max_word_len-1] = 0;


			char *tok2;
			tok2 = strtok(tok_copy, " \t");

			j = 0;
			if ((tok2 != NULL) && (strlen(tok2)>3)){
				strncpy(all_data[i-bad_iter-1].question[0], tok2, 19);
		    	all_data[i-bad_iter-1].question[0][max_word_len-1] = 0;

		    	//add to tree if not test data
		    	// if (all_data[i-bad_iter-1].example_num % train != 0){
		    	insert_word(&vocab, all_data[i-bad_iter-1].question[0]);
    			j += 1;
		    	// }
    			
			}

			while (tok2 != NULL){
				if (j>=num_words){
					break;
				}
				tok2 = strtok(NULL, " \t");
		        if ((tok2 != NULL) && (strlen(tok2)>3)){
		            strncpy(all_data[i-bad_iter-1].question[j], tok2, 19);
		            all_data[i-bad_iter-1].question[j][max_word_len-1] = 0;

		            //add to tree if not test data
		            // if (all_data[i-bad_iter-1].example_num % train != 0){
					insert_word(&vocab, all_data[i-bad_iter-1].question[j]);
					j++;
    				// }
		        }
				
			} //end while

    		// all_data[i-bad_iter-1] = instance;
    		// print_data(&all_data[i-bad_iter-1]);

    		////add to vocabulary (using array, VERY slow with lots of data)
    		// add_to_word_list(all_data[i-bad_iter-1].question, word_list, &vocab_count);
    		
    		//add to category list
    		add_to_cat_list(all_data[i-bad_iter-1].cat, cat_list, &category_count);

    	} //end if
    } //end for

    //close file
    fclose(f);

    //assign unique number to each feature
    //first feature is feature 1, feature 0 is for errors etc.
    unsigned int mm = 1;
    number_features(vocab, &mm);

    //Some of the csv rows aren't read in properly with fgets
    if (rank == 0 && verbose>0){
        printf("Bad iterations = %i/%i\n", bad_iter, i);
        printf("Feature count = %i\n", count_features(vocab));
    }
    // print_inorder(vocab);

    // for (ii=0; ii<40; ii++){
    // 	printf("%s", cat_list[ii]);
    // }

    ////turn data into numeric features////
    for (i=0; i<total_examples; i++){
    	num_data[i].example_num = all_data[i].example_num;
    	num_data[i].cat = get_cat_index(cat_list, all_data[i].cat);
    	words_to_num(&num_data[i], &all_data[i], &vocab, num_words);
        // count_features2(&num_data[i]);
    }

    // num_data->array_of_features[0].feature_num = 44;

    // print_num_data(&num_data[0]);
    // print_num_data(&num_data[1]);
    total_examples = total_examples-bad_iter-1;

    int sadfjh;
    double av_feature_count = 0;
    for (ii=0; ii<total_examples; ii++){
        sadfjh = count_features2(&num_data[ii]);
        av_feature_count += sadfjh;
        // printf("%i ", sadfjh);
    }
    // printf("\n av_feature_count %f\n", av_feature_count/(total_examples-bad_iter-1));
    
    
    // print_num_data(&num_data[4464]);

    // printf("vocab->right = %s \n", vocab->feature);
    // print_data(&all_data[0]);
    // print_data(&all_data[29000]);
    // printf("%s, %u\n", "1829", get_feature_number(&vocab, "1829"));

    //Debugging why some examples were always closest...
    // for (ii=0; ii<total_examples; ii++){
    //     if (num_data[ii].example_num == 4664){
    //         print_num_data(&num_data[ii]);
    //     }
    //     else if (num_data[ii].example_num == 1505){
    //         print_num_data(&num_data[ii]);
    //     }
    //     else if (num_data[ii].example_num == 10124){
    //         print_num_data(&num_data[ii]);
    //     }
    //     else if (num_data[ii].example_num == 550){
    //         print_num_data(&num_data[ii]);
    //     }
    // }


    //find the distance between first example and rest
    double distance;

    //range each process will cover
    int range;
    range = total_examples/world_size;

    // printf("%i, %i\n", range, total_examples);
    // printf("R, Min, Max = %i, %i, %i\n", rank, rank*range, (rank+1)*range);


   	//correct/total/answer
   	int c = 0;
    int total = 0;
    int answer;

    //Timing variables
    double start_time, end_time;
    double total_time[10] = {0};

    //MPI only
    if (run_type == 0)
    {
        //store struct which keep track of the k nearest neighbors
        struct distance_results results;
        results.example_num = 0;
        results.distances = calloc(k, sizeof(double));
        results.cat = calloc(k, sizeof(int));
        results.example_nums = calloc(k, sizeof(int));

        //struct used to calculate the mode of the k nearest neighbors
        struct mode mod;
        mod.count = calloc(k, sizeof(int));
        mod.cat = calloc(k, sizeof(int));

        //Timing loop
        for(jj=0; jj<11; jj++){

            c = 0;
            total = 0;

            //Sync before every iteration
            MPI_Barrier(MPI_COMM_WORLD);

            //Have one burn in iteration
            if (jj>0){
                start_time = MPI_Wtime();
            }

            //Main loop
            for (kk=rank*range; kk<(rank+1)*range; kk++){
                //only test on test data
                if (num_data[kk].example_num%train != 0){
                    continue;
                }

                //Skip if category doesn't exist
                if (num_data[kk].cat == 0){
                    continue;
                }

                results.correct_answer = num_data[kk].cat;
                results.example_num = num_data[kk].example_num;
                for (ii=0; ii<k; ii++){
                    results.distances[ii] = 0;
                    results.cat[ii] = 0;
                    mod.count[ii] = 0;
                    mod.cat[ii] = 0;
                }

                //calc distance to neighbors
                for (ii=0; ii<total_examples-1; ii++){
                    //don't calc distance to self
                    if (kk != ii){
                        //Eliminate bad data (examples with few words tend to have low distances
                        //reguardless of whether they are more similar...
                        if (num_data[ii].total_features >= 40){
                            distance = get_distance(&num_data[kk], &num_data[ii], num_words);
                            // if (distance < 2){
                            //  continue;
                            // }
                            // printf("%f ", distance);
                            if (num_data[ii].example_num > 0){
                                add_distance_to_results(&results, distance, k, 
                                                        num_data[ii].cat, num_data[ii].example_num);
                            }
                        }
                    }
                    
                }

                answer = calc_nearest_neighbor(&results, &mod, k);
                if (answer == results.correct_answer){
                    c += 1;
                }

                total += 1;

                if (verbose>0 && debug>0){
                    printf("Process = %i, Correct/Total = %i/%i  Answer/Correct = %i/%i\n", 
                            rank, c, total, answer, results.correct_answer);
                }
                
            }

            if (jj>0){
                end_time = MPI_Wtime();
            }
            total_time[jj-1] = end_time - start_time;

        }
            
        //free distance result
        free(results.distances);
        free(results.cat); 

        //free mode struct
        free(mod.count);
        free(mod.cat);

        if (verbose>0){
           MPI_Barrier(MPI_COMM_WORLD);
            if (rank == 0){
                printf("/// Process Results ///\n");
            }
            MPI_Barrier(MPI_COMM_WORLD);
            printf("Process = %i, Correct/Total = %i/%i\n", 
                            rank, c, total); 
            sleep(0.1);
            MPI_Barrier(MPI_COMM_WORLD);
    
        }
    }
    //OMP ONLY (using mpirun, only use first process)
    if (run_type == 1 && rank == 0)
    {
        omp_set_dynamic(0); //Explicitly disable dynamic teams
        omp_set_num_threads(num_threads); //Specify thread count

        #pragma omp parallel \
                private(kk, ii, distance, answer) \
                reduction(+:c,total) \
                shared(num_data)
        {
            //store struct which keep track of the k nearest neighbors
            struct distance_results results;
            results.example_num = 0;
            results.distances = calloc(k, sizeof(double));
            results.cat = calloc(k, sizeof(int));
            results.example_nums = calloc(k, sizeof(int));

            //struct used to calculate the mode of the k nearest neighbors
            struct mode mod;
            mod.count = calloc(k, sizeof(int));
            mod.cat = calloc(k, sizeof(int));
        
            #pragma omp for
            for (kk=0; kk<total_examples; kk++){
                // printf("Thread = %i, Iter = %i, c = %i, total=%i\n", omp_get_thread_num(), kk, c, total);

                //only test on test data
                if (num_data[kk].example_num%train != 0){
                    continue;
                }

                if (num_data[kk].cat == 0){
                    continue;
                }

                results.correct_answer = num_data[kk].cat;
                results.example_num = num_data[kk].example_num;
                for (ii=0; ii<k; ii++){
                    results.distances[ii] = 0;
                    results.cat[ii] = 0;
                    mod.count[ii] = 0;
                    mod.cat[ii] = 0;
                }

                // print_num_data(&num_data[kk]);

                //calc distance to neighbors
                for (ii=0; ii<total_examples-1; ii++){
                    //don't calc distance to self
                    if (kk != ii){
                        //Eliminate bad data (examples with few words tend to have low distances
                        //reguardless of whether they are more similar...
                        if (num_data[ii].total_features >= 40){
                            distance = get_distance(&num_data[kk], &num_data[ii], num_words);
                            // if (distance < 2){
                            //  continue;
                            // }
                            // printf("%f ", distance);
                            if (num_data[ii].example_num > 0){
                                add_distance_to_results(&results, distance, k, 
                                                        num_data[ii].cat, num_data[ii].example_num);
                            }
                        }
                    }
                    
                }

                answer = calc_nearest_neighbor(&results, &mod, k);
                if (answer == results.correct_answer){
                    c += 1;
                }

                total += 1;

                if (verbose>0 && debug>0){
                    printf("Thread = %i, Correct/Total = %i/%i  Answer/Correct = %i/%i\n", 
                        omp_get_thread_num(), c, total, answer, results.correct_answer);
                }
                
            }

            if(verbose > 0){
                //Thread results
                #pragma omp barrier
                if (omp_get_thread_num() == 0){
                    printf("/// Thread Results ///\n");
                }
                #pragma omp barrier
                printf("Thread = %i, Correct/Total = %i/%i\n", 
                        omp_get_thread_num(), c, total);
                #pragma omp barrier
            }
            

            //free distance result
            free(results.distances);
            free(results.cat); 

            //free mode struct
            free(mod.count);
            free(mod.cat);

        }

        if (verbose > 0){
            printf("/// Final Results ///\n");
            printf("Correct/Total = %i/%i\n", c, total);
        }
    }
    //HYBRID
    if (run_type == 2)
    {
        omp_set_dynamic(0); //Explicitly disable dynamic teams
        omp_set_num_threads(num_threads); //Specify thread count

        #pragma omp parallel \
            private(kk, ii, distance, answer) \
            firstprivate(verbose) \
            reduction(+:c,total) \
            shared(num_data)
        {
            //store struct which keep track of the k nearest neighbors
            struct distance_results results;
            results.example_num = 0;
            results.distances = calloc(k, sizeof(double));
            results.cat = calloc(k, sizeof(int));
            results.example_nums = calloc(k, sizeof(int));

            //struct used to calculate the mode of the k nearest neighbors
            struct mode mod;
            mod.count = calloc(k, sizeof(int));
            mod.cat = calloc(k, sizeof(int));

            #pragma omp for
            for (kk=rank*range; kk<(rank+1)*range; kk++){
                //only test on test data
                if (num_data[kk].example_num%train != 0){
                    continue;
                }

                if (num_data[kk].cat == 0){
                    continue;
                }

                results.correct_answer = num_data[kk].cat;
                results.example_num = num_data[kk].example_num;
                for (ii=0; ii<k; ii++){
                    results.distances[ii] = 0;
                    results.cat[ii] = 0;
                    mod.count[ii] = 0;
                    mod.cat[ii] = 0;
                }

                // print_num_data(&num_data[kk]);

                //calc distance to neighbors
                for (ii=0; ii<total_examples-1; ii++){
                    //don't calc distance to self
                    if (kk != ii){
                        //Eliminate bad data (examples with few words tend to have low distances
                        //reguardless of whether they are more similar...
                        if (num_data[ii].total_features >= 40){
                            distance = get_distance(&num_data[kk], &num_data[ii], num_words);
                            // if (distance < 2){
                            //  continue;
                            // }
                            // printf("%f ", distance);
                            if (num_data[ii].example_num > 0){
                                add_distance_to_results(&results, distance, k, 
                                                        num_data[ii].cat, num_data[ii].example_num);
                            }
                        }
                    }
                    
                }

                answer = calc_nearest_neighbor(&results, &mod, k);
                if (answer == results.correct_answer){
                    c += 1;
                }
                // printf("\n");
                // for (ii=0; ii<k; ii++){
                //  printf("Distance, cat, example_num1, example_num2 = %2.2f, %i, %i, %i\n", 
                //      results.distances[ii], results.cat[ii], results.example_num, results.example_nums[ii]);
                // }
                // else{
                    
                // }
                total += 1;

                if (verbose>0 && debug>0){
                    printf("Process = %i, Thread = %i, Correct/Total = %i/%i  Answer/Correct = %i/%i\n", 
                            rank, omp_get_thread_num(), c, total, answer, results.correct_answer);
                }

            }

            if (verbose > 0){
                //Individual thread result
                MPI_Barrier(MPI_COMM_WORLD);
                if (rank == 0 && omp_get_thread_num() == 0){
                    printf("/// Thread Results ///\n");
                }
                MPI_Barrier(MPI_COMM_WORLD);
                #pragma omp barrier
                printf("Process = %i, Thread = %i, Correct/Total = %i/%i\n", 
                        rank, omp_get_thread_num(), c, total);
            }

            //free distance result
            free(results.distances);
            free(results.cat); 

            //free mode struct
            free(mod.count);
            free(mod.cat);
        }
    }
    

    // int MPI_Reduce(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype,
    //            MPI_Op op, int root, MPI_Comm comm)

    if ((run_type == 0 || run_type == 2) && verbose > 0){
        int total_correct = 0;
        int total_tested = 0;
        MPI_Reduce(&c, &total_correct, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        MPI_Reduce(&total, &total_tested, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

        MPI_Barrier(MPI_COMM_WORLD);
        if (rank == 0){
            printf("/// Final Results ///\n");
            printf("Correct/Total = %i/%i\n", total_correct, total_tested);
            // printf("verbose = %i", verbose);
        }
    }
    

    //print data to outfile
    MPI_Barrier(MPI_COMM_WORLD);

    //dynamic filename with leading zeroes for easy conversion to gif
    if (rank == 0){
        char buffer[128];
        // run_type _ k _ data_used . txt
        snprintf(buffer, sizeof(char)*128, "RESULTS/%i_%d_%d.txt", run_type, k, total_examples+bad_iter+1);

        // printf("BUFFER = %s\n", buffer);
        // MPI_Barrier(MPI_COMM_WORLD);

        //open
        FILE *fp;
        fp = fopen(buffer, "wb");

        for(i=0; i<10; i++)
        {
            if (i < 9)
            {
                fprintf(fp, "%2.9f,", total_time[i]);
            }
            else
            {
                fprintf(fp, "%2.9f\n", total_time[i]);
            }
        }
    }
    


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

    // //free distance result
    // free(results.distances);
    // free(results.cat); 

    // //free mode struct
    // free(mod.count);
    // free(mod.cat);

    MPI_Barrier(MPI_COMM_WORLD);
    sleep(0.5);
    MPI_Finalize();
}

//finds the most frequent class of nearest neighbor for a given example
//ties go to class with nearest neighbor
int calc_nearest_neighbor(struct distance_results *results, struct mode *mod, int k){
	int cur_class, count;
	int ii, jj;

	//count each class
	for (ii=0; ii<k; ii++){
		cur_class = results->cat[ii];

		//Don't count number of noexistant classes
		if (cur_class == 0){
			continue;
		}

		mod->count[ii] = 0;
		mod->cat[ii] = results->cat[ii];

		for (jj=0; jj<k; jj++){
			if (results->cat[jj] == cur_class){
				mod->count[ii] ++;
			}
		}
	}

	cur_class = mod->cat[0];
	count = mod->count[0];

	//find class maximums
	for (ii=1; ii<k; ii++){
		if (mod->count[ii] > count){
			count = mod->count[ii];
			cur_class = mod->cat[ii];
		}
	}

	return cur_class;
}


//adds distance from get_distance to results
void add_distance_to_results(struct distance_results *results, double distance, 
											int k, int cat, int example_num){
	int ii;
	double temp;
	int temp_cat;
	int temp_ex;

	//for whatever reason these two ALWAYS show up... there's a bug somewhere...
	// if ((example_num == 4664) || (example_num == 550) || (example_num == 10124)
 //        || (example_num == 1505)){
	// 	return;
	// }

	//ties are thrown out
	for (ii=0; ii<k; ii++){
		//if nothing in results yet
		if (results->cat[ii] == 0){
			// printf("CAT, DISTANCE = %i, %2.2f\n", cat, distance);
			results->cat[ii] = cat;
			results->distances[ii] = distance;
			results->example_nums[ii] = example_num;
			// printf("%i, %2.2f\n", results->cat[ii], results->distances[ii]);
			// printf("HEREHERHEHR\n");
			return;
		}

		//if this neighbor is closer, replace and
		//then check if replaces neighbor is less than
		//other neighbors
		if ((results->cat[ii] > 0) &&
			distance < results->distances[ii]){
			temp = results->distances[ii];
			temp_cat = results->cat[ii];
			temp_ex = results->example_nums[ii];

			results->distances[ii] = distance;
			results->cat[ii] = cat;
			results->example_nums[ii] = example_num;

			distance = temp;
			cat = temp_cat;
			example_num = temp_ex;
		}
	}
}

//subtracts two data "arrays" and then returns the distance
//the data arrays are in structs
//returns euclidean distance
double get_distance(struct numeric_data *s1, struct numeric_data *s2, int num_words){
	double cur_distance = 0;
	int ii, jj, same_feature;

	//check for features in both s1 and s2, as well as features only in s1
	for (ii=0; ii<num_words; ii++){
		
		same_feature = 0;
		
		//end when there are not more features in array
		if (s1->array_of_features[ii].feature_num == 0){
			break;
		}

		for (jj=0; jj<num_words; jj++){
			//end when there are not more features in array
			if (s2->array_of_features[jj].feature_num == 0){
				break;
			}

			//if they both have feature, subtract distance between them
			if (s1->array_of_features[ii].feature_num == 
				s2->array_of_features[jj].feature_num){
				cur_distance += ((s1->array_of_features[ii].count - s2->array_of_features[jj].count) *
								(s1->array_of_features[ii].count - s2->array_of_features[jj].count));
				// printf("1 - Cur_dist = %f\n", cur_distance);
				same_feature = 1;
				break;
			}
		}

		//if they don't both have the same feature
		if (same_feature == 0){
			cur_distance += (s1->array_of_features[ii].count * s1->array_of_features[ii].count);
			// printf("2 - Cur_dist = %f\n", cur_distance);
		}
	}

	//check for features only in s2
	for (ii=0; ii<num_words; ii++){
				
		//end when there are not more features in array
		if (s2->array_of_features[ii].feature_num == 0){
			break;
		}

		for (jj=0; jj<num_words; jj++){
			//end when there are not more features in array
			if (s1->array_of_features[jj].feature_num == 0){
				break;
			}

			//if they both have feature, break
			if (s2->array_of_features[ii].feature_num == 
				s1->array_of_features[jj].feature_num){
				break;
			}
		}

		cur_distance += (s2->array_of_features[ii].count * s2->array_of_features[ii].count);
		// printf("3 - Cur_dist = %f\n", cur_distance);
	}

	//sqrt to find euclidean distance
	return sqrt(cur_distance);
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

int count_features2(struct numeric_data *num_data){
    int ii;
    for (ii=0; ii<300; ii++){
        if (num_data->array_of_features[ii].feature_num == 0){
            break;
        }
    }
    num_data->total_features = ii;
    return ii;
}

void words_to_num(struct numeric_data *num_data, struct data *all_data, 
	                               feature_tree **vocab, int num_words){
	int ii, jj;
	unsigned int cur_feature;
	// printf("\n\n");
	for (ii=0; ii<num_words; ii++){
		//get feature number for every word
		cur_feature = get_feature_number(vocab, all_data->question[ii]);

		// printf("%s %u  ", all_data->question[ii], cur_feature);

		//If no more feature break
		if (cur_feature == 0){
			break;
		}
		// printf("%s %u  ", all_data->question[ii], cur_feature);

		//if valid feature number, add to feature array
		if (cur_feature > 0){
			//check if feature is already in array
			//add to count if it is, otherwise increase count
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
    printf("feature_count = %i\n", instance->total_features);
	printf("Data array = ");
	int p = 0;
	for (p=0; p<40; p++){
		printf("%u-%u ", instance->array_of_features[p].feature_num, 
			              instance->array_of_features[p].count);
	}
	printf("\nCategory = %i\n", instance->cat);

}
