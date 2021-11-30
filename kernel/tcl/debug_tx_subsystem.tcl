create_bd_cell -type ip -vlnv xilinx.com:ip:system_ila:1.1 tcp_tx_subsystem/system_ila_0

set_property HDL_ATTRIBUTE.DEBUG true [get_bd_intf_nets { \
    tcp_tx_subsystem/in_r_1 tcp_tx_subsystem/tcp_packetizer_0_out_r \
    tcp_tx_subsystem/tcp_packetizer_0_cmd_txHandler_V \
    tcp_tx_subsystem/s_axis_tcp_tx_status_1 \
    tcp_tx_subsystem/control \
    tcp_tx_subsystem/cmd_V_1 \
    tcp_tx_subsystem/tcp_txHandler_0_m_axis_tcp_tx_meta \
    tcp_tx_subsystem/Conn3}]

set_property -dict [ \
    list CONFIG.C_BRAM_CNT {6} \
    CONFIG.C_NUM_MONITOR_SLOTS {8} \
    CONFIG.C_SLOT_0_INTF_TYPE {xilinx.com:interface:axis_rtl:1.0} \
    CONFIG.C_SLOT_1_INTF_TYPE {xilinx.com:interface:axis_rtl:1.0} \
    CONFIG.C_SLOT_2_INTF_TYPE {xilinx.com:interface:axis_rtl:1.0} \
    CONFIG.C_SLOT_3_INTF_TYPE {xilinx.com:interface:axis_rtl:1.0} \
    CONFIG.C_SLOT_4_INTF_TYPE {xilinx.com:interface:axis_rtl:1.0} \
    CONFIG.C_SLOT_5_INTF_TYPE {xilinx.com:interface:axis_rtl:1.0} \
    CONFIG.C_SLOT_6_INTF_TYPE {xilinx.com:interface:axis_rtl:1.0} \
    CONFIG.C_SLOT_7_INTF_TYPE {xilinx.com:interface:axis_rtl:1.0} \
] [get_bd_cells tcp_tx_subsystem/system_ila_0]

connect_bd_intf_net [get_bd_intf_pins tcp_tx_subsystem/system_ila_0/SLOT_0_AXIS] [get_bd_intf_pins tcp_tx_subsystem/tcp_packetizer_0/in_r]
connect_bd_intf_net [get_bd_intf_pins tcp_tx_subsystem/system_ila_0/SLOT_1_AXIS] [get_bd_intf_pins tcp_tx_subsystem/tcp_packetizer_0/cmd_V]
connect_bd_intf_net [get_bd_intf_pins tcp_tx_subsystem/system_ila_0/SLOT_2_AXIS] [get_bd_intf_pins tcp_tx_subsystem/tcp_packetizer_0/sts_V]
connect_bd_intf_net [get_bd_intf_pins tcp_tx_subsystem/system_ila_0/SLOT_3_AXIS] [get_bd_intf_pins tcp_tx_subsystem/tcp_packetizer_0/out_r]
connect_bd_intf_net [get_bd_intf_pins tcp_tx_subsystem/system_ila_0/SLOT_4_AXIS] [get_bd_intf_pins tcp_tx_subsystem/tcp_txHandler_0/cmd_txHandler_V]
connect_bd_intf_net [get_bd_intf_pins tcp_tx_subsystem/system_ila_0/SLOT_5_AXIS] [get_bd_intf_pins tcp_tx_subsystem/tcp_txHandler_0/m_axis_tcp_tx_meta]
connect_bd_intf_net [get_bd_intf_pins tcp_tx_subsystem/system_ila_0/SLOT_6_AXIS] [get_bd_intf_pins tcp_tx_subsystem/tx_fifo/M_AXIS]
connect_bd_intf_net [get_bd_intf_pins tcp_tx_subsystem/system_ila_0/SLOT_7_AXIS] [get_bd_intf_pins tcp_tx_subsystem/tcp_txHandler_0/s_axis_tcp_tx_status]

connect_bd_net [get_bd_pins tcp_tx_subsystem/ap_clk] [get_bd_pins tcp_tx_subsystem/system_ila_0/clk]
connect_bd_net [get_bd_pins tcp_tx_subsystem/ap_rst_n] [get_bd_pins tcp_tx_subsystem/system_ila_0/resetn]
