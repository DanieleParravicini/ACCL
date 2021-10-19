
set fpgapart [lindex $::argv 0]
set boardpart [lindex $::argv 1]

create_project project_ernic project_ernic -part $fpgapart
set_property board_part $boardpart [current_project]
#set_property platform.extensible true [current_project]

create_bd_design "design_ernic_ip"
#instantiate ERNIC
create_bd_cell -type ip -vlnv xilinx.com:ip:ernic:3.0 ernic_0



#create and connect S_AXILITE 
set S_AXI_LITE_CLK_FREQ 100000000
create_bd_port -dir I -type clk -freq_hz $S_AXI_LITE_CLK_FREQ s_axi_lite_aclk
connect_bd_net [get_bd_pins /ernic_0/s_axi_lite_aclk    ] [get_bd_ports s_axi_lite_aclk     ]
create_bd_port -dir I -type rst s_axi_lite_aresetn
connect_bd_net [get_bd_pins /ernic_0/s_axi_lite_aresetn ] [get_bd_ports s_axi_lite_aresetn  ]
create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 s_axi_lite
set_property -dict [\
    list CONFIG.PROTOCOL                [get_property CONFIG.PROTOCOL               [get_bd_intf_pins ernic_0/s_axi_lite]] \
         CONFIG.HAS_BURST               [get_property CONFIG.HAS_BURST              [get_bd_intf_pins ernic_0/s_axi_lite]] \
         CONFIG.HAS_LOCK                [get_property CONFIG.HAS_LOCK               [get_bd_intf_pins ernic_0/s_axi_lite]] \
         CONFIG.HAS_PROT                [get_property CONFIG.HAS_PROT               [get_bd_intf_pins ernic_0/s_axi_lite]] \
         CONFIG.HAS_CACHE               [get_property CONFIG.HAS_CACHE              [get_bd_intf_pins ernic_0/s_axi_lite]] \
         CONFIG.HAS_QOS                 [get_property CONFIG.HAS_QOS                [get_bd_intf_pins ernic_0/s_axi_lite]] \
         CONFIG.HAS_REGION              [get_property CONFIG.HAS_REGION             [get_bd_intf_pins ernic_0/s_axi_lite]] \
         CONFIG.SUPPORTS_NARROW_BURST   [get_property CONFIG.SUPPORTS_NARROW_BURST  [get_bd_intf_pins ernic_0/s_axi_lite]] \
         CONFIG.MAX_BURST_LENGTH        [get_property CONFIG.MAX_BURST_LENGTH       [get_bd_intf_pins ernic_0/s_axi_lite]] \
] [get_bd_intf_ports s_axi_lite]
connect_bd_intf_net [get_bd_intf_pins ernic_0/s_axi_lite] [get_bd_intf_ports s_axi_lite]

#create and connect m_axi_ports
set M_AXI_CLK_FREQ 200000000
create_bd_port -dir I -type clk -freq_hz $M_AXI_CLK_FREQ m_axi_aclk
connect_bd_net [get_bd_pins /ernic_0/m_axi_aclk] [get_bd_ports m_axi_aclk]
create_bd_port -dir I -type rst m_axi_aresetn
connect_bd_net [get_bd_pins /ernic_0/m_axi_aresetn] [get_bd_ports m_axi_aresetn]

create_bd_intf_port -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 qp_mgr_m_axi
set_property -dict [ \
    list CONFIG.DATA_WIDTH              [get_property CONFIG.DATA_WIDTH             [get_bd_intf_pins ernic_0/qp_mgr_m_axi]] \
         CONFIG.HAS_QOS                 [get_property CONFIG.HAS_QOS                [get_bd_intf_pins ernic_0/qp_mgr_m_axi]] \
         CONFIG.HAS_REGION              [get_property CONFIG.HAS_REGION             [get_bd_intf_pins ernic_0/qp_mgr_m_axi]] \
         CONFIG.NUM_READ_OUTSTANDING    [get_property CONFIG.NUM_READ_OUTSTANDING   [get_bd_intf_pins ernic_0/qp_mgr_m_axi]] \
         CONFIG.NUM_WRITE_OUTSTANDING   [get_property CONFIG.NUM_WRITE_OUTSTANDING  [get_bd_intf_pins ernic_0/qp_mgr_m_axi]] \
] [get_bd_intf_ports qp_mgr_m_axi]
connect_bd_intf_net [get_bd_intf_pins ernic_0/qp_mgr_m_axi] [get_bd_intf_ports qp_mgr_m_axi]

