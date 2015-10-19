#include "stdlib.h"
#include "argp.h"
#include "mpi.h"
#include "stdio.h"
#include "math.h"
#include "string.h"
#include "unistd.h"

int main (int argc, char **argv)
{



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

    int section_size = 4;

    unsigned char *section;
    section = (unsigned char*)malloc(sizeof(unsigned char)*section_size);

    unsigned short ii;
    for (ii=0;ii<section_size;ii++)
    {
        if (rank%2 == 0)
        {
            section[ii] = 255;
        }
        else
        {
            section[ii] = 0;
        }

        if (rank == 0)
        {
            printf("%i ", section[ii]);
        }
    }

    //Create new derived datatype
    MPI_Datatype submatrix;

    int array_of_gsizes[2] = {4, 4};
    int array_of_distribs[2] = { MPI_DISTRIBUTE_BLOCK, MPI_DISTRIBUTE_BLOCK };
    int array_of_dargs[2] = { MPI_DISTRIBUTE_DFLT_DARG, MPI_DISTRIBUTE_DFLT_DARG };
    int array_of_psize[2] = {2, 2};
    // int order = MPI_ORDER_C;

    //size,rank,ndims,array_gsizes,array_distribs,array_args,array_psizes
    //order,oldtype,*newtype
    MPI_Type_create_darray(world_size, rank, 2, array_of_gsizes, array_of_distribs,
            array_of_dargs, array_of_psize, MPI_ORDER_C, MPI_UNSIGNED_CHAR, &submatrix);
    MPI_Type_commit(&submatrix);


     /* open the file, and set the view */
    MPI_File file;
    MPI_File_open(MPI_COMM_WORLD, "test_mpio", 
                  MPI_MODE_CREATE|MPI_MODE_WRONLY,
                  MPI_INFO_NULL, &file);

    MPI_File_set_view(file, 0,  MPI_UNSIGNED_CHAR, submatrix, 
                           "native", MPI_INFO_NULL);

    // //header
    // fprintf(fp, "P2\n");
    // fprintf(fp, "%4d %4d\n", rsize, csize);
    // fprintf(fp, "255\n");
    // char *header1[2];
    // header1[0] = "P2";
    // header1[1] = "///n";
    // header1[0] = "P";
    // header1[1] = "2";
    // header1[2] = "\\";
    // header1[3] = "n";

    // unsigned char header2[3];
    // header2[0] = "0";
    // header2[1] = "0";
    // header2[2] = "0";
    // header2[3] = "4";
    // header2[4] = " ";
    // header2[5] = "0";
    // header2[6] = "0";
    // header2[7] = "0";
    // header2[8] = "4";
    // header2[9] = "\\";
    // header2[10] = "n";

    // unsigned char header3[5];

    // if (rank == 0)
    // {
    //     MPI_File_write(file, &header1, 5, MPI_CHAR, MPI_STATUS_IGNORE);
    // }

    // MPI_Barrier(MPI_COMM_WORLD);

    MPI_File_write_all(file, section, section_size, MPI_UNSIGNED_CHAR, MPI_STATUS_IGNORE);

    MPI_Type_free(&submatrix);

    MPI_Finalize();
}