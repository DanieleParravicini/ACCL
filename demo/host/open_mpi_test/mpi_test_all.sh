ele=(1 2 4 8 16 32 64 128 256 512 1024 2048 4096 8192 16384 32768)

# for i in "${ele[@]}"  
# do
#     for n in 2
#     do
#         /mnt/scratch/dkoutsou/openmpi/bin/mpirun -np $n --hostfile hostfile_alveo ./mpi_sendrecv $i 1000 | tee ./log/sendrecv/mpi_size_${i}_n_${n}_sendrecv.txt 
#     done
# done

for i in "${ele[@]}"  
do
    for n in {5..8}
    do
        /mnt/scratch/dkoutsou/openmpi/bin/mpirun -np $n --hostfile hostfile_alveo ./mpi_scatter $i 1000 | tee ./log/scatter/mpi_size_${i}_n_${n}_scatter.txt 
    done
done

for i in "${ele[@]}"  
do
    for n in {5..8}
    do
        /mnt/scratch/dkoutsou/openmpi/bin/mpirun -np $n --hostfile hostfile_alveo ./mpi_broadcast $i 1000 | tee ./log/broadcast/mpi_size_${i}_n_${n}_broadcast.txt 
    done
done

for i in "${ele[@]}"  
do
    for n in {5..8}
    do
        /mnt/scratch/dkoutsou/openmpi/bin/mpirun -np $n --hostfile hostfile_alveo ./mpi_gather $i 1000 | tee ./log/gather/mpi_size_${i}_n_${n}_gather.txt 
    done
done

for i in "${ele[@]}"  
do
    for n in {5..8}
    do
        /mnt/scratch/dkoutsou/openmpi/bin/mpirun -np $n --hostfile hostfile_alveo ./mpi_all_gather $i 1000 | tee ./log/all_gather/mpi_size_${i}_n_${n}_all_gather.txt 
    done
done


for i in "${ele[@]}"  
do
    for n in {5..8}
    do
        /mnt/scratch/dkoutsou/openmpi/bin/mpirun -np $n --hostfile hostfile_alveo ./mpi_reduce $i 1000 | tee ./log/reduce/mpi_size_${i}_n_${n}_reduce.txt 
    done
done

for i in "${ele[@]}"  
do
    for n in {5..8}
    do
        /mnt/scratch/dkoutsou/openmpi/bin/mpirun -np $n --hostfile hostfile_alveo ./mpi_all_reduce $i 1000 | tee ./log/all_reduce/mpi_size_${i}_n_${n}_all_reduce.txt 
    done
done