create_bd_intf_port -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 resp_hndler_m_axi
set_property -dict [ \
    list CONFIG.DATA_WIDTH              [get_property CONFIG.DATA_WIDTH             [get_bd_intf_pins ernic_0/resp_hndler_m_axi]] \
         CONFIG.HAS_QOS                 [get_property CONFIG.HAS_QOS                [get_bd_intf_pins ernic_0/resp_hndler_m_axi]] \
         CONFIG.HAS_REGION              [get_property CONFIG.HAS_REGION             [get_bd_intf_pins ernic_0/resp_hndler_m_axi]] \
         CONFIG.NUM_READ_OUTSTANDING    [get_property CONFIG.NUM_READ_OUTSTANDING   [get_bd_intf_pins ernic_0/resp_hndler_m_axi]] \
         CONFIG.NUM_WRITE_OUTSTANDING   [get_property CONFIG.NUM_WRITE_OUTSTANDING  [get_bd_intf_pins ernic_0/resp_hndler_m_axi]] \
] [get_bd_intf_ports resp_hndler_m_axi]
connect_bd_intf_net [get_bd_intf_pins ernic_0/resp_hndler_m_axi] [get_bd_intf_ports resp_hndler_m_axi]

create_bd_intf_port -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 rx_pkt_hndler_ddr_m_axi
set_property -dict [ \
    list CONFIG.DATA_WIDTH              [get_property CONFIG.DATA_WIDTH             [get_bd_intf_pins ernic_0/rx_pkt_hndler_ddr_m_axi]] \
         CONFIG.HAS_QOS                 [get_property CONFIG.HAS_QOS                [get_bd_intf_pins ernic_0/rx_pkt_hndler_ddr_m_axi]] \
         CONFIG.HAS_REGION              [get_property CONFIG.HAS_REGION             [get_bd_intf_pins ernic_0/rx_pkt_hndler_ddr_m_axi]] \
         CONFIG.NUM_READ_OUTSTANDING    [get_property CONFIG.NUM_READ_OUTSTANDING   [get_bd_intf_pins ernic_0/rx_pkt_hndler_ddr_m_axi]] \
         CONFIG.NUM_WRITE_OUTSTANDING   [get_property CONFIG.NUM_WRITE_OUTSTANDING  [get_bd_intf_pins ernic_0/rx_pkt_hndler_ddr_m_axi]] \
] [get_bd_intf_ports rx_pkt_hndler_ddr_m_axi]
connect_bd_intf_net [get_bd_intf_pins ernic_0/rx_pkt_hndler_ddr_m_axi] [get_bd_intf_ports rx_pkt_hndler_ddr_m_axi]


create_bd_intf_port -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 rx_pkt_hndler_rdrsp_m_axi
set_property -dict [ \
    list CONFIG.DATA_WIDTH              [get_property CONFIG.DATA_WIDTH             [get_bd_intf_pins ernic_0/rx_pkt_hndler_rdrsp_m_axi]] \
         CONFIG.HAS_QOS                 [get_property CONFIG.HAS_QOS                [get_bd_intf_pins ernic_0/rx_pkt_hndler_rdrsp_m_axi]] \
         CONFIG.HAS_REGION              [get_property CONFIG.HAS_REGION             [get_bd_intf_pins ernic_0/rx_pkt_hndler_rdrsp_m_axi]] \
         CONFIG.NUM_READ_OUTSTANDING    [get_property CONFIG.NUM_READ_OUTSTANDING   [get_bd_intf_pins ernic_0/rx_pkt_hndler_rdrsp_m_axi]] \
         CONFIG.NUM_WRITE_OUTSTANDING   [get_property CONFIG.NUM_WRITE_OUTSTANDING  [get_bd_intf_pins ernic_0/rx_pkt_hndler_rdrsp_m_axi]] \
] [get_bd_intf_ports rx_pkt_hndler_rdrsp_m_axi]
connect_bd_intf_net [get_bd_intf_pins ernic_0/rx_pkt_hndler_rdrsp_m_axi] [get_bd_intf_ports rx_pkt_hndler_rdrsp_m_axi]


