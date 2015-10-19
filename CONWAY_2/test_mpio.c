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

    int array_of_gsizes[2] = {6, 6};
    int array_of_distribs[2] = { MPI_DISTRIBUTE_BLOCK, MPI_DISTRIBUTE_BLOCK };
    int array_of_dargs[2] = { MPI_DISTRIBUTE_DFLT_DARG, MPI_DISTRIBUTE_DFLT_DARG };
    int array_of_psize[2] = {3, 3};
    // int order = MPI_ORDER_C;

    //size,rank,ndims,array_gsizes,array_distribs,array_args,array_psizes
    //order,oldtype,*newtype
    MPI_Type_create_darray(world_size, rank, 2, array_of_gsizes, array_of_distribs,
            array_of_dargs, array_of_psize, MPI_ORDER_C, MPI_UNSIGNED_CHAR, &submatrix);
    MPI_Type_commit(&submatrix);


     /* open the file, and set the view */
    MPI_File file;
    MPI_File_open(MPI_COMM_WORLD, "test_mpio.pgm", 
                  MPI_MODE_CREATE|MPI_MODE_WRONLY,
                  MPI_INFO_NULL, &file);

    MPI_File_set_view(file, 0,  MPI_UNSIGNED_CHAR, MPI_UNSIGNED_CHAR, 
                           "native", MPI_INFO_NULL);

    // //header
    // fprintf(fp, "P2\n");
    // fprintf(fp, "%4d %4d\n", rsize, csize);
    // fprintf(fp, "255\n");
    char header1[15];
    header1[0] = 0x50;
    header1[1] = 0x35;
    header1[2] = 0x0a;
    header1[3] = 0x30;
    header1[4] = 0x30;
    header1[5] = 0x36;
    header1[6] = 0x20;
    header1[7] = 0x30;
    header1[8] = 0x30;
    header1[9] = 0x36;
    header1[10] = 0x0a;
    header1[11] = 0x32;
    header1[12] = 0x35;
    header1[13] = 0x35;
    header1[14] = 0x0a;

    char footer;
    footer = 0x0a;

    //write header
    MPI_File_write(file, &header1, 15, MPI_CHAR, MPI_STATUS_IGNORE);

    MPI_Barrier(MPI_COMM_WORLD);

    //write matrix
    MPI_File_set_view(file, 15,  MPI_UNSIGNED_CHAR, submatrix, 
                           "native", MPI_INFO_NULL);

    MPI_File_write_all(file, section, section_size, MPI_UNSIGNED_CHAR, MPI_STATUS_IGNORE);

    MPI_Barrier(MPI_COMM_WORLD);

    //write footer (trailing newline)
    MPI_File_set_view(file, 15+section_size*world_size,  MPI_UNSIGNED_CHAR, MPI_UNSIGNED_CHAR, 
                           "native", MPI_INFO_NULL);

    MPI_File_write(file, &footer, 1, MPI_CHAR, MPI_STATUS_IGNORE);

    MPI_Type_free(&submatrix);
    free(section);

    MPI_Finalize();
}