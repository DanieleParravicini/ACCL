#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <assert.h>

int main(int argc, char** argv) {
  if (argc != 3) {
    fprintf(stderr, "Usage:  data_size_KB num_trials\n");
    exit(1);
  }

  int bytes = 1024;
  int num_elements = bytes/sizeof(int);
  if (argc >= 2)
    {
        bytes = 1024 * strtol(argv[1], NULL, 10);
        num_elements = bytes/sizeof(int); 
    }
  
  int num_trials = 1;
  if (argc >= 3)
  {
      num_trials = strtol(argv[2], NULL, 10);
  } 

  // printf("bytes:%d, num_elements:%d, num_trials:%d\n", bytes, num_elements, num_trials);

  MPI_Init(NULL, NULL);

  // Get the name of the processor
  char processor_name[MPI_MAX_PROCESSOR_NAME];
  int name_len;
  MPI_Get_processor_name(processor_name, &name_len);

  printf("%s, bytes:%d, num_elements:%d, num_trials:%d\n", processor_name, bytes, num_elements, num_trials);

  int world_rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  double total_mpi_time = 0.0;
  int i;
  int fnderr=0;;

    int *in, *out, *sol;

    in = (int *)malloc( num_elements * sizeof(int) );
    out = (int *)malloc( num_elements * sizeof(int) );
    sol = (int *)malloc( num_elements * sizeof(int) );
    for (i=0; i<num_elements; i++)
    {
        *(in + i) = i;
        *(sol + i) = i*size;
        *(out + i) = 0;
    }
  
  double starttime, endtime, duration;

  //first round to warm up, not included in time counting
  for (i = 0; i < num_trials+1; i++) {


    // Time All-reduce
    MPI_Barrier(MPI_COMM_WORLD);
    starttime = MPI_Wtime();
    MPI_Allreduce( in, out, num_elements, MPI_INT, MPI_SUM, MPI_COMM_WORLD );
    MPI_Barrier(MPI_COMM_WORLD);
    endtime = MPI_Wtime();
    duration = endtime - starttime;

    if(i>=1)
    {
      total_mpi_time = total_mpi_time + duration;
    }
    
    if(world_rank==0)
    {
	    printf("iteration:%d, time[s]:%f\n",i, duration);
    }
  }

  // Print off timing information
  if (world_rank == 0) {
    printf("Data size = %d, Trials = %d\n", num_elements * (int)sizeof(int),
           num_trials);
    printf("Avg MPI_all_reduce time [us]= *%lf\n", total_mpi_time*1000000.0 / num_trials);
  }

    free( in );
    free( out );
    free( sol );
    MPI_Finalize();
}

