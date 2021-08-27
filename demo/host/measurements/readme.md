# Prerequisite
1. ACCL: move csv created by test_tcp_cmac_seq_mpi.py under measurements/accl
2. OpenMPI from ../openmpi/log 
    a. remove measurements/openmpi : ```rm openmpi/*.csv```
    b. go into ../open_mpi/log:      ```cd ../openmpi/log```
    c. run convert_to_csv:           ```python convert_to_csv.py```
# How to make graphs out of measurements

To graph latency for openMPI vs ACCL use:   ```python create_graphs.py --openMPI```
To graph latency for all collectives at all buffer size available at different communicator size use:   ```python create_graphs.py --rank2```