create_bd_intf_port -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 wqe_proc_top_m_axi
set_property -dict [ \
    list CONFIG.DATA_WIDTH              [get_property CONFIG.DATA_WIDTH             [get_bd_intf_pins ernic_0/wqe_proc_top_m_axi]] \
         CONFIG.HAS_QOS                 [get_property CONFIG.HAS_QOS                [get_bd_intf_pins ernic_0/wqe_proc_top_m_axi]] \
         CONFIG.HAS_REGION              [get_property CONFIG.HAS_REGION             [get_bd_intf_pins ernic_0/wqe_proc_top_m_axi]] \
         CONFIG.NUM_READ_OUTSTANDING    [get_property CONFIG.NUM_READ_OUTSTANDING   [get_bd_intf_pins ernic_0/wqe_proc_top_m_axi]] \
         CONFIG.NUM_WRITE_OUTSTANDING   [get_property CONFIG.NUM_WRITE_OUTSTANDING  [get_bd_intf_pins ernic_0/wqe_proc_top_m_axi]] \
] [get_bd_intf_ports wqe_proc_top_m_axi]
connect_bd_intf_net [get_bd_intf_pins ernic_0/wqe_proc_top_m_axi] [get_bd_intf_ports wqe_proc_top_m_axi]


create_bd_intf_port -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 wqe_proc_wr_ddr_m_axi
set_property -dict [ \
    list CONFIG.DATA_WIDTH              [get_property CONFIG.DATA_WIDTH             [get_bd_intf_pins ernic_0/wqe_proc_wr_ddr_m_axi]] \
         CONFIG.HAS_QOS                 [get_property CONFIG.HAS_QOS                [get_bd_intf_pins ernic_0/wqe_proc_wr_ddr_m_axi]] \
         CONFIG.HAS_REGION              [get_property CONFIG.HAS_REGION             [get_bd_intf_pins ernic_0/wqe_proc_wr_ddr_m_axi]] \
         CONFIG.NUM_READ_OUTSTANDING    [get_property CONFIG.NUM_READ_OUTSTANDING   [get_bd_intf_pins ernic_0/wqe_proc_wr_ddr_m_axi]] \
         CONFIG.NUM_WRITE_OUTSTANDING   [get_property CONFIG.NUM_WRITE_OUTSTANDING  [get_bd_intf_pins ernic_0/wqe_proc_wr_ddr_m_axi]] \
] [get_bd_intf_ports wqe_proc_wr_ddr_m_axi]
connect_bd_intf_net [get_bd_intf_pins ernic_0/wqe_proc_wr_ddr_m_axi] [get_bd_intf_ports wqe_proc_wr_ddr_m_axi]


#CMAC interfaces
set CMAC_FREQ 322265625
create_bd_port -dir I -type clk -freq_hz $CMAC_FREQ cmac_clk
connect_bd_net [get_bd_pins /ernic_0/cmac_rx_clk] [get_bd_ports cmac_clk]
connect_bd_net [get_bd_ports cmac_clk           ] [get_bd_pins ernic_0/cmac_tx_clk]

create_bd_port -dir I -type rst cmac_rst
set_property CONFIG.POLARITY [get_property CONFIG.POLARITY [get_bd_pins ernic_0/cmac_rx_rst]] [get_bd_ports cmac_rst]
connect_bd_net [get_bd_pins /ernic_0/cmac_rx_rst] [get_bd_ports cmac_rst]
connect_bd_net [get_bd_ports cmac_rst           ] [get_bd_pins ernic_0/cmac_tx_rst]

create_bd_intf_port -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 cmac_tx
connect_bd_intf_net [get_bd_intf_pins ernic_0/cmac_m_axis] [get_bd_intf_ports cmac_tx]

