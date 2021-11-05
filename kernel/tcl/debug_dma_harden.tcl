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

# Add dma debug
set_property HDL_ATTRIBUTE.DEBUG true [ \
    get_bd_intf_nets { \
    control/dma_harden/dma_enqueue_0_cmd_dma_tcp_V \
    control/dma_harden/dma_enqueue_0_cmd_dma_udp_V \
    control/dma_harden/dma_enqueue_0_inflight_queue_V \
    control/dma_harden/sts_header_cmd_switch_M01_AXIS \
    control/dma_harden/sts_header_cmd_switch_M05_AXIS \
    control/dma_harden/sts_header_cmd_switch_M03_AXIS \
    control/dma_harden/sts_header_cmd_switch_M07_AXIS \
    control/dma_harden/exchange_memory_bypass_1}\
]

create_bd_cell -type ip -vlnv xilinx.com:ip:system_ila:1.1 control/dma_harden/system_ila_dma_harden
set_property -dict [ list \
 CONFIG.C_SLOT {1} \
 CONFIG.C_BRAM_CNT {47} \
 CONFIG.C_NUM_MONITOR_SLOTS {8} \
 CONFIG.C_SLOT_1_INTF_TYPE {xilinx.com:interface:axis_rtl:1.0} \
 CONFIG.C_SLOT_2_INTF_TYPE {xilinx.com:interface:axis_rtl:1.0} \
 CONFIG.C_SLOT_3_INTF_TYPE {xilinx.com:interface:axis_rtl:1.0} \
 CONFIG.C_SLOT_4_INTF_TYPE {xilinx.com:interface:axis_rtl:1.0} \
 CONFIG.C_SLOT_5_INTF_TYPE {xilinx.com:interface:axis_rtl:1.0} \
 CONFIG.C_SLOT_6_INTF_TYPE {xilinx.com:interface:axis_rtl:1.0} \
 CONFIG.C_SLOT_7_INTF_TYPE {xilinx.com:interface:axis_rtl:1.0} \
] [get_bd_cells control/dma_harden/system_ila_dma_harden]

connect_bd_net [get_bd_pins control/dma_harden/system_ila_dma_harden/clk   ] [get_bd_pins control/dma_harden/ap_clk          ] 
connect_bd_net [get_bd_pins control/dma_harden/system_ila_dma_harden/resetn] [get_bd_pins control/dma_harden/encore_aresetn  ]

connect_bd_intf_net [get_bd_intf_pins control/dma_harden/system_ila_dma_harden/SLOT_0_AXI ] [get_bd_intf_pins control/dma_harden/dma_memory_ic/M00_AXI]
connect_bd_intf_net [get_bd_intf_pins control/dma_harden/system_ila_dma_harden/SLOT_1_AXIS] [get_bd_intf_pins control/dma_harden/dma_enqueue_0/cmd_dma_tcp_V]
connect_bd_intf_net [get_bd_intf_pins control/dma_harden/system_ila_dma_harden/SLOT_2_AXIS] [get_bd_intf_pins control/dma_harden/dma_enqueue_0/cmd_dma_udp_V]
connect_bd_intf_net [get_bd_intf_pins control/dma_harden/system_ila_dma_harden/SLOT_3_AXIS] [get_bd_intf_pins control/dma_harden/dma_enqueue_0/inflight_queue_V]
connect_bd_intf_net [get_bd_intf_pins control/dma_harden/system_ila_dma_harden/SLOT_4_AXIS] [get_bd_intf_pins control/dma_harden/dma_dequeue_0/sts_dma_udp_V]
connect_bd_intf_net [get_bd_intf_pins control/dma_harden/system_ila_dma_harden/SLOT_5_AXIS] [get_bd_intf_pins control/dma_harden/dma_dequeue_0/sts_dma_tcp_V]
connect_bd_intf_net [get_bd_intf_pins control/dma_harden/system_ila_dma_harden/SLOT_6_AXIS] [get_bd_intf_pins control/dma_harden/dma_dequeue_0/header_udp_V]
connect_bd_intf_net [get_bd_intf_pins control/dma_harden/system_ila_dma_harden/SLOT_7_AXIS] [get_bd_intf_pins control/dma_harden/dma_dequeue_0/header_tcp_V]

save_bd_design
validate_bd_design