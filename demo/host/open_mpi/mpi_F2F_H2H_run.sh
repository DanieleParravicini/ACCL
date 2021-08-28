# ele=(1 2 4 8 16 32 64 128 256 512 1024 2048 4096 8192 16384 32768)
ele=(1 2 4 8 16 32 64 128 256 512 1024 2048 4096 8192 16384 32768)
numrun=1000

for i in "${ele[@]}"  
do
    for n in 2
    do
        /mnt/scratch/zhe/openmpi/openmpi-4.1.1/bin/mpiexec --mca btl_tcp_links 4 -np $n --hostfile hostfile_alveo -- bash mpi_F2F_H2H_bash.sh $i $numrun 1 | tee ./log/sendrecv/mpi_4_1_mc_F2F_H2H_size_${i}_n_${n}_sendrecv.txt 
    done
done

 for i in "${ele[@]}"  
 do
     for n in {3..8}
     do
         /mnt/scratch/zhe/openmpi/openmpi-4.1.1/bin/mpiexec --mca btl_tcp_links 4 -np $n --hostfile hostfile_alveo -- bash mpi_F2F_H2H_bash.sh $i $numrun 2 | tee ./log/broadcast/mpi_4_1_mc_F2F_H2H_size_${i}_n_${n}_broadcast.txt 
     done
 done

 for i in "${ele[@]}"  
 do
     for n in {3..8}
     do
         /mnt/scratch/zhe/openmpi/openmpi-4.1.1/bin/mpiexec --mca btl_tcp_links 4 -np $n --hostfile hostfile_alveo -- bash mpi_F2F_H2H_bash.sh $i $numrun 3 | tee ./log/scatter/mpi_4_1_mc_F2F_H2H_size_${i}_n_${n}_scatter.txt 
     done
 done

 for i in "${ele[@]}"  
 do
     for n in {3..8}
     do
         /mnt/scratch/zhe/openmpi/openmpi-4.1.1/bin/mpiexec --mca btl_tcp_links 4 -np $n --hostfile hostfile_alveo -- bash mpi_F2F_H2H_bash.sh $i $numrun 4 | tee ./log/reduce/mpi_4_1_mc_F2F_H2H_size_${i}_n_${n}_reduce.txt 
     done
 done

 for i in "${ele[@]}"  
 do
     for n in {3..8}
     do
         /mnt/scratch/zhe/openmpi/openmpi-4.1.1/bin/mpiexec --mca btl_tcp_links 4 -np $n --hostfile hostfile_alveo -- bash mpi_F2F_H2H_bash.sh $i $numrun 5 | tee ./log/all_reduce/mpi_4_1_mc_F2F_H2H_size_${i}_n_${n}_all_reduce.txt 
     done
 done

 for i in "${ele[@]}"  
 do
     for n in {3..8}
     do
         /mnt/scratch/zhe/openmpi/openmpi-4.1.1/bin/mpiexec --mca btl_tcp_links 4 -np $n --hostfile hostfile_alveo -- bash mpi_F2F_H2H_bash.sh $i $numrun 6 | tee ./log/gather/mpi_4_1_mc_F2F_H2H_size_${i}_n_${n}_gather.txt 
     done
 done

 for i in "${ele[@]}"  
 do
     for n in {3..8}
     do
         /mnt/scratch/zhe/openmpi/openmpi-4.1.1/bin/mpiexec --mca btl_tcp_links 4  -np $n --hostfile hostfile_alveo -- bash mpi_F2F_H2H_bash.sh $i $numrun 7 | tee ./log/all_gather/mpi_4_1_mc_F2F_H2H_size_${i}_n_${n}_all_gather.txt 
     done
 done