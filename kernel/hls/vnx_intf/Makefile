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

DEVICE=xcu250-figd2104-2L-e
PACKETIZER_IP=build_vnx_packetizer/sol1/impl/ip/xilinx_com_hls_vnx_packetizer_1_0.zip
DEPACKETIZER_IP=build_vnx_depacketizer/sol1/impl/ip/xilinx_com_hls_vnx_depacketizer_1_0.zip
TARGET=ip

all: $(PACKETIZER_IP) $(DEPACKETIZER_IP)

$(PACKETIZER_IP): build_vnx_packetizer.tcl vnx_packetizer.cpp
	vitis_hls $< -tclargs $(TARGET) $(DEVICE)

$(DEPACKETIZER_IP): build_vnx_depacketizer.tcl vnx_depacketizer.cpp
	vitis_hls $< -tclargs $(TARGET) $(DEVICE)

