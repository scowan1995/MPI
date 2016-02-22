#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <assert.h>

// Creates an array of random numbers. Each number has a value from 0 - 1
char *create_rand_chars(int num_elements) {
    char *rand_nums = (float *)malloc(sizeof(float) * num_elements);
    assert(rand_nums != NULL);
    int i;
    for (i = 0; i < num_elements; i++) {
        rand_nums[i] = "abcde"[((rand() / (float)RAND_MAX))%5];
    }
    return rand_nums;
}

// Computes the average of an array of numbers
float remove_a(char *array, int num_elements) {
    float sum = 0.f;
    int i;
    for (i = 0; i < num_elements; i++) {
        if (array[i]=='a'){
            array[i] = 'z';
            sum++;
        }
    }
    return sum;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: avg num_elements_per_proc\n");
        exit(1);
    }
    std::cout<<"t least here 0"<<std::endl;
    int num_elements_per_proc = atoi(argv[1]);
    // Seed the random number generator to get different results each time
    srand(time(NULL));

    MPI_Init(NULL, NULL);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Create a random array of elements on the root process. Its total
    // size will be the number of elements per process times the number
    // of processes
    char *rand_chars = NULL;
    if (world_rank == 0) {
        rand_chars = create_rand_chars(num_elements_per_proc * world_size);
    }

    // For each process, create a buffer that will hold a subset of the entire
    // array
    char *sub_rand_chars = (char *)malloc(sizeof(char) * num_elements_per_proc);
    assert(sub_rand_chars != NULL);

    // Scatter the random numbers from the root process to all processes in
    // the MPI world
    MPI_Scatter(rand_nums, num_elements_per_proc, MPI_CHAR, sub_rand_chars,
                num_elements_per_proc, MPI_CHAR, 0, MPI_COMM_WORLD);

    // Compute the average of your subset
    float sub_avg = remove_a(sub_rand_chars, num_elements_per_proc);

    // Gather all partial averages down to the root process
    float *sub_avgs = NULL;
    if (world_rank == 0) {
        sub_avgs = (float *)malloc(sizeof(float) * world_size);
        assert(sub_avgs != NULL);
    }
    MPI_Gather(&sub_avg, 1, MPI_CHAR, sub_avgs, 1, MPI_CHAR, 0, MPI_COMM_WORLD);

    // Now that we have all of the partial averages on the root, compute the
    // total average of all numbers. Since we are assuming each process computed
    // an average across an equal amount of elements, this computation will
    // produce the correct answer.
    std::cout<< "got here 1"<<std::endl;
    if (world_rank == 0) {
        for (int i = 0; i< sub_avgs.length(); i++)
        {
            std<<cout<<" output:"<<sub_avgs[i]<<std::endl;
        }
       // float avg = compute_avg(sub_avgs, world_size);
        //printf("Avg of all elements is %f\n", avg);
        // Compute the average across the original data for comparison
        //float original_data_avg =
        //        compute_avg(rand_nums, num_elements_per_proc * world_size);
       // printf("Avg computed across original data is %f\n", original_data_avg);
    }
    std::cout<< "got here 2"<<std::endl;
    // Clean up
    if (world_rank == 0) {
        free(rand_nums);
        free(sub_avgs);
    }
    free(sub_rand_chars);

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}