create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 cmac_rx
set_property -dict [ \
    list CONFIG.TDATA_NUM_BYTES         [get_property CONFIG.TDATA_NUM_BYTES    [get_bd_intf_pins ernic_0/roce_cmac_s_axis]] \
         CONFIG.TUSER_WIDTH             [get_property CONFIG.TUSER_WIDTH        [get_bd_intf_pins ernic_0/roce_cmac_s_axis]] \
         CONFIG.HAS_TREADY              [get_property CONFIG.HAS_TREADY         [get_bd_intf_pins ernic_0/roce_cmac_s_axis]] \
         CONFIG.HAS_TKEEP               [get_property CONFIG.HAS_TKEEP          [get_bd_intf_pins ernic_0/roce_cmac_s_axis]] \
        CONFIG.HAS_TLAST                [get_property CONFIG.HAS_TLAST          [get_bd_intf_pins ernic_0/roce_cmac_s_axis]] \
] [get_bd_intf_ports cmac_rx]
connect_bd_intf_net [get_bd_intf_pins ernic_0/roce_cmac_s_axis] [get_bd_intf_ports cmac_rx]

#doorbells
#CQ PI doorbell 
create_bd_cell -type ip -vlnv xilinx.com:ip:xlconcat:2.1 concat_CQ_PI
set_property -dict [list CONFIG.IN2_WIDTH.VALUE_SRC USER \
                         CONFIG.NUM_PORTS {3} \
                         CONFIG.IN2_WIDTH {6} \
] [get_bd_cells concat_CQ_PI]

create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant:1.1 CQ_PI_constant_padding
set_property -dict [list CONFIG.CONST_WIDTH {6} CONFIG.CONST_VAL {0}] [get_bd_cells CQ_PI_constant_padding]

connect_bd_net [get_bd_pins ernic_0/resp_hndler_o_send_cq_db_cnt    	] [get_bd_pins concat_CQ_PI/In0]
connect_bd_net [get_bd_pins ernic_0/resp_hndler_o_send_cq_db_addr       ] [get_bd_pins concat_CQ_PI/In1]
connect_bd_net [get_bd_pins CQ_PI_constant_padding/dout                 ] [get_bd_pins concat_CQ_PI/In2]

create_bd_intf_port -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 cq_pi_db

create_bd_cell -type ip -vlnv xilinx.com:ip:axis_register_slice:1.1 axis_reg_cq_pi_db
set_property -dict [list CONFIG.TDATA_NUM_BYTES.VALUE_SRC USER  ] [get_bd_cells axis_reg_cq_pi_db]
set_property -dict [list CONFIG.TDATA_NUM_BYTES {6}             ] [get_bd_cells axis_reg_cq_pi_db]

connect_bd_net [get_bd_pins ernic_0/resp_hndler_o_send_cq_db_cnt_valid  ] [get_bd_pins axis_reg_cq_pi_db/s_axis_tvalid      ]
connect_bd_net [get_bd_pins axis_reg_cq_pi_db/s_axis_tready             ] [get_bd_pins ernic_0/resp_hndler_i_send_cq_db_rdy ]
connect_bd_net [get_bd_pins axis_reg_cq_pi_db/s_axis_tdata              ] [get_bd_pins concat_CQ_PI/dout                      ]
connect_bd_intf_net [get_bd_intf_ports cq_pi_db ] [get_bd_intf_pins axis_reg_cq_pi_db/M_AXIS]
connect_bd_net      [get_bd_ports m_axi_aclk    ] [get_bd_pins axis_reg_cq_pi_db/aclk       ]
connect_bd_net      [get_bd_ports m_axi_aresetn ] [get_bd_pins axis_reg_cq_pi_db/aresetn    ]

#RQ PI doorbell
create_bd_cell -type ip -vlnv xilinx.com:ip:xlconcat:2.1 concat_RQ_PI
set_property -dict [list CONFIG.IN2_WIDTH.VALUE_SRC USER \
                         CONFIG.NUM_PORTS {3} \
                         CONFIG.IN2_WIDTH {6} \
] [get_bd_cells concat_RQ_PI]

