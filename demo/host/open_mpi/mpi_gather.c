
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <assert.h>


int main(int argc, char** argv) {
  if (argc != 3) {
    fprintf(stderr, "Usage: data_size_KB num_trials\n");
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

  // printf("Total bytes:%d, total num_elements:%d, num_trials:%d\n", bytes, num_elements, num_trials);

  MPI_Init(NULL, NULL);

  // Get the name of the processor
  char processor_name[MPI_MAX_PROCESSOR_NAME];
  int name_len;
  MPI_Get_processor_name(processor_name, &name_len);

  printf("%s, bytes:%d, num_elements:%d, num_trials:%d\n", processor_name, bytes, num_elements, num_trials);

  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  int size;
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  double total_my_Gather_time = 0.0;
  double total_mpi_Gather_time = 0.0;
  int i;
  int* data = (int*)malloc(sizeof(int) * num_elements);
  int* result = (int*)malloc(sizeof(int) * num_elements);
  assert(data != NULL);
  assert(result != NULL);

  int send_count = num_elements/size;
  int recv_count = send_count;
  

  
  double starttime, endtime, duration;

  for (i = 0; i < num_trials+1; i++) {

    // Time MPI_Gather
    MPI_Barrier(MPI_COMM_WORLD);
    starttime = MPI_Wtime();
    MPI_Gather(data, send_count, MPI_INT, result, recv_count, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    endtime = MPI_Wtime();
    duration = endtime - starttime;
    if(i>=1)
    {
      total_mpi_Gather_time = total_mpi_Gather_time + duration;
    }
    
    if(world_rank==0)
    {
	    printf("iteration:%d, time[s]:%f\n",i, duration);
    }
  }

  // Print off timing information
  if (world_rank == 0) {
    printf("Gather send element count:%d, byte count:%ld, trials:%d, ", send_count, send_count*sizeof(int), num_trials);
    printf("Avg MPI_Gather time [us]= *%lf\n", total_mpi_Gather_time*1000000.0 / num_trials);
  }

  free(data);
  MPI_Finalize();
}