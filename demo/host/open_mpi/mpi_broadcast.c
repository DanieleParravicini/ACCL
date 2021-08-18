
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <assert.h>

void my_bcast(void* data, int count, MPI_Datatype datatype, int root,
              MPI_Comm communicator) {
  int world_rank;
  MPI_Comm_rank(communicator, &world_rank);
  int world_size;
  MPI_Comm_size(communicator, &world_size);

  if (world_rank == root) {
    // If we are the root process, send our data to everyone
    int i;
    for (i = 0; i < world_size; i++) {
      if (i != world_rank) {
        MPI_Send(data, count, datatype, i, 0, communicator);
      }
    }
  } else {
    // If we are a receiver process, receive the data from the root
    MPI_Recv(data, count, datatype, root, 0, communicator, MPI_STATUS_IGNORE);
  }
}

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

  
  MPI_Init(NULL, NULL);

  // Get the name of the processor
  char processor_name[MPI_MAX_PROCESSOR_NAME];
  int name_len;
  MPI_Get_processor_name(processor_name, &name_len);

  printf("%s, bytes:%d, num_elements:%d, num_trials:%d\n", processor_name, bytes, num_elements, num_trials);
  
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  double total_my_bcast_time = 0.0;
  double total_mpi_bcast_time = 0.0;
  int i;

  int* data = (int*)malloc(sizeof(int) * num_elements);
  assert(data != NULL);

  double starttime, endtime, duration;

  for (i = 0; i < num_trials+1; i++) {
    
    // Time MPI_Bcast
    MPI_Barrier(MPI_COMM_WORLD);
    starttime = MPI_Wtime();
    MPI_Bcast(data, num_elements, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    endtime = MPI_Wtime();
    duration = endtime - starttime;
    if(i>=1)
    {
      total_mpi_bcast_time = total_mpi_bcast_time + duration;
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
    double avg_duration = total_mpi_bcast_time*1000000.0 / num_trials;
    double throughput = (world_size-1) * bytes * 8 / (avg_duration * 1000.0);
    printf("Avg MPI_Bcast time [us]= *%lf, throughput [Gbps] = %lf\n", avg_duration, throughput);
  }

  
  MPI_Finalize();
}
