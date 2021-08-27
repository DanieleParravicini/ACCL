
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <assert.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
// #include "xlnx-comm.hpp"
// #include "xlnx-consts.hpp"

#include "experimental/xrt_aie.h"
#include "experimental/xrt_device.h"
#include "experimental/xrt_kernel.h"
#include <experimental/xrt_xclbin.h>

#define SENDRECV 1 
#define BCAST 2 
#define SCATTER 3
#define REDUCE 4
#define ALLREDUCE 5
#define GATHER 6
#define ALLGATHER 7

int main(int argc, char** argv) {
  if (argc != 4) {
    fprintf(stderr, "Usage: data_size_KB num_trials collective\n");
    exit(1);
  }

  MPI_Init(NULL, NULL);
  // Get the name of the processor
  char processor_name[MPI_MAX_PROCESSOR_NAME];
  int name_len;
  MPI_Get_processor_name(processor_name, &name_len);

  int bytes = 1024;
  int num_elements = bytes/sizeof(int);
  int num_trials = 1;
  int collective = SENDRECV;

  if (argc >= 2)
    {
        bytes = 1024 * strtol(argv[1], NULL, 10);
        num_elements = bytes/sizeof(int); 
    }
  
  
  if (argc >= 3)
  {
      num_trials = strtol(argv[2], NULL, 10);
  } 

  if (argc >= 4)
  {
      collective = strtol(argv[3], NULL, 10);
  } 

  unsigned int dev_index = 0;
  auto device = xrt::device(dev_index);
  // Do some checks to see if we're on the U280
  char *name;
  if (gethostname(name, 255) < 0) {
    perror("gethostname error");
    exit(1);
  }
  std::cout<<name<<std::endl;

  if( (strcmp(name,"alveo3b") == 0) | (strcmp(name,"alveo3c") == 0) | (strcmp(name,"alveo4b") == 0) | (strcmp(name,"alveo4c") == 0) ) {
    auto xclbin_uuid = device.load_xclbin("/home/zhe/XCCL/bitstream/ccl_offload_tcp_seq_cmac_12.xclbin");
    printf("%s, device on u280. Finished loading bitstream\n", name);
  }
  else if( (strcmp(name,"alveo1a") == 0) | (strcmp(name,"alveo1b") == 0) | (strcmp(name,"alveo2a") == 0) | (strcmp(name,"alveo2b") == 0) )
  {
    auto xclbin_uuid = device.load_xclbin("/home/zhe/XCCL/bitstream/ccl_offload_tcp_seq_cmac_u250_7.xclbin");
    printf("%s, device on u250. Finished loading bitstream\n", name);
  }

  printf("%s, bytes:%d, num_elements:%d, num_trials:%d\n", processor_name, bytes, num_elements, num_trials);
  
  int world_rank, size;
  int i;
  int fnderr=0;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int *in, *out, *sol;

  posix_memalign((void**)&in,4096,num_elements*sizeof(int));
  posix_memalign((void**)&out,4096,num_elements*sizeof(int));
  posix_memalign((void**)&sol,4096,num_elements*sizeof(int));

  for (i=0; i<num_elements; i++)
  {
      *(in + i) = i;
      *(sol + i) = i*size;
      *(out + i) = 0;
  }

  auto inbuf = xrt::bo (device, in, num_elements*sizeof(int), 0);
  auto outbuf = xrt::bo (device, out, num_elements*sizeof(int), 0);

  // printf("Finished allocating buffers\n");
  
  double starttime_total, endtime_total, duration_total;
  double starttime_com, endtime_com, duration_com;
  double starttime_sync, endtime_sync, duration_sync;
  double total_mpi_time = 0.0;
  double com_mpi_time = 0.0;
  double sync_mpi_time = 0.0;

  if(collective == SENDRECV)
  {
    if(world_rank == 0)
      printf("Start Send/Recv test:\n");
    //first round to warm up, not included in time counting
    for (i = 0; i < num_trials+1; i++) 
    {
      //initialize the input buffer/output buffer with different values
      for (int j=0; j<num_elements; j++)
      {
        *(in + j) = i;
        *(out + j) = i;
      }
      // Time Send Recv
      MPI_Barrier(MPI_COMM_WORLD);
      starttime_total = MPI_Wtime();

      if (world_rank == 0)
      {
        inbuf.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
      }

      MPI_Barrier(MPI_COMM_WORLD);
      starttime_com = MPI_Wtime();
      if (world_rank == 0)
        MPI_Send( in, num_elements, MPI_INT, 1, 0, MPI_COMM_WORLD );
      else if (world_rank == 1)
        MPI_Recv( out, num_elements, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      MPI_Barrier(MPI_COMM_WORLD);
      endtime_com = MPI_Wtime();

      if (world_rank != 0)
      {
        outbuf.sync(XCL_BO_SYNC_BO_TO_DEVICE);
      }

      MPI_Barrier(MPI_COMM_WORLD);
      endtime_total = MPI_Wtime();
      duration_total = endtime_total - starttime_total;
      duration_com = endtime_com - starttime_com;
      duration_sync = duration_total - duration_com;

      if(i>=1)
      {
        total_mpi_time = total_mpi_time + duration_total;
        com_mpi_time = com_mpi_time + duration_com;
        sync_mpi_time = sync_mpi_time + duration_sync;
      }
      
      if(world_rank==0)
      {
        printf("iteration:%d, F2F_time[s]:%f, H2H_time:%f, sync_time:%f\n",i, duration_total, duration_com, duration_sync);
      }
    }
  }
  else if (collective == SCATTER)
  {
    if(world_rank == 0)
      printf("Start Scatter test:\n");
    int send_count = num_elements/size;
    int recv_count = send_count;
    for (i = 0; i < num_trials+1; i++) 
    {
      //initialize the input buffer/output buffer with different values
      for (int j=0; j<num_elements; j++)
      {
        *(in + j) = i;
        *(out + j) = i;
      }
      // Time MPI_Scatter
      MPI_Barrier(MPI_COMM_WORLD);
      starttime_total = MPI_Wtime();
      if(world_rank == 0)
      {
        // Sync Bytes of the whole world
        inbuf.sync(XCL_BO_SYNC_BO_FROM_DEVICE, bytes, 0);
      }

      MPI_Barrier(MPI_COMM_WORLD);
      starttime_com = MPI_Wtime();
      MPI_Scatter(in, send_count, MPI_INT, out, recv_count, MPI_INT, 0, MPI_COMM_WORLD);
      MPI_Barrier(MPI_COMM_WORLD);
      endtime_com = MPI_Wtime();

      if(world_rank != 0)
      {
        outbuf.sync(XCL_BO_SYNC_BO_TO_DEVICE, bytes/size, 0);
      }
      MPI_Barrier(MPI_COMM_WORLD);
      endtime_total = MPI_Wtime();
      duration_total = endtime_total - starttime_total;
      duration_com = endtime_com - starttime_com;
      duration_sync = duration_total - duration_com;

      if(i>=1)
      {
        total_mpi_time = total_mpi_time + duration_total;
        com_mpi_time = com_mpi_time + duration_com;
        sync_mpi_time = sync_mpi_time + duration_sync;
      }
      
      if(world_rank==0)
      {
        printf("iteration:%d, F2F_time[s]:%f, H2H_time:%f, sync_time:%f\n",i, duration_total, duration_com, duration_sync);
      }
    }
  }
  else if (collective == BCAST)
  {
    if(world_rank == 0)
      printf("Start Bcast test:\n");
    for (i = 0; i < num_trials+1; i++) 
    {
      //initialize the input buffer/output buffer with different values
      for (int j=0; j<num_elements; j++)
      {
        *(in + j) = i;
        *(out + j) = i;
      }
      // Time MPI_Bcast
      MPI_Barrier(MPI_COMM_WORLD);
      starttime_total = MPI_Wtime();
      if (world_rank == 0)
      {
        inbuf.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
      }

      MPI_Barrier(MPI_COMM_WORLD);
      starttime_com = MPI_Wtime();
      MPI_Bcast(in, num_elements, MPI_INT, 0, MPI_COMM_WORLD);
      MPI_Barrier(MPI_COMM_WORLD);
      endtime_com = MPI_Wtime();

      if(world_rank != 0) 
      {
        outbuf.sync(XCL_BO_SYNC_BO_TO_DEVICE);
      }
      
      MPI_Barrier(MPI_COMM_WORLD);
      endtime_total = MPI_Wtime();
      duration_total = endtime_total - starttime_total;
      duration_com = endtime_com - starttime_com;
      duration_sync = duration_total - duration_com;

      if(i>=1)
      {
        total_mpi_time = total_mpi_time + duration_total;
        com_mpi_time = com_mpi_time + duration_com;
        sync_mpi_time = sync_mpi_time + duration_sync;
      }
      
      if(world_rank==0)
      {
        printf("iteration:%d, F2F_time[s]:%f, H2H_time:%f, sync_time:%f\n",i, duration_total, duration_com, duration_sync);
      }
    }
  }
  else if (collective == REDUCE)
  {
    if(world_rank == 0)
      printf("Start Reduce test:\n");
    //first round to warm up, not included in time counting
    for (i = 0; i < num_trials+1; i++) 
    {
      //initialize the input buffer/output buffer with different values
      for (int j=0; j<num_elements; j++)
      {
        *(in + j) = i;
        *(out + j) = i;
      }
      // Time Reduce
      MPI_Barrier(MPI_COMM_WORLD);
      starttime_total = MPI_Wtime();

      inbuf.sync(XCL_BO_SYNC_BO_FROM_DEVICE);

      MPI_Barrier(MPI_COMM_WORLD);
      starttime_com = MPI_Wtime();
      MPI_Reduce( in, out, num_elements, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD );
      MPI_Barrier(MPI_COMM_WORLD);
      endtime_com = MPI_Wtime();

      if(world_rank == 0)
        outbuf.sync(XCL_BO_SYNC_BO_TO_DEVICE);

      MPI_Barrier(MPI_COMM_WORLD);
      endtime_total = MPI_Wtime();
      duration_total = endtime_total - starttime_total;
      duration_com = endtime_com - starttime_com;
      duration_sync = duration_total - duration_com;

      if(i>=1)
      {
        total_mpi_time = total_mpi_time + duration_total;
        com_mpi_time = com_mpi_time + duration_com;
        sync_mpi_time = sync_mpi_time + duration_sync;
      }
      
      if(world_rank==0)
      {
        printf("iteration:%d, F2F_time[s]:%f, H2H_time:%f, sync_time:%f\n",i, duration_total, duration_com, duration_sync);
      }
    }
  }
  else if (collective == ALLREDUCE)
  { 
    if(world_rank == 0)
      printf("Start All-reduce test:\n");
    //first round to warm up, not included in time counting
    for (i = 0; i < num_trials+1; i++) 
    {
      //initialize the input buffer/output buffer with different values
      for (int j=0; j<num_elements; j++)
      {
        *(in + j) = i;
        *(out + j) = i;
      }
      // Time All-reduce
      MPI_Barrier(MPI_COMM_WORLD);
      starttime_total = MPI_Wtime();

      inbuf.sync(XCL_BO_SYNC_BO_FROM_DEVICE);

      MPI_Barrier(MPI_COMM_WORLD);
      starttime_com = MPI_Wtime();
      MPI_Allreduce( in, out, num_elements, MPI_INT, MPI_SUM, MPI_COMM_WORLD );
      MPI_Barrier(MPI_COMM_WORLD);
      endtime_com = MPI_Wtime();

      outbuf.sync(XCL_BO_SYNC_BO_TO_DEVICE);

      MPI_Barrier(MPI_COMM_WORLD);
      endtime_total = MPI_Wtime();
      duration_total = endtime_total - starttime_total;
      duration_com = endtime_com - starttime_com;
      duration_sync = duration_total - duration_com;

      if(i>=1)
      {
        total_mpi_time = total_mpi_time + duration_total;
        com_mpi_time = com_mpi_time + duration_com;
        sync_mpi_time = sync_mpi_time + duration_sync;
      }
      
      if(world_rank==0)
      {
        printf("iteration:%d, F2F_time[s]:%f, H2H_time:%f, sync_time:%f\n",i, duration_total, duration_com, duration_sync);
      }
    }   
  }
  else if (collective == GATHER)
  {
    int send_count = num_elements/size;
    int recv_count = send_count;
    if(world_rank == 0)
      printf("Start Gather test:\n");
    for (i = 0; i < num_trials+1; i++) 
    {
      //initialize the input buffer/output buffer with different values
      for (int j=0; j<num_elements; j++)
      {
        *(in + j) = i;
        *(out + j) = i;
      }
      // Time MPI_Gather
      MPI_Barrier(MPI_COMM_WORLD);
      starttime_total = MPI_Wtime();
      //sync bytes per process
      inbuf.sync(XCL_BO_SYNC_BO_FROM_DEVICE, bytes/size, 0);

      MPI_Barrier(MPI_COMM_WORLD);
      starttime_com = MPI_Wtime();
      MPI_Gather(in, send_count, MPI_INT, out, recv_count, MPI_INT, 0, MPI_COMM_WORLD);
      MPI_Barrier(MPI_COMM_WORLD);
      endtime_com = MPI_Wtime();

      if (world_rank == 0)
        outbuf.sync(XCL_BO_SYNC_BO_TO_DEVICE);

      MPI_Barrier(MPI_COMM_WORLD);
      endtime_total = MPI_Wtime();
      duration_total = endtime_total - starttime_total;
      duration_com = endtime_com - starttime_com;
      duration_sync = duration_total - duration_com;

      if(i>=1)
      {
        total_mpi_time = total_mpi_time + duration_total;
        com_mpi_time = com_mpi_time + duration_com;
        sync_mpi_time = sync_mpi_time + duration_sync;
      }
      
      if(world_rank==0)
      {
        printf("iteration:%d, F2F_time[s]:%f, H2H_time:%f, sync_time:%f\n",i, duration_total, duration_com, duration_sync);
      }
    }
  }
  else if (collective == ALLGATHER)
  {
    int send_count = num_elements/size;
    int recv_count = send_count;
    if(world_rank == 0)
      printf("Start All-gather test:\n");
    for (i = 0; i < num_trials+1; i++) {

      //initialize the input buffer/output buffer with different values
      for (int j=0; j<num_elements; j++)
      {
        *(in + j) = i;
        *(out + j) = i;
      }
      // Time MPI_Allgather
      MPI_Barrier(MPI_COMM_WORLD);
      starttime_total = MPI_Wtime();

      inbuf.sync(XCL_BO_SYNC_BO_FROM_DEVICE);

      MPI_Barrier(MPI_COMM_WORLD);
      starttime_com = MPI_Wtime();
      MPI_Allgather(in, send_count, MPI_INT, out, recv_count, MPI_INT, MPI_COMM_WORLD);
      MPI_Barrier(MPI_COMM_WORLD);
      endtime_com = MPI_Wtime();
      
      outbuf.sync(XCL_BO_SYNC_BO_TO_DEVICE);
      MPI_Barrier(MPI_COMM_WORLD);
      endtime_total = MPI_Wtime();
      duration_total = endtime_total - starttime_total;
      duration_com = endtime_com - starttime_com;
      duration_sync = duration_total - duration_com;

      if(i>=1)
      {
        total_mpi_time = total_mpi_time + duration_total;
        com_mpi_time = com_mpi_time + duration_com;
        sync_mpi_time = sync_mpi_time + duration_sync;
      }
      
      if(world_rank==0)
      {
        printf("iteration:%d, F2F_time[s]:%f, H2H_time:%f, sync_time:%f\n",i, duration_total, duration_com, duration_sync);
      }
    }
  }
  
  // Print off timing information
  if (world_rank == 0) {
    printf("Data size = %d, Trials = %d\n", num_elements * (int)sizeof(int),
           num_trials);
    double avg_duration_total = total_mpi_time*1000000.0 / num_trials;
    double avg_duration_com = com_mpi_time*1000000.0 / num_trials;
    double avg_duration_sync = sync_mpi_time*1000000.0 / num_trials;
    // double F2F_throughput = bytes * 8 / (avg_duration_total * 1000.0);
    double H2H_throughput = bytes * 8 / (avg_duration_com * 1000.0);
    printf("Avg MPI F2F time [us]= *%lf, H2H time= %lf, sync time= %lf, H2H throughput [Gbps] = %lf\n", avg_duration_total, avg_duration_com, avg_duration_sync, H2H_throughput);
  }

  free( in );
  free( out );
  free( sol );
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
}
