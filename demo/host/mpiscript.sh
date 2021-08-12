# /*******************************************************************************
#  Copyright (C) 2021 Xilinx, Inc
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#
# *******************************************************************************/

source /opt/xilinx/xrt/setup.sh
source /opt/tools/Xilinx/Vitis/2020.2/.settings64-Vitis.sh
source /opt/tools/external/anaconda/bin/activate pynq-dask

#python test_mpi4py.py 
cd ~/ACCL/demo/host
python test_tcp_cmac_seq_mpi.py --xclbin ../build/tcp/ccl_offload.xclbin --device 0 --experiment test1 --nruns 20 --nbufs 32 --bsize 256 --segment_size 128 512 1024 --send --bcast --scatter --gather --reduce --allgather --allreduce
