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

# Add dma_mover debug
set_property HDL_ATTRIBUTE.DEBUG true [ \
    get_bd_intf_nets { \
    control/dma_harden/microblaze_0_axi_periph_M06_AXI1 \
    control/dma_harden/fifo_tcp_packetizer_sts_M_AXIS \
    control/dma_harden/fifo_udp_packetizer_sts_M_AXIS \
    control/dma_harden/fifo_dma1_s2mm_sts_M_AXIS \
    control/dma_harden/fifo_dma0_mm2s_sts_M_AXIS \
    control/dma_harden/fifo_dma1_mm2s_sts_M_AXIS \
    control/dma_harden/dma_mover_0_return_stream_V \
    control/dma_harden/dma_mover_0_DMA0_RX_CMD_V \
    control/dma_harden/dma_mover_0_DMA1_RX_CMD_V \
    control/dma_harden/dma_mover_0_DMA1_TX_CMD_V \
    control/dma_harden/dma_mover_0_TCP_PKT_CMD_V \
    control/dma_harden/dma_mover_0_UDP_PKT_CMD_V \
    control/dma_harden/dma_mover_0_m_axi_mem \
    control/dma_harden/axis_dwidth_converter_0_M_AXIS \
    control/dma_harden/exchange_memory_bypass_1} \
]


create_bd_cell -type ip -vlnv xilinx.com:ip:system_ila:1.1 control/dma_harden/system_ila_dma_mover
set_property -dict [ list \
 CONFIG.C_SLOT {1} \
 CONFIG.C_BRAM_CNT {47} \
 CONFIG.C_NUM_MONITOR_SLOTS {15} \
 CONFIG.C_SLOT_1_INTF_TYPE {xilinx.com:interface:axis_rtl:1.0} \
 CONFIG.C_SLOT_2_INTF_TYPE {xilinx.com:interface:axis_rtl:1.0} \
 CONFIG.C_SLOT_3_INTF_TYPE {xilinx.com:interface:axis_rtl:1.0} \
 CONFIG.C_SLOT_4_INTF_TYPE {xilinx.com:interface:axis_rtl:1.0} \
 CONFIG.C_SLOT_5_INTF_TYPE {xilinx.com:interface:axis_rtl:1.0} \
 CONFIG.C_SLOT_6_INTF_TYPE {xilinx.com:interface:axis_rtl:1.0} \
 CONFIG.C_SLOT_7_INTF_TYPE {xilinx.com:interface:axis_rtl:1.0} \
 CONFIG.C_SLOT_8_INTF_TYPE {xilinx.com:interface:axis_rtl:1.0} \
 CONFIG.C_SLOT_9_INTF_TYPE {xilinx.com:interface:axis_rtl:1.0} \
 CONFIG.C_SLOT_10_INTF_TYPE {xilinx.com:interface:axis_rtl:1.0} \
 CONFIG.C_SLOT_11_INTF_TYPE {xilinx.com:interface:axis_rtl:1.0} \
 CONFIG.C_SLOT_12_INTF_TYPE {xilinx.com:interface:axis_rtl:1.0} \
] [get_bd_cells control/dma_harden/system_ila_dma_mover]

connect_bd_net [get_bd_pins control/dma_harden/system_ila_dma_mover/clk   ] [get_bd_pins control/dma_harden/ap_clk          ] 
connect_bd_net [get_bd_pins control/dma_harden/system_ila_dma_mover/resetn] [get_bd_pins control/dma_harden/encore_aresetn  ]

connect_bd_intf_net [get_bd_intf_pins control/dma_harden/s_axi_control2         ] [get_bd_intf_pins control/dma_harden/system_ila_dma_mover/SLOT_0_AXI]
connect_bd_intf_net [get_bd_intf_pins control/dma_harden/DMA0_RX_STS_V          ] [get_bd_intf_pins control/dma_harden/system_ila_dma_mover/SLOT_1_AXIS]
connect_bd_intf_net [get_bd_intf_pins control/dma_harden/DMA1_RX_STS_V          ] [get_bd_intf_pins control/dma_harden/system_ila_dma_mover/SLOT_2_AXIS]
connect_bd_intf_net [get_bd_intf_pins control/dma_harden/DMA1_TX_STS_V          ] [get_bd_intf_pins control/dma_harden/system_ila_dma_mover/SLOT_3_AXIS]
connect_bd_intf_net [get_bd_intf_pins control/dma_harden/UDP_PKT_STS_V          ] [get_bd_intf_pins control/dma_harden/system_ila_dma_mover/SLOT_4_AXIS]
connect_bd_intf_net [get_bd_intf_pins control/dma_harden/TCP_PKT_STS_V          ] [get_bd_intf_pins control/dma_harden/system_ila_dma_mover/SLOT_5_AXIS]
connect_bd_intf_net [get_bd_intf_pins control/dma_harden/dma_mover_0/pkt_stream_V] [get_bd_intf_pins control/dma_harden/system_ila_dma_mover/SLOT_6_AXIS]
connect_bd_intf_net [get_bd_intf_pins control/dma_harden/DMA0_RX_CMD_V          ] [get_bd_intf_pins control/dma_harden/system_ila_dma_mover/SLOT_7_AXIS]
connect_bd_intf_net [get_bd_intf_pins control/dma_harden/DMA1_RX_CMD_V          ] [get_bd_intf_pins control/dma_harden/system_ila_dma_mover/SLOT_8_AXIS]
connect_bd_intf_net [get_bd_intf_pins control/dma_harden/DMA1_TX_CMD_V          ] [get_bd_intf_pins control/dma_harden/system_ila_dma_mover/SLOT_9_AXIS]
connect_bd_intf_net [get_bd_intf_pins control/dma_harden/UDP_PKT_CMD_V          ] [get_bd_intf_pins control/dma_harden/system_ila_dma_mover/SLOT_10_AXIS]
connect_bd_intf_net [get_bd_intf_pins control/dma_harden/TCP_PKT_CMD_V          ] [get_bd_intf_pins control/dma_harden/system_ila_dma_mover/SLOT_11_AXIS]
connect_bd_intf_net [get_bd_intf_pins control/dma_harden/TCP_PKT_CMD_V          ] [get_bd_intf_pins control/dma_harden/system_ila_dma_mover/SLOT_11_AXIS]
connect_bd_intf_net [get_bd_intf_pins control/dma_harden/dma_mover_0/return_stream_V] [get_bd_intf_pins control/dma_harden/system_ila_dma_mover/SLOT_12_AXIS]
connect_bd_intf_net [get_bd_intf_pins control/dma_harden/dma_memory_ic/S02_AXI  ] [get_bd_intf_pins control/dma_harden/system_ila_dma_mover/SLOT_13_AXI]
connect_bd_intf_net [get_bd_intf_pins control/dma_harden/dma_memory_ic/M00_AXI  ] [get_bd_intf_pins control/dma_harden/system_ila_dma_mover/SLOT_14_AXI]

save_bd_design
validate_bd_design