create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant:1.1 RQ_PI_constant_padding
set_property -dict [list CONFIG.CONST_WIDTH {6} CONFIG.CONST_VAL {0}] [get_bd_cells RQ_PI_constant_padding]

connect_bd_net [get_bd_pins ernic_0/rx_pkt_hndler_o_rq_db_data    	    ] [get_bd_pins concat_RQ_PI/In0]
connect_bd_net [get_bd_pins ernic_0/rx_pkt_hndler_o_rq_db_addr          ] [get_bd_pins concat_RQ_PI/In1]
connect_bd_net [get_bd_pins RQ_PI_constant_padding/dout                 ] [get_bd_pins concat_RQ_PI/In2]
create_bd_intf_port -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 rq_pi_db

create_bd_cell -type ip -vlnv xilinx.com:ip:axis_register_slice:1.1 axis_reg_rq_pi_db
set_property -dict [list CONFIG.TDATA_NUM_BYTES.VALUE_SRC USER  ] [get_bd_cells axis_reg_rq_pi_db]
set_property -dict [list CONFIG.TDATA_NUM_BYTES {6}             ] [get_bd_cells axis_reg_rq_pi_db]

connect_bd_net [get_bd_pins ernic_0/rx_pkt_hndler_o_rq_db_data_valid    ] [get_bd_pins axis_reg_rq_pi_db/s_axis_tvalid      ]
connect_bd_net [get_bd_pins axis_reg_rq_pi_db/s_axis_tready             ] [get_bd_pins ernic_0/rx_pkt_hndler_i_rq_db_rdy ]
connect_bd_net [get_bd_pins axis_reg_rq_pi_db/s_axis_tdata              ] [get_bd_pins concat_RQ_PI/dout                      ]
connect_bd_intf_net [get_bd_intf_ports rq_pi_db ] [get_bd_intf_pins axis_reg_rq_pi_db/M_AXIS    ]
connect_bd_net      [get_bd_ports m_axi_aclk    ] [get_bd_pins      axis_reg_rq_pi_db/aclk      ]
connect_bd_net      [get_bd_ports m_axi_aresetn ] [get_bd_pins      axis_reg_rq_pi_db/aresetn   ]

#RQ CI doorbell
create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 rq_ci_db
set_property -dict [list CONFIG.TDATA_NUM_BYTES {6}] [get_bd_intf_ports rq_ci_db]
create_bd_cell -type ip -vlnv xilinx.com:ip:axis_register_slice:1.1 axis_reg_rq_ci_db
set_property -dict [list CONFIG.TDATA_NUM_BYTES.VALUE_SRC USER  ] [get_bd_cells axis_reg_rq_ci_db]
set_property -dict [list CONFIG.TDATA_NUM_BYTES {6}             ] [get_bd_cells axis_reg_rq_ci_db]

connect_bd_intf_net [get_bd_intf_ports rq_ci_db ] [get_bd_intf_pins axis_reg_rq_ci_db/S_AXIS]
connect_bd_net      [get_bd_ports m_axi_aclk    ] [get_bd_pins axis_reg_rq_ci_db/aclk       ]
connect_bd_net      [get_bd_ports m_axi_aresetn ] [get_bd_pins axis_reg_rq_ci_db/aresetn    ]

create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 xlslice_rq_ci_addr
create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 xlslice_rq_ci_value

connect_bd_net [get_bd_pins axis_reg_rq_ci_db/m_axis_tdata] [get_bd_pins xlslice_rq_ci_addr/Din ]
connect_bd_net [get_bd_pins axis_reg_rq_ci_db/m_axis_tdata] [get_bd_pins xlslice_rq_ci_value/Din]
set_property -dict [list CONFIG.DIN_TO  {0} CONFIG.DIN_FROM {31} CONFIG.DIN_WIDTH {48} CONFIG.DOUT_WIDTH {32}] [get_bd_cells xlslice_rq_ci_addr]
set_property -dict [list CONFIG.DIN_TO {32} CONFIG.DIN_FROM {47} CONFIG.DIN_WIDTH {48} CONFIG.DOUT_WIDTH {16}] [get_bd_cells xlslice_rq_ci_value]
connect_bd_net [get_bd_pins xlslice_rq_ci_addr/Dout         ] [get_bd_pins ernic_0/i_qp_rq_cidb_wr_addr_hndshk ]
connect_bd_net [get_bd_pins xlslice_rq_ci_value/Dout        ] [get_bd_pins ernic_0/i_qp_rq_cidb_hndshk         ]
connect_bd_net [get_bd_pins axis_reg_rq_ci_db/m_axis_tvalid ] [get_bd_pins ernic_0/i_qp_rq_cidb_wr_valid_hndshk ]
connect_bd_net [get_bd_pins ernic_0/o_qp_rq_cidb_wr_rdy     ] [get_bd_pins axis_reg_rq_ci_db/m_axis_tready      ]

