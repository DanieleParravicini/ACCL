## Test your xclbin
To test your `.xclbin` on XACC do the following:
1. copy on XACC some of the ACCL directories maintaining the same structure as in the repo.
    ```
    ssh alveo0
    mkdir ACCL
    mkdir -p ACCL/demo/build/tcp_u280
    mkdir -p ACCL/demo/host/measurements/accl
    exit
    scp -r ACCL/demo/build/link*/ccl_offload.* alveo0:~/ACCL/demo/build/tcp_u280
    scp -r ACCL/driver alveo0:~/ACCL/
    scp -r ACCL/demo/host alveo0:~/ACCL/demo/
    ```
1.  login on one XACC node and go in the ``ACCL/demo/host``
1.  create  ``/measurements/accl`` directory where the ``.csv`` with the logs will be saved
1.  modify ``run_test_tcp.sh`` to target a number of XACC nodes

    ```
    #alveo3b 10.1.212.126
    #alveo3c 10.1.212.127
    #alveo4b 10.1.212.129
    #alveo4c 10.1.212.130 
    mpiexec --host 10.1.212.123,10.1.212.127,10.1.212.129,10.1.212.130 -- bash mpiscript.sh
    ```

1.  login to those hosts copying your ssh id 

    ```
    ssh-copy-id 10.1.212.123
    ssh-copy-id 10.1.212.127
    ssh-copy-id 10.1.212.129
    ssh-copy-id 10.1.212.130
    ```

1. when you run ``run_test_tcp.sh`` it will execute ``mpiscript.sh`` that will recall the python test script with the parameters. Example of ``mpiscript.sh``

    ```
    
    source /opt/xilinx/xrt/setup.sh
    source /opt/tools/Xilinx/Vitis/2020.2/.settings64-Vitis.sh
    source /opt/tools/external/anaconda/bin/activate pynq-dask

    declare -a collectives=("send" "bcast" "scatter" "gather" "allgather" "reduce" "allreduce")

    cd ~/ACCL/demo/host
    ele_consec=" 1 2 4 8 16 32 64 128 256 512 1024 2048 4096 8192 16384 32768 65536"
    segment_sizes=(512 1024 2048)
    numrun=10
    measures="experiment name"

    for segment_size in "${segment_sizes[@]}"
    do
        for col in "${collectives[@]}"
        do
            xbutil validate
            python test_tcp.py --xclbin ../tcp.xclbin --experiment $measures --device 0 --nbufs 40 --nruns $numrun --segment_size $segment_size --bsize $ele_consec --$col --use_tcp
        done
    done
    xbutil validate

    ```

1. the ``test_tcp.py`` will create a .``csv`` under ``./measurements/accl`` that logs the execution time for the collectives


