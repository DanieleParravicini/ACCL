set_property HDL_ATTRIBUTE.DEBUG true [ \
    get_bd_intf_nets { \
    control/hls_control_1 \
    control/hostctrl_in_0_sts_out_V \
    control/hostctrl_in_0_cmd_out_V \
    control/microblaze_0_M13_AXIS \
    }]

create_bd_cell -type ip -vlnv xilinx.com:ip:system_ila:1.1 control/ila_hls_control
set_property -dict [list \
    CONFIG.C_BRAM_CNT {6} \
    CONFIG.C_NUM_MONITOR_SLOTS {4} \
    CONFIG.C_BRAM_CNT {23.5} \
    CONFIG.C_SLOT_0_INTF_TYPE {xilinx.com:interface:axis_rtl:1.0} \
    CONFIG.C_SLOT_1_INTF_TYPE {xilinx.com:interface:axis_rtl:1.0} \
    CONFIG.C_SLOT_2_INTF_TYPE {xilinx.com:interface:axis_rtl:1.0} \
    CONFIG.C_SLOT_3_INTF_TYPE {xilinx.com:interface:axis_rtl:1.0} \
] [get_bd_cells control/ila_hls_control]

connect_bd_net      [get_bd_pins control/ap_clk                          ] [get_bd_pins control/ila_hls_control/clk]
connect_bd_net      [get_bd_pins control/ila_hls_control/resetn          ] [get_bd_pins control/proc_sys_reset_0/peripheral_aresetn]

connect_bd_intf_net [get_bd_intf_pins control/ila_hls_control/SLOT_0_AXIS] [get_bd_intf_pins control/hostctrl_in_0/cmd_in_V]
connect_bd_intf_net [get_bd_intf_pins control/ila_hls_control/SLOT_1_AXIS] [get_bd_intf_pins control/hostctrl_in_0/sts_in_V]
connect_bd_intf_net [get_bd_intf_pins control/ila_hls_control/SLOT_2_AXIS] [get_bd_intf_pins control/microblaze_0/S13_AXIS]
connect_bd_intf_net [get_bd_intf_pins control/ila_hls_control/SLOT_3_AXIS] [get_bd_intf_pins control/hostctrl_in_0/sts_out_V]

save_bd_design
validate_bd_design