#SQ PI doorbell
create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 sq_pi_db
set_property -dict [list CONFIG.TDATA_NUM_BYTES {6}] [get_bd_intf_ports sq_pi_db]
create_bd_cell -type ip -vlnv xilinx.com:ip:axis_register_slice:1.1 axis_reg_sq_pi_db
set_property -dict [list CONFIG.TDATA_NUM_BYTES.VALUE_SRC USER  ] [get_bd_cells axis_reg_sq_pi_db]
set_property -dict [list CONFIG.TDATA_NUM_BYTES {6}             ] [get_bd_cells axis_reg_sq_pi_db]

connect_bd_intf_net [get_bd_intf_ports sq_pi_db ] [get_bd_intf_pins axis_reg_sq_pi_db/S_AXIS]
connect_bd_net      [get_bd_ports m_axi_aclk    ] [get_bd_pins axis_reg_sq_pi_db/aclk       ]
connect_bd_net      [get_bd_ports m_axi_aresetn ] [get_bd_pins axis_reg_sq_pi_db/aresetn    ]

create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 xlslice_sq_pi_addr
create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 xlslice_sq_pi_value

connect_bd_net [get_bd_pins axis_reg_sq_pi_db/m_axis_tdata] [get_bd_pins xlslice_sq_pi_addr/Din ]
connect_bd_net [get_bd_pins axis_reg_sq_pi_db/m_axis_tdata] [get_bd_pins xlslice_sq_pi_value/Din]
set_property -dict [list CONFIG.DIN_TO  {0} CONFIG.DIN_FROM {31} CONFIG.DIN_WIDTH {48} CONFIG.DOUT_WIDTH {32}] [get_bd_cells xlslice_sq_pi_addr]
set_property -dict [list CONFIG.DIN_TO {32} CONFIG.DIN_FROM {47} CONFIG.DIN_WIDTH {48} CONFIG.DOUT_WIDTH {16}] [get_bd_cells xlslice_sq_pi_value]
connect_bd_net [get_bd_pins xlslice_sq_pi_addr/Dout         ] [get_bd_pins ernic_0/i_qp_sq_pidb_wr_addr_hndshk ]
connect_bd_net [get_bd_pins xlslice_sq_pi_value/Dout        ] [get_bd_pins ernic_0/i_qp_sq_pidb_hndshk         ]
connect_bd_net [get_bd_pins axis_reg_sq_pi_db/m_axis_tvalid ] [get_bd_pins ernic_0/i_qp_sq_pidb_wr_valid_hndshk ]
connect_bd_net [get_bd_pins ernic_0/o_qp_sq_pidb_wr_rdy     ] [get_bd_pins axis_reg_sq_pi_db/m_axis_tready      ]

#Tieoff all other interfaces
create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant:1.1 xlconstant_0
set_property -dict [list CONFIG.CONST_VAL {0}] [get_bd_cells xlconstant_0]
connect_bd_net [get_bd_pins xlconstant_0/dout] [get_bd_pins ernic_0/ieth_immdt_axis_trdy]
connect_bd_net [get_bd_pins xlconstant_0/dout] [get_bd_pins ernic_0/non_roce_dma_m_axis_tready]
connect_bd_net [get_bd_pins xlconstant_0/dout] [get_bd_pins ernic_0/non_roce_dma_s_axis_tvalid]
connect_bd_net [get_bd_pins xlconstant_0/dout] [get_bd_pins ernic_0/non_roce_cmac_s_axis_tvalid]

