#!/bin/bash
ele=(1 2 4 8 16 32 64 128 256 512 1024 2048 4096 8192 16384 32768)

for i in "${ele[@]}"  
do
    for n in 2
    do
        /mnt/scratch/dkoutsou/openmpi/bin/mpirun -np $n --hostfile hostfile_alveo ./mpi_sendrecv $i 1000 | tee ./log/sendrecv/mpi_size_${i}_n_${n}_sendrecv.txt 
    done
done