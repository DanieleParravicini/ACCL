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
    axis_switch_0/axis_subset_converter_0_M_AXIS \
    axis_switch_0/axis_subset_converter_1_M_AXIS \
    axis_switch_0/axis_subset_converter_2_M_AXIS \
    axis_switch_0/axis_subset_converter_3_M_AXIS \
    axis_switch_0/axis_subset_converter_4_M_AXIS \
    axis_switch_0/axis_switch_internal_0_M00_AXIS \
    axis_switch_0/axis_switch_internal_0_M01_AXIS \
    axis_switch_0/axis_switch_internal_0_M02_AXIS \
    axis_switch_0/axis_switch_internal_0_M03_AXIS \
    axis_switch_0/axis_switch_internal_0_M04_AXIS \
    axis_switch_0/axis_switch_internal_0_M05_AXIS \
    axis_switch_0/scenario_V_1 \
    axis_switch_0/axis_switch_internal_0_s_decode_err }\
]

create_bd_cell -type ip -vlnv xilinx.com:ip:system_ila:1.1 axis_switch_0/system_ila_axis_switch_0
set_property -dict [ list \
    CONFIG.C_SLOT {0} \
    CONFIG.C_BRAM_CNT {70.5} \
    CONFIG.C_NUM_MONITOR_SLOTS {12} \
    CONFIG.C_MON_TYPE {MIX} \
    CONFIG.C_SLOT_0_INTF_TYPE {xilinx.com:interface:axis_rtl:1.0} \
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
] [get_bd_cells axis_switch_0/system_ila_axis_switch_0]

connect_bd_net [get_bd_pins axis_switch_0/system_ila_axis_switch_0/clk   ] [get_bd_pins axis_switch_0/aclk     ] 
connect_bd_net [get_bd_pins axis_switch_0/system_ila_axis_switch_0/resetn] [get_bd_pins axis_switch_0/aresetn  ]


connect_bd_intf_net [get_bd_intf_pins axis_switch_0/system_ila_axis_switch_0/SLOT_0_AXIS]  [get_bd_intf_pins axis_switch_0/axis_switch_internal_0/S00_AXIS ]
connect_bd_intf_net [get_bd_intf_pins axis_switch_0/system_ila_axis_switch_0/SLOT_1_AXIS]  [get_bd_intf_pins axis_switch_0/axis_switch_internal_0/S01_AXIS ]
connect_bd_intf_net [get_bd_intf_pins axis_switch_0/system_ila_axis_switch_0/SLOT_2_AXIS]  [get_bd_intf_pins axis_switch_0/axis_switch_internal_0/S02_AXIS ]
connect_bd_intf_net [get_bd_intf_pins axis_switch_0/system_ila_axis_switch_0/SLOT_3_AXIS]  [get_bd_intf_pins axis_switch_0/axis_switch_internal_0/S03_AXIS ]
connect_bd_intf_net [get_bd_intf_pins axis_switch_0/system_ila_axis_switch_0/SLOT_4_AXIS]  [get_bd_intf_pins axis_switch_0/axis_switch_internal_0/S04_AXIS ]
connect_bd_intf_net [get_bd_intf_pins axis_switch_0/system_ila_axis_switch_0/SLOT_5_AXIS]  [get_bd_intf_pins axis_switch_0/axis_switch_internal_0/M00_AXIS ]
connect_bd_intf_net [get_bd_intf_pins axis_switch_0/system_ila_axis_switch_0/SLOT_6_AXIS]  [get_bd_intf_pins axis_switch_0/axis_switch_internal_0/M01_AXIS ]
connect_bd_intf_net [get_bd_intf_pins axis_switch_0/system_ila_axis_switch_0/SLOT_7_AXIS]  [get_bd_intf_pins axis_switch_0/axis_switch_internal_0/M02_AXIS ]
connect_bd_intf_net [get_bd_intf_pins axis_switch_0/system_ila_axis_switch_0/SLOT_8_AXIS]  [get_bd_intf_pins axis_switch_0/axis_switch_internal_0/M03_AXIS ]
connect_bd_intf_net [get_bd_intf_pins axis_switch_0/system_ila_axis_switch_0/SLOT_9_AXIS]  [get_bd_intf_pins axis_switch_0/axis_switch_internal_0/M04_AXIS ]
connect_bd_intf_net [get_bd_intf_pins axis_switch_0/system_ila_axis_switch_0/SLOT_10_AXIS] [get_bd_intf_pins axis_switch_0/axis_switch_internal_0/M05_AXIS ]
connect_bd_intf_net [get_bd_intf_pins axis_switch_0/system_ila_axis_switch_0/SLOT_11_AXIS] [get_bd_intf_pins axis_switch_0/tdest_creation_0/scenario_V ]
connect_bd_net      [get_bd_pins axis_switch_0/system_ila_axis_switch_0/probe0           ] [get_bd_pins      axis_switch_0/axis_switch_internal_0/s_decode_err]
save_bd_design
validate_bd_design