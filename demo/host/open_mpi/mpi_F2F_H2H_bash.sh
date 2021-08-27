source /opt/xilinx/xrt/setup.sh
source /opt/tools/Xilinx/Vitis/2020.2/.settings64-Vitis.sh
source /opt/tools/external/anaconda/bin/activate pynq-dask

#$1 data size KB
#$2 number of runs
#$3 collective code

./mpi_F2F_H2H $1 $2 $3 