#Addresses
assign_bd_address
# for axi_lite based on doc 3.1 registers spans from 0x00 to 0x202B0 + ((255) x 0x0100) = 0x302B0
assign_bd_address -offset 0x00000000 -range 0x00040000 -target_address_space [get_bd_addr_spaces /s_axi_lite]    [get_bd_addr_segs ernic_0/s_axi_lite/reg0] -force
# for all other m_axi interfaces exiting from ERNIC allocate the max memory space size
assign_bd_address -offset 0x00000000 -range 0x100000000 -target_address_space [get_bd_addr_spaces /ernic_0/m_axi] [get_bd_addr_segs /qp_mgr_m_axi/Reg]               -force
assign_bd_address -offset 0x00000000 -range 0x100000000 -target_address_space [get_bd_addr_spaces /ernic_0/m_axi] [get_bd_addr_segs /resp_hndler_m_axi/Reg]          -force
assign_bd_address -offset 0x00000000 -range 0x100000000 -target_address_space [get_bd_addr_spaces /ernic_0/m_axi] [get_bd_addr_segs /rx_pkt_hndler_ddr_m_axi/Reg]    -force
assign_bd_address -offset 0x00000000 -range 0x100000000 -target_address_space [get_bd_addr_spaces /ernic_0/m_axi] [get_bd_addr_segs /rx_pkt_hndler_rdrsp_m_axi/Reg]  -force
assign_bd_address -offset 0x00000000 -range 0x100000000 -target_address_space [get_bd_addr_spaces /ernic_0/m_axi] [get_bd_addr_segs /wqe_proc_top_m_axi/Reg]         -force
assign_bd_address -offset 0x00000000 -range 0x100000000 -target_address_space [get_bd_addr_spaces /ernic_0/m_axi] [get_bd_addr_segs /wqe_proc_wr_ddr_m_axi/Reg]      -force

validate_bd_design
save_bd_design

generate_target all [get_files  /home/danielep/ACCL_ERNIC/demo/build/ERNIC/project_ernic/project_ernic.srcs/sources_1/bd/design_ernic_ip/design_ernic_ip.bd]
catch { config_ip_cache -export [get_ips -all design_ernic_ip_ernic_0_0] }
catch { config_ip_cache -export [get_ips -all design_ernic_ip_axis_reg_cq_pi_db_0] }
catch { config_ip_cache -export [get_ips -all design_ernic_ip_axis_reg_rq_pi_db_0] }
catch { config_ip_cache -export [get_ips -all design_ernic_ip_axis_reg_rq_ci_db_0] }
catch { config_ip_cache -export [get_ips -all design_ernic_ip_axis_reg_sq_pi_db_0] }
export_ip_user_files -of_objects [get_files /home/danielep/ACCL_ERNIC/demo/build/ERNIC/project_ernic/project_ernic.srcs/sources_1/bd/design_ernic_ip/design_ernic_ip.bd] -no_script -sync -force -quiet
create_ip_run [get_files -of_objects [get_fileset sources_1] /home/danielep/ACCL_ERNIC/demo/build/ERNIC/project_ernic/project_ernic.srcs/sources_1/bd/design_ernic_ip/design_ernic_ip.bd]
launch_runs design_ernic_ip_ernic_0_0_synth_1 design_ernic_ip_axis_reg_cq_pi_db_0_synth_1 design_ernic_ip_axis_reg_rq_pi_db_0_synth_1 design_ernic_ip_axis_reg_rq_ci_db_0_synth_1 design_ernic_ip_axis_reg_sq_pi_db_0_synth_1 -jobs 16
make_wrapper -files [get_files /home/danielep/ACCL_ERNIC/demo/build/ERNIC/project_ernic/project_ernic.srcs/sources_1/bd/design_ernic_ip/design_ernic_ip.bd] -top
add_files -norecurse /home/danielep/ACCL_ERNIC/demo/build/ERNIC/project_ernic/project_ernic.gen/sources_1/bd/design_ernic_ip/hdl/design_ernic_ip_wrapper.v