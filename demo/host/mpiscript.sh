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
