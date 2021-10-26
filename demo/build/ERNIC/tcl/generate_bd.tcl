
#   Copyright (c) 2021, Xilinx, Inc.
#   All rights reserved.
#
#   Redistribution and use in source and binary forms, with or without
#   modification, are permitted provided that the following conditions are met:
#
#   1.  Redistributions of source code must retain the above copyright notice,
#       this list of conditions and the following disclaimer.
#
#   2.  Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#
#   3.  Neither the name of the copyright holder nor the names of its
#       contributors may be used to endorse or promote products derived from
#       this software without specific prior written permission.
#
#   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
#   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#   PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
#   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
#   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
#   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
#   OR BUSINESS INTERRUPTION). HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
#   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
#   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
#   ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
set fpgapart  [lindex $::argv 0]
set boardpart [lindex $::argv 1]
variable fpgapart
variable boardpart
set fpgapart  "xcu280-fsvh2892-2L-e"
set boardpart "xilinx.com:au280:part0:1.1"
set interface_number 0

create_project project_ernic project_ernic -part $fpgapart
set_property board_part $boardpart [current_project]
#set_property platform.extensible true [current_project]

###### Create Block Design ######
set design_name "design_ernic_and_cmac_ip"

create_bd_design ${design_name}
open_bd_design ${design_name}

proc create_cmac {} {
    global fpgapart
    global boardpart
    ##### Create and configure CMAC IP #####

    # Default GT reference frequency
    set gt_ref_clk 322.265625
    set interface "0"
    set freerunningclock 100
    if {${fpgapart} eq "xcu50-fsvh2104-2L-e"} {
        # Possible core_selection CMACE4_X0Y3 and CMACE4_X0Y4
        #set gt_ref_clk 161.1328125
        set core_selection  CMACE4_X0Y3
        set group_selection X0Y28~X0Y31
        set interface_number 0
    } elseif {${fpgapart} eq "xcu200-fsgd2104-2-e"} {
    switch ${interface} {
        "1" {
        # Possible core_selection CMACE4_X0Y6 and CMACE4_X0Y7
        set core_selection  CMACE4_X0Y6
        set group_selection X1Y44~X1Y47
        set interface_number 1
        }
        default {
        # Possible core_selection CMACE4_X0Y6; CMACE4_X0Y7 and CMACE4_X0Y8
        set core_selection  CMACE4_X0Y8
        set group_selection X1Y48~X1Y51
        set interface_number 0
        }
    }
    } elseif {${fpgapart} eq "xcu250-figd2104-2L-e"} {
    switch ${interface} {
        "1" {
        # Possible core_selection CMACE4_X0Y6; CMACE4_X0Y7 and CMACE4_X0Y8
        set core_selection  CMACE4_X0Y6
        set group_selection X1Y40~X1Y43
        set interface_number 1
        }
        default {
        # Possible core_selection CMACE4_X0Y7 and CMACE4_X0Y8
        set core_selection  CMACE4_X0Y7
        set group_selection X1Y44~X1Y47
        set interface_number 0
        }
    }
    } elseif {${fpgapart} eq "xcu280-fsvh2892-2L-e"} {
    set freerunningclock 50
    switch ${interface} {
        "1" {
        # Possible core_selection CMACE4_X0Y6 and CMACE4_X0Y7
        set core_selection  CMACE4_X0Y6
        set group_selection X0Y44~X0Y47
        set interface_number 1
        }
        default {
        # Possible core_selection CMACE4_X0Y5; CMACE4_X0Y6 and CMACE4_X0Y7
        set core_selection  CMACE4_X0Y5
        set group_selection X0Y40~X0Y43
        set interface_number 0
        }
    }
    } else {
    puts "unknown part
    return -1
    }
    variable cmac_name
    set cmac_name cmac_uplus_${interface_number}
    set gt_clk_freq [expr int(${gt_ref_clk} * 1000000)]
    puts "Generating IPI for ${cmac_name} with GT clock running at ${gt_clk_freq} Hz"

    set cmac_ip [create_bd_cell -type ip -vlnv xilinx.com:ip:cmac_usplus ${cmac_name}]
    set_property -dict [ list \
    CONFIG.CMAC_CAUI4_MODE             {1} \
    CONFIG.NUM_LANES                   {4x25} \
    CONFIG.GT_REF_CLK_FREQ             $gt_ref_clk \
    CONFIG.CMAC_CORE_SELECT            $core_selection \
    CONFIG.GT_GROUP_SELECT             $group_selection \
    CONFIG.GT_DRP_CLK                  $freerunningclock \
    CONFIG.USER_INTERFACE              {AXIS} \
    CONFIG.INCLUDE_SHARED_LOGIC        {2} \
    CONFIG.LANE5_GT_LOC                {NA} \
    CONFIG.LANE6_GT_LOC                {NA} \
    CONFIG.LANE7_GT_LOC                {NA} \
    CONFIG.LANE8_GT_LOC                {NA} \
    CONFIG.LANE9_GT_LOC                {NA} \
    CONFIG.LANE10_GT_LOC               {NA} \
    CONFIG.OPERATING_MODE              {Duplex} \
    CONFIG.ENABLE_AXI_INTERFACE        {1} \
    CONFIG.INCLUDE_STATISTICS_COUNTERS {1} \
    CONFIG.RX_CHECK_ACK                {1} \
    CONFIG.TX_FLOW_CONTROL             {1} \
    CONFIG.RX_FLOW_CONTROL             {1} \
    CONFIG.ENABLE_TIME_STAMPING        {0} \
    CONFIG.TX_PTP_1STEP_ENABLE         {2} \
    CONFIG.PTP_TRANSPCLK_MODE          {0} \
    CONFIG.TX_PTP_LATENCY_ADJUST       {0} \
    CONFIG.ENABLE_PIPELINE_REG         {1} \
    CONFIG.INCLUDE_RS_FEC              {1} \
    ] $cmac_ip
    #INCLUDE_RS_FEC in ERNIC is 1
    ###### Create interface ports ######

    set clk_gt_freerun [ create_bd_port -dir I -type clk -freq_hz [expr ${freerunningclock} * 1000000] clk_gt_freerun ]

    #set M_AXIS [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 M_AXIS ]
    #set_property -dict [ list \
    #CONFIG.FREQ_HZ {300000000} \
    #] $M_AXIS

    set S_AXILITE [ create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 S_AXILITE ]
    set_property -dict [ list \
    CONFIG.ADDR_WIDTH {12} \
    CONFIG.ARUSER_WIDTH {0} \
    CONFIG.AWUSER_WIDTH {0} \
    CONFIG.BUSER_WIDTH {0} \
    CONFIG.DATA_WIDTH {32} \
    CONFIG.FREQ_HZ {300000000} \
    CONFIG.HAS_BRESP {1} \
    CONFIG.HAS_BURST {0} \
    CONFIG.HAS_CACHE {0} \
    CONFIG.HAS_LOCK {0} \
    CONFIG.HAS_PROT {0} \
    CONFIG.HAS_QOS {0} \
    CONFIG.HAS_REGION {0} \
    CONFIG.HAS_RRESP {1} \
    CONFIG.HAS_WSTRB {1} \
    CONFIG.ID_WIDTH {0} \
    CONFIG.MAX_BURST_LENGTH {1} \
    CONFIG.NUM_READ_OUTSTANDING {1} \
    CONFIG.NUM_READ_THREADS {1} \
    CONFIG.NUM_WRITE_OUTSTANDING {1} \
    CONFIG.NUM_WRITE_THREADS {1} \
    CONFIG.PROTOCOL {AXI4LITE} \
    CONFIG.READ_WRITE_MODE {READ_WRITE} \
    CONFIG.RUSER_BITS_PER_BYTE {0} \
    CONFIG.RUSER_WIDTH {0} \
    CONFIG.SUPPORTS_NARROW_BURST {0} \
    CONFIG.WUSER_BITS_PER_BYTE {0} \
    CONFIG.WUSER_WIDTH {0} \
    ] $S_AXILITE

    #set S_AXIS [ create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 S_AXIS ]
    #set_property -dict [ list \
    #CONFIG.FREQ_HZ {300000000} \
    #CONFIG.HAS_TKEEP {1} \
    #CONFIG.HAS_TLAST {1} \
    #CONFIG.HAS_TREADY {1} \
    #CONFIG.HAS_TSTRB {0} \
    #CONFIG.LAYERED_METADATA {undef} \
    #CONFIG.TDATA_NUM_BYTES {64} \
    #CONFIG.TDEST_WIDTH {0} \
    #CONFIG.TID_WIDTH {0} \
    #CONFIG.TUSER_WIDTH {16} \
    #] $S_AXIS

    set gt_ref_clk [ create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:diff_clock_rtl:1.0 gt_ref_clk ]
    set_property -dict [ list \
    CONFIG.FREQ_HZ $gt_clk_freq \
    ] $gt_ref_clk

    set gt_serial_port [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:gt_rtl:1.0 gt_serial_port ]


    # Create ports
    set ap_clk [ create_bd_port -dir I -type clk -freq_hz 300000000 ap_clk ]
    #CONFIG.ASSOCIATED_BUSIF {S_AXIS:M_AXIS:S_AXILITE} \
    set_property -dict [ list \
    CONFIG.ASSOCIATED_RESET {ap_rst_n} \
    ] $ap_clk
    set ap_rst_n [ create_bd_port -dir I -type rst ap_rst_n ]
    set_property -dict [ list \
    CONFIG.POLARITY {ACTIVE_LOW} \
    ] $ap_rst_n

    ###### Add IP ######

    ## Create instance: acc_kernel_tx_cdc, and set properties
    #set acc_kernel_tx_cdc [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_clock_converter acc_kernel_tx_cdc ]
    #set_property -dict [ list \
    #CONFIG.HAS_TKEEP {1} \
    #CONFIG.HAS_TLAST {1} \
    #CONFIG.IS_ACLK_ASYNC {1} \
    #CONFIG.TDATA_NUM_BYTES {64} \
    #CONFIG.TUSER_WIDTH.VALUE_SRC USER \
    #CONFIG.TUSER_WIDTH {0} \
    #] $acc_kernel_tx_cdc
#
    ## Create instance: fifo_cmac_rx_cdc, and set properties
    #set fifo_cmac_rx_cdc [ create_bd_cell -type ip -vlnv xilinx.com:ip:fifo_generator fifo_cmac_rx_cdc ]
    #set_property -dict [ list \
    #CONFIG.Clock_Type_AXI {Independent_Clock} \
    #CONFIG.Empty_Threshold_Assert_Value_axis {509} \
    #CONFIG.Empty_Threshold_Assert_Value_rach {13} \
    #CONFIG.Empty_Threshold_Assert_Value_rdch {1018} \
    #CONFIG.Empty_Threshold_Assert_Value_wach {13} \
    #CONFIG.Empty_Threshold_Assert_Value_wdch {1018} \
    #CONFIG.Empty_Threshold_Assert_Value_wrch {13} \
    #CONFIG.Enable_TLAST {true} \
    #CONFIG.FIFO_Implementation_axis {Independent_Clocks_Block_RAM} \
    #CONFIG.FIFO_Implementation_rach {Independent_Clocks_Distributed_RAM} \
    #CONFIG.FIFO_Implementation_rdch {Independent_Clocks_Builtin_FIFO} \
    #CONFIG.FIFO_Implementation_wach {Independent_Clocks_Distributed_RAM} \
    #CONFIG.FIFO_Implementation_wdch {Independent_Clocks_Builtin_FIFO} \
    #CONFIG.FIFO_Implementation_wrch {Independent_Clocks_Distributed_RAM} \
    #CONFIG.Full_Threshold_Assert_Value_axis {511} \
    #CONFIG.Full_Threshold_Assert_Value_rach {15} \
    #CONFIG.Full_Threshold_Assert_Value_wach {15} \
    #CONFIG.Full_Threshold_Assert_Value_wrch {15} \
    #CONFIG.HAS_TKEEP {true} \
    #CONFIG.INTERFACE_TYPE {AXI_STREAM} \
    #CONFIG.Input_Depth_axis {512} \
    #CONFIG.Reset_Type {Asynchronous_Reset} \
    #CONFIG.TDATA_NUM_BYTES {64} \
    #CONFIG.TKEEP_WIDTH {64} \
    #CONFIG.TSTRB_WIDTH {64} \
    #CONFIG.TUSER_WIDTH {0} \
    #] $fifo_cmac_rx_cdc
#
    ## Create instance: fifo_cmac_tx, and set properties
    #set fifo_cmac_tx [ create_bd_cell -type ip -vlnv xilinx.com:ip:fifo_generator fifo_cmac_tx ]
    #set_property -dict [ list \
    #CONFIG.Clock_Type_AXI {Common_Clock} \
    #CONFIG.Empty_Threshold_Assert_Value_axis {510} \
    #CONFIG.Empty_Threshold_Assert_Value_rach {14} \
    #CONFIG.Empty_Threshold_Assert_Value_rdch {1022} \
    #CONFIG.Empty_Threshold_Assert_Value_wach {14} \
    #CONFIG.Empty_Threshold_Assert_Value_wdch {1022} \
    #CONFIG.Empty_Threshold_Assert_Value_wrch {14} \
    #CONFIG.Enable_TLAST {true} \
    #CONFIG.FIFO_Application_Type_axis {Packet_FIFO} \
    #CONFIG.FIFO_Implementation_axis {Common_Clock_Block_RAM} \
    #CONFIG.FIFO_Implementation_rach {Common_Clock_Distributed_RAM} \
    #CONFIG.FIFO_Implementation_rdch {Common_Clock_Builtin_FIFO} \
    #CONFIG.FIFO_Implementation_wach {Common_Clock_Distributed_RAM} \
    #CONFIG.FIFO_Implementation_wdch {Common_Clock_Builtin_FIFO} \
    #CONFIG.FIFO_Implementation_wrch {Common_Clock_Distributed_RAM} \
    #CONFIG.Full_Threshold_Assert_Value_axis {511} \
    #CONFIG.Full_Threshold_Assert_Value_rach {15} \
    #CONFIG.Full_Threshold_Assert_Value_wach {15} \
    #CONFIG.Full_Threshold_Assert_Value_wrch {15} \
    #CONFIG.HAS_TKEEP {true} \
    #CONFIG.INTERFACE_TYPE {AXI_STREAM} \
    #CONFIG.Input_Depth_axis {512} \
    #CONFIG.Reset_Type {Asynchronous_Reset} \
    #CONFIG.TDATA_NUM_BYTES {64} \
    #CONFIG.TKEEP_WIDTH {64} \
    #CONFIG.TSTRB_WIDTH {64} \
    #CONFIG.TUSER_WIDTH {0} \
    #] $fifo_cmac_tx

    # Create instance: cmac_sync, and set properties
    #set cmac_sync [ create_bd_cell -type module -reference cmac_sync cmac_sync ]

    # Create instance: util_vector_logic_0, and set properties
    #set util_vector_logic_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:util_vector_logic util_vector_logic_0 ]
    #set_property -dict [ list \
    #CONFIG.C_OPERATION {not} \
    #CONFIG.C_SIZE {1} \
    #CONFIG.LOGO_FILE {data/sym_notgate.png} \
    #] $util_vector_logic_0

    # Create instance: util_vector_logic_1, and set properties
    set util_vector_logic_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:util_vector_logic util_vector_logic_1 ]
    set_property -dict [ list \
    CONFIG.C_OPERATION {not} \
    CONFIG.C_SIZE {1} \
    CONFIG.LOGO_FILE {data/sym_notgate.png} \
    ] $util_vector_logic_1

    # Create instance: util_vector_logic_2, and set properties
    #set util_vector_logic_2 [ create_bd_cell -type ip -vlnv xilinx.com:ip:util_vector_logic util_vector_logic_2 ]
    #set_property -dict [ list \
    #CONFIG.C_OPERATION {not} \
    #CONFIG.C_SIZE {1} \
    #CONFIG.LOGO_FILE {data/sym_notgate.png} \
    #] $util_vector_logic_2

    # Create instance: xlconstant_0, and set properties
    set xlconstant_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant: xlconstant_0 ]
    set_property -dict [ list \
    CONFIG.CONST_VAL {0} \
    ] $xlconstant_0

    create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant:1.1 xlconstant_1
    set_property -dict [list \
        CONFIG.CONST_WIDTH {12} \
        CONFIG.CONST_VAL {0} \
    ] [get_bd_cells xlconstant_1]
    connect_bd_net [get_bd_pins xlconstant_1/dout] [get_bd_pins cmac_uplus_0/gt_loopback_in]

    # Create instance: smartconnect, and set properties
    set smartconnect [create_bd_cell -type ip -vlnv xilinx.com:ip:smartconnect smartconnect]
    set_property -dict [list \
    CONFIG.NUM_SI {1} \
    CONFIG.NUM_MI {2} \
    ] [get_bd_cells smartconnect]

    # Create interface connections
    connect_bd_intf_net -intf_net S_AXILITE_1 -boundary_type lower [get_bd_intf_ports S_AXILITE] [get_bd_intf_pins smartconnect/S00_AXI]
    connect_bd_intf_net -intf_net smartconnect_M00_AXI -boundary_type lower [get_bd_intf_pins smartconnect/M00_AXI] [get_bd_intf_pins ${cmac_name}/s_axi]
    #connect_bd_intf_net -intf_net S_AXIS_1 [get_bd_intf_ports S_AXIS] [get_bd_intf_pins acc_kernel_tx_cdc/S_AXIS]
    #connect_bd_intf_net -intf_net acc_kernel_tx_cdc_M_AXIS [get_bd_intf_pins acc_kernel_tx_cdc/M_AXIS] [get_bd_intf_pins fifo_cmac_tx/S_AXIS]
    #connect_bd_intf_net -intf_net ${cmac_name}_axis_rx [get_bd_intf_pins ${cmac_name}/axis_rx] [get_bd_intf_pins fifo_cmac_rx_cdc/S_AXIS]
    connect_bd_intf_net -intf_net ${cmac_name}_gt_serial_port [get_bd_intf_ports gt_serial_port] [get_bd_intf_pins ${cmac_name}/gt_serial_port]
    #connect_bd_intf_net -intf_net fifo_cmac_rx_cdc_M_AXIS [get_bd_intf_ports M_AXIS] [get_bd_intf_pins fifo_cmac_rx_cdc/M_AXIS]
    #connect_bd_intf_net -intf_net fifo_cmac_tx_M_AXIS [get_bd_intf_pins ${cmac_name}/axis_tx] [get_bd_intf_pins fifo_cmac_tx/M_AXIS]
    connect_bd_intf_net -intf_net gt_ref_clk_1 [get_bd_intf_ports gt_ref_clk] [get_bd_intf_pins ${cmac_name}/gt_ref_clk]
    #connect_bd_intf_net -intf_net cmac_sync_s_axi [get_bd_intf_pins cmac_sync/s_axi] [get_bd_intf_pins smartconnect/S01_AXI]
    ###### Create port connections ######

    connect_bd_net -net ap_rst_n [get_bd_ports ap_rst_n] [get_bd_pins util_vector_logic_1/Op1] [get_bd_pins smartconnect/aresetn]
    connect_bd_net -net s_aclk_0_1 [get_bd_ports ap_clk] [get_bd_pins ${cmac_name}/s_axi_aclk] [get_bd_pins smartconnect/aclk]
    connect_bd_net -net usr_rx_reset [get_bd_pins ${cmac_name}/usr_rx_reset] [get_bd_pins util_vector_logic_0/Op1] 
    connect_bd_net -net usr_tx_reset [get_bd_pins ${cmac_name}/usr_tx_reset] [get_bd_pins util_vector_logic_2/Op1] 
    #connect_bd_net -net ${cmac_name}_usr_rx_clk [get_bd_pins ${cmac_name}/gt_rxusrclk2] [get_bd_pins fifo_cmac_rx_cdc/s_aclk] [get_bd_pins ${cmac_name}/rx_clk]
    #connect_bd_net -net ${cmac_name}_usr_tx_clk [get_bd_pins acc_kernel_tx_cdc/m_axis_aclk] [get_bd_pins ${cmac_name}/gt_txusrclk2] [get_bd_pins fifo_cmac_tx/s_aclk]
    connect_bd_net -net clk_gt_freerun_net [get_bd_ports clk_gt_freerun] [get_bd_pins ${cmac_name}/init_clk]
    #connect_bd_net -net util_vector_logic_0_Res [get_bd_pins fifo_cmac_rx_cdc/s_aresetn] [get_bd_pins util_vector_logic_0/Res]
    connect_bd_net -net util_vector_logic_1_Res [get_bd_pins util_vector_logic_1/Res] [get_bd_pins ${cmac_name}/sys_reset] [get_bd_pins ${cmac_name}/s_axi_sreset] 
    #connect_bd_net -net util_vector_logic_2_Res [get_bd_pins acc_kernel_tx_cdc/m_axis_aresetn] [get_bd_pins fifo_cmac_tx/s_aresetn] [get_bd_pins util_vector_logic_2/Res]
    connect_bd_net -net xlconstant_0_dout [get_bd_pins xlconstant_0/dout] [get_bd_pins ${cmac_name}/core_rx_reset] [get_bd_pins ${cmac_name}/pm_tick] [get_bd_pins ${cmac_name}/gtwiz_reset_tx_datapath] [get_bd_pins ${cmac_name}/gtwiz_reset_rx_datapath] [get_bd_pins ${cmac_name}/core_tx_reset] [get_bd_pins ${cmac_name}/core_drp_reset] [get_bd_pins ${cmac_name}/drp_clk] [get_bd_pins cmac_uplus_0/drp_en] [get_bd_pins cmac_uplus_0/drp_we]

    #connect_bd_net -net cmac_stat_rx_aligned [get_bd_pins ${cmac_name}/stat_rx_aligned] [get_bd_pins cmac_sync/stat_rx_aligned]

    ###### Create address segments ######

    #assign_bd_address -target_address_space [get_bd_addr_spaces S_AXILITE] [get_bd_addr_segs ${cmac_name}/s_axi/Reg0] -force
    #assign_bd_address -target_address_space [get_bd_addr_spaces cmac_sync/s_axi] [get_bd_addr_segs ${cmac_name}/s_axi/Reg0] -force
    #assign_bd_address -offset 0x00000000 -range 0x00010000 -target_address_space [get_bd_addr_spaces S_AXILITE]    [get_bd_addr_segs ernic_0/s_axi_lite/reg0] -force
    #assign_bd_address -target_address_space [get_bd_addr_spaces S_AXILITE] [get_bd_addr_segs ${cmac_name}/s_axi/Reg0] -force
    ###### Validate and save the IPI ######

    #validate_bd_design
    save_bd_design

}

create_cmac 

proc create_ernic {} {

    #instantiate ERNIC
    set ernic_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:ernic:3.0 ernic_0 ]
    set_property -dict [ list \
        CONFIG.C_ADDR_WIDTH {64} \
        CONFIG.C_EN_DEBUG_PORTS {1} \
        CONFIG.C_EN_FLOW_CTRL {0} \
        CONFIG.C_EN_INITIATOR_LITE {1} \
        CONFIG.C_EN_NVMOF_HW_HNDSHK {1} \
        CONFIG.C_EN_WR_RETRY_DATA_BUF {0} \
        CONFIG.C_MAX_SGL_DEPTH {512} \
        CONFIG.C_MAX_WR_RETRY_DATA_BUF_DEPTH {512} \
        CONFIG.C_M_AXI_ADDR_WIDTH {64} \
        CONFIG.C_NUM_QP {64} \
    ] $ernic_0

    #create and connect S_AXILITE 
    #set S_AXI_LITE_CLK_FREQ 100000000
    #create_bd_port -dir I -type clk -freq_hz $S_AXI_LITE_CLK_FREQ s_axi_lite_aclk
    connect_bd_net [get_bd_pins /ernic_0/s_axi_lite_aclk    ] [get_bd_ports ap_clk      ]
    #create_bd_port -dir I -type rst s_axi_lite_aresetn
    connect_bd_net [get_bd_pins /ernic_0/s_axi_lite_aresetn ] [get_bd_ports ap_rst_n    ]
    #create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 s_axi_lite
    #set_property -dict [\
    #    list CONFIG.PROTOCOL                [get_property CONFIG.PROTOCOL               [get_bd_intf_pins ernic_0/s_axi_lite]] \
    #        CONFIG.HAS_BURST               [get_property CONFIG.HAS_BURST              [get_bd_intf_pins ernic_0/s_axi_lite]] \
    #        CONFIG.HAS_LOCK                [get_property CONFIG.HAS_LOCK               [get_bd_intf_pins ernic_0/s_axi_lite]] \
    #        CONFIG.HAS_PROT                [get_property CONFIG.HAS_PROT               [get_bd_intf_pins ernic_0/s_axi_lite]] \
    #        CONFIG.HAS_CACHE               [get_property CONFIG.HAS_CACHE              [get_bd_intf_pins ernic_0/s_axi_lite]] \
    #        CONFIG.HAS_QOS                 [get_property CONFIG.HAS_QOS                [get_bd_intf_pins ernic_0/s_axi_lite]] \
    #        CONFIG.HAS_REGION              [get_property CONFIG.HAS_REGION             [get_bd_intf_pins ernic_0/s_axi_lite]] \
    #        CONFIG.SUPPORTS_NARROW_BURST   [get_property CONFIG.SUPPORTS_NARROW_BURST  [get_bd_intf_pins ernic_0/s_axi_lite]] \
    #        CONFIG.MAX_BURST_LENGTH        [get_property CONFIG.MAX_BURST_LENGTH       [get_bd_intf_pins ernic_0/s_axi_lite]] \
    #] [get_bd_intf_ports s_axi_lite]
    #connect_bd_intf_net [get_bd_intf_pins ernic_0/s_axi_lite] [get_bd_intf_ports s_axi_lite]
    connect_bd_intf_net [get_bd_intf_pins smartconnect/M01_AXI] [get_bd_intf_pins ${ernic_0}/s_axi_lite]
    
    #create and connect m_axi_ports
    #set M_AXI_CLK_FREQ 200000000
    #create_bd_port -dir I -type clk -freq_hz $M_AXI_CLK_FREQ m_axi_aclk
    connect_bd_net [get_bd_pins /ernic_0/m_axi_aclk] [get_bd_ports ap_clk]
    #create_bd_port -dir I -type rst m_axi_aresetn
    connect_bd_net [get_bd_pins /ernic_0/m_axi_aresetn] [get_bd_ports ap_rst_n]

    create_bd_intf_port -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 qp_mgr_m_axi
    set_property -dict [ \
        list CONFIG.DATA_WIDTH              [get_property CONFIG.DATA_WIDTH             [get_bd_intf_pins ernic_0/qp_mgr_m_axi]] \
            CONFIG.HAS_QOS                  [get_property CONFIG.HAS_QOS                [get_bd_intf_pins ernic_0/qp_mgr_m_axi]] \
            CONFIG.HAS_REGION               [get_property CONFIG.HAS_REGION             [get_bd_intf_pins ernic_0/qp_mgr_m_axi]] \
            CONFIG.NUM_READ_OUTSTANDING     [get_property CONFIG.NUM_READ_OUTSTANDING   [get_bd_intf_pins ernic_0/qp_mgr_m_axi]] \
            CONFIG.NUM_WRITE_OUTSTANDING    [get_property CONFIG.NUM_WRITE_OUTSTANDING  [get_bd_intf_pins ernic_0/qp_mgr_m_axi]] \
    ] [get_bd_intf_ports qp_mgr_m_axi]
    connect_bd_intf_net [get_bd_intf_pins ernic_0/qp_mgr_m_axi] [get_bd_intf_ports qp_mgr_m_axi]

    create_bd_intf_port -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 resp_hndler_m_axi
    set_property -dict [ \
        list CONFIG.DATA_WIDTH              [get_property CONFIG.DATA_WIDTH             [get_bd_intf_pins ernic_0/resp_hndler_m_axi]] \
            CONFIG.HAS_QOS                  [get_property CONFIG.HAS_QOS                [get_bd_intf_pins ernic_0/resp_hndler_m_axi]] \
            CONFIG.HAS_REGION               [get_property CONFIG.HAS_REGION             [get_bd_intf_pins ernic_0/resp_hndler_m_axi]] \
            CONFIG.NUM_READ_OUTSTANDING     [get_property CONFIG.NUM_READ_OUTSTANDING   [get_bd_intf_pins ernic_0/resp_hndler_m_axi]] \
            CONFIG.NUM_WRITE_OUTSTANDING    [get_property CONFIG.NUM_WRITE_OUTSTANDING  [get_bd_intf_pins ernic_0/resp_hndler_m_axi]] \
    ] [get_bd_intf_ports resp_hndler_m_axi]
    connect_bd_intf_net [get_bd_intf_pins ernic_0/resp_hndler_m_axi] [get_bd_intf_ports resp_hndler_m_axi]

    create_bd_intf_port -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 rx_pkt_hndler_ddr_m_axi
    set_property -dict [ \
        list CONFIG.DATA_WIDTH              [get_property CONFIG.DATA_WIDTH             [get_bd_intf_pins ernic_0/rx_pkt_hndler_ddr_m_axi]] \
            CONFIG.HAS_QOS                  [get_property CONFIG.HAS_QOS                [get_bd_intf_pins ernic_0/rx_pkt_hndler_ddr_m_axi]] \
            CONFIG.HAS_REGION               [get_property CONFIG.HAS_REGION             [get_bd_intf_pins ernic_0/rx_pkt_hndler_ddr_m_axi]] \
            CONFIG.NUM_READ_OUTSTANDING     [get_property CONFIG.NUM_READ_OUTSTANDING   [get_bd_intf_pins ernic_0/rx_pkt_hndler_ddr_m_axi]] \
            CONFIG.NUM_WRITE_OUTSTANDING    [get_property CONFIG.NUM_WRITE_OUTSTANDING  [get_bd_intf_pins ernic_0/rx_pkt_hndler_ddr_m_axi]] \
    ] [get_bd_intf_ports rx_pkt_hndler_ddr_m_axi]
    connect_bd_intf_net [get_bd_intf_pins ernic_0/rx_pkt_hndler_ddr_m_axi] [get_bd_intf_ports rx_pkt_hndler_ddr_m_axi]


    create_bd_intf_port -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 rx_pkt_hndler_rdrsp_m_axi
    set_property -dict [ \
        list CONFIG.DATA_WIDTH              [get_property CONFIG.DATA_WIDTH             [get_bd_intf_pins ernic_0/rx_pkt_hndler_rdrsp_m_axi]] \
            CONFIG.HAS_QOS                  [get_property CONFIG.HAS_QOS                [get_bd_intf_pins ernic_0/rx_pkt_hndler_rdrsp_m_axi]] \
            CONFIG.HAS_REGION               [get_property CONFIG.HAS_REGION             [get_bd_intf_pins ernic_0/rx_pkt_hndler_rdrsp_m_axi]] \
            CONFIG.NUM_READ_OUTSTANDING     [get_property CONFIG.NUM_READ_OUTSTANDING   [get_bd_intf_pins ernic_0/rx_pkt_hndler_rdrsp_m_axi]] \
            CONFIG.NUM_WRITE_OUTSTANDING    [get_property CONFIG.NUM_WRITE_OUTSTANDING  [get_bd_intf_pins ernic_0/rx_pkt_hndler_rdrsp_m_axi]] \
    ] [get_bd_intf_ports rx_pkt_hndler_rdrsp_m_axi]
    connect_bd_intf_net [get_bd_intf_pins ernic_0/rx_pkt_hndler_rdrsp_m_axi] [get_bd_intf_ports rx_pkt_hndler_rdrsp_m_axi]


    create_bd_intf_port -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 wqe_proc_top_m_axi
    set_property -dict [ \
        list CONFIG.DATA_WIDTH              [get_property CONFIG.DATA_WIDTH             [get_bd_intf_pins ernic_0/wqe_proc_top_m_axi]] \
            CONFIG.HAS_QOS                  [get_property CONFIG.HAS_QOS                [get_bd_intf_pins ernic_0/wqe_proc_top_m_axi]] \
            CONFIG.HAS_REGION               [get_property CONFIG.HAS_REGION             [get_bd_intf_pins ernic_0/wqe_proc_top_m_axi]] \
            CONFIG.NUM_READ_OUTSTANDING     [get_property CONFIG.NUM_READ_OUTSTANDING   [get_bd_intf_pins ernic_0/wqe_proc_top_m_axi]] \
            CONFIG.NUM_WRITE_OUTSTANDING    [get_property CONFIG.NUM_WRITE_OUTSTANDING  [get_bd_intf_pins ernic_0/wqe_proc_top_m_axi]] \
    ] [get_bd_intf_ports wqe_proc_top_m_axi]
    connect_bd_intf_net [get_bd_intf_pins ernic_0/wqe_proc_top_m_axi] [get_bd_intf_ports wqe_proc_top_m_axi]


    create_bd_intf_port -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 wqe_proc_wr_ddr_m_axi
    set_property -dict [ \
        list CONFIG.DATA_WIDTH              [get_property CONFIG.DATA_WIDTH             [get_bd_intf_pins ernic_0/wqe_proc_wr_ddr_m_axi]] \
            CONFIG.HAS_QOS                  [get_property CONFIG.HAS_QOS                [get_bd_intf_pins ernic_0/wqe_proc_wr_ddr_m_axi]] \
            CONFIG.HAS_REGION               [get_property CONFIG.HAS_REGION             [get_bd_intf_pins ernic_0/wqe_proc_wr_ddr_m_axi]] \
            CONFIG.NUM_READ_OUTSTANDING     [get_property CONFIG.NUM_READ_OUTSTANDING   [get_bd_intf_pins ernic_0/wqe_proc_wr_ddr_m_axi]] \
            CONFIG.NUM_WRITE_OUTSTANDING    [get_property CONFIG.NUM_WRITE_OUTSTANDING  [get_bd_intf_pins ernic_0/wqe_proc_wr_ddr_m_axi]] \
    ] [get_bd_intf_ports wqe_proc_wr_ddr_m_axi]
    connect_bd_intf_net [get_bd_intf_pins ernic_0/wqe_proc_wr_ddr_m_axi] [get_bd_intf_ports wqe_proc_wr_ddr_m_axi]


    #CMAC interfaces
    #set CMAC_FREQ 322265625
    #create_bd_port -dir I -type clk -freq_hz $CMAC_FREQ cmac_clk
    #connect_bd_net [get_bd_pins /ernic_0/cmac_rx_clk] [get_bd_ports cmac_clk]
    #connect_bd_net [get_bd_ports cmac_clk           ] [get_bd_pins ernic_0/cmac_tx_clk]
#
    #create_bd_port -dir I -type rst cmac_rst
    #set_property CONFIG.POLARITY [get_property CONFIG.POLARITY [get_bd_pins ernic_0/cmac_rx_rst]] [get_bd_ports cmac_rst]
    #connect_bd_net [get_bd_pins /ernic_0/cmac_rx_rst] [get_bd_ports cmac_rst]
    #connect_bd_net [get_bd_ports cmac_rst           ] [get_bd_pins ernic_0/cmac_tx_rst]
#
    #create_bd_intf_port -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 cmac_tx
    #connect_bd_intf_net [get_bd_intf_pins ernic_0/cmac_m_axis] [get_bd_intf_ports cmac_tx]
#
    #create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 cmac_rx
    #set_property -dict [ \
    #    list CONFIG.TDATA_NUM_BYTES         [get_property CONFIG.TDATA_NUM_BYTES    [get_bd_intf_pins ernic_0/roce_cmac_s_axis]] \
    #        CONFIG.TUSER_WIDTH              [get_property CONFIG.TUSER_WIDTH        [get_bd_intf_pins ernic_0/roce_cmac_s_axis]] \
    #        CONFIG.HAS_TREADY               [get_property CONFIG.HAS_TREADY         [get_bd_intf_pins ernic_0/roce_cmac_s_axis]] \
    #        CONFIG.HAS_TKEEP                [get_property CONFIG.HAS_TKEEP          [get_bd_intf_pins ernic_0/roce_cmac_s_axis]] \
    #        CONFIG.HAS_TLAST                [get_property CONFIG.HAS_TLAST          [get_bd_intf_pins ernic_0/roce_cmac_s_axis]] \
    #] [get_bd_intf_ports cmac_rx]
    #connect_bd_intf_net [get_bd_intf_pins ernic_0/roce_cmac_s_axis] [get_bd_intf_ports cmac_rx]

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
    connect_bd_net      [get_bd_ports ap_clk    ] [get_bd_pins axis_reg_cq_pi_db/aclk       ]
    connect_bd_net      [get_bd_ports ap_rst_n ] [get_bd_pins axis_reg_cq_pi_db/aresetn    ]

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
    connect_bd_net      [get_bd_ports ap_clk    ] [get_bd_pins      axis_reg_rq_pi_db/aclk      ]
    connect_bd_net      [get_bd_ports ap_rst_n ] [get_bd_pins      axis_reg_rq_pi_db/aresetn   ]

    #RQ CI doorbell
    create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 rq_ci_db
    set_property -dict [list CONFIG.TDATA_NUM_BYTES {6}] [get_bd_intf_ports rq_ci_db]
    create_bd_cell -type ip -vlnv xilinx.com:ip:axis_register_slice:1.1 axis_reg_rq_ci_db
    set_property -dict [list CONFIG.TDATA_NUM_BYTES.VALUE_SRC USER  ] [get_bd_cells axis_reg_rq_ci_db]
    set_property -dict [list CONFIG.TDATA_NUM_BYTES {6}             ] [get_bd_cells axis_reg_rq_ci_db]

    connect_bd_intf_net [get_bd_intf_ports rq_ci_db ] [get_bd_intf_pins axis_reg_rq_ci_db/S_AXIS]
    connect_bd_net      [get_bd_ports ap_clk    ] [get_bd_pins axis_reg_rq_ci_db/aclk       ]
    connect_bd_net      [get_bd_ports ap_rst_n ] [get_bd_pins axis_reg_rq_ci_db/aresetn    ]

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
    connect_bd_net      [get_bd_ports ap_clk    ] [get_bd_pins axis_reg_sq_pi_db/aclk       ]
    connect_bd_net      [get_bd_ports ap_rst_n ] [get_bd_pins axis_reg_sq_pi_db/aresetn    ]

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
    #create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant:1.1 xlconstant_00
    #set_property -dict [list CONFIG.CONST_VAL {0}] [get_bd_cells xlconstant_00]
    connect_bd_net [get_bd_pins xlconstant_0/dout] [get_bd_pins ernic_0/ieth_immdt_axis_trdy]
    connect_bd_net [get_bd_pins xlconstant_0/dout] [get_bd_pins ernic_0/non_roce_dma_m_axis_tready]
    connect_bd_net [get_bd_pins xlconstant_0/dout] [get_bd_pins ernic_0/non_roce_dma_s_axis_tvalid]
    connect_bd_net [get_bd_pins xlconstant_0/dout] [get_bd_pins ernic_0/non_roce_cmac_s_axis_tvalid]
    save_bd_design
    #connect ERNIC to cmac
    #rx data clock and reset
    #import pkt_filter ip
    set_property  ip_repo_paths  . [current_project]
    update_ip_catalog
    create_bd_cell -type ip -vlnv danielep.org:user:RDMA_pkt_filter_one_port:1.0 RDMA_pkt_filter_one_0
    connect_bd_intf_net [get_bd_intf_pins cmac_uplus_0/axis_rx] [get_bd_intf_pins RDMA_pkt_filter_one_0/s_axis]
    connect_bd_net      [get_bd_pins cmac_uplus_0/gt_rxusrclk2] [get_bd_pins RDMA_pkt_filter_one_0/clk]
    create_bd_cell -type ip -vlnv xilinx.com:ip:util_vector_logic:2.0 inverter_0
    set_property -dict [ \
        list CONFIG.C_SIZE {1} \
        CONFIG.C_OPERATION {not} \
    ] [get_bd_cells inverter_0 ]
    connect_bd_net [get_bd_pins cmac_uplus_0/usr_rx_reset] [get_bd_pins inverter_0/Op1]
    connect_bd_net [get_bd_pins inverter_0/Res] [get_bd_pins RDMA_pkt_filter_one_0/rst]
    # fifo to handle cdc between RDMA pkt filter and ERNIC (apparently (non_)roce_cmac_s_axis belong to clock domain of m_axi_aclk)
    # Create instance: fifo_generator_0, and set properties
    set fifo_generator_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:fifo_generator:13.2 fifo_generator_0 ]
    set_property -dict [ list \
    CONFIG.Clock_Type_AXI {Independent_Clock} \
    CONFIG.Empty_Threshold_Assert_Value_axis {509} \
    CONFIG.Empty_Threshold_Assert_Value_rach {13} \
    CONFIG.Empty_Threshold_Assert_Value_rdch {1018} \
    CONFIG.Empty_Threshold_Assert_Value_wach {13} \
    CONFIG.Empty_Threshold_Assert_Value_wdch {1018} \
    CONFIG.Empty_Threshold_Assert_Value_wrch {13} \
    CONFIG.Enable_TLAST {true} \
    CONFIG.FIFO_Application_Type_axis {Packet_FIFO} \
    CONFIG.FIFO_Implementation_axis {Independent_Clocks_Block_RAM} \
    CONFIG.FIFO_Implementation_rach {Independent_Clocks_Distributed_RAM} \
    CONFIG.FIFO_Implementation_rdch {Independent_Clocks_Builtin_FIFO} \
    CONFIG.FIFO_Implementation_wach {Independent_Clocks_Distributed_RAM} \
    CONFIG.FIFO_Implementation_wdch {Independent_Clocks_Builtin_FIFO} \
    CONFIG.FIFO_Implementation_wrch {Independent_Clocks_Distributed_RAM} \
    CONFIG.Full_Threshold_Assert_Value_axis {511} \
    CONFIG.Full_Threshold_Assert_Value_rach {15} \
    CONFIG.Full_Threshold_Assert_Value_wach {15} \
    CONFIG.Full_Threshold_Assert_Value_wrch {15} \
    CONFIG.HAS_TKEEP {true} \
    CONFIG.INTERFACE_TYPE {AXI_STREAM} \
    CONFIG.Input_Depth_axis {512} \
    CONFIG.Reset_Type {Asynchronous_Reset} \
    CONFIG.TDATA_NUM_BYTES {64} \
    CONFIG.TKEEP_WIDTH {64} \
    CONFIG.TSTRB_WIDTH {64} \
    CONFIG.TUSER_WIDTH {1} \
    ] $fifo_generator_0
    
    connect_bd_intf_net [get_bd_intf_pins RDMA_pkt_filter_one_0/roce_m_axism_axis   ] [get_bd_intf_pins fifo_generator_0/S_AXIS]
    connect_bd_intf_net [get_bd_intf_pins fifo_generator_0/M_AXIS                   ] [get_bd_intf_pins ernic_0/roce_cmac_s_axis]
    connect_bd_net      [get_bd_pins fifo_generator_0/s_aclk                        ] [get_bd_pins cmac_uplus_0/gt_rxusrclk2]
    connect_bd_net      [get_bd_pins fifo_generator_0/s_aresetn                     ] [get_bd_pins inverter_0/Res]
    connect_bd_net      [get_bd_ports ap_clk                                        ] [get_bd_pins fifo_generator_0/m_aclk]
    #pkt_filter to ernic rx
    connect_bd_intf_net [get_bd_intf_pins RDMA_pkt_filter_one_0/roce_m_axism_axis] [get_bd_intf_pins ernic_0/roce_cmac_s_axis]
    connect_bd_net      [get_bd_pins cmac_uplus_0/gt_rxusrclk2] [get_bd_pins cmac_uplus_0/rx_clk] [get_bd_pins ernic_0/cmac_rx_clk]
    connect_bd_net      [get_bd_pins cmac_uplus_0/usr_rx_reset] [get_bd_pins ernic_0/cmac_rx_rst]
    #tx data clock and reset
    connect_bd_intf_net [get_bd_intf_pins cmac_uplus_0/axis_tx] [get_bd_intf_pins ernic_0/cmac_m_axis]
    connect_bd_net [get_bd_pins cmac_uplus_0/gt_txusrclk2] [get_bd_pins ernic_0/cmac_tx_clk]
    connect_bd_net [get_bd_pins cmac_uplus_0/usr_tx_reset] [get_bd_pins ernic_0/cmac_tx_rst]
    #ctrl flow
    connect_bd_net [get_bd_pins cmac_uplus_0/ctl_tx_pause_req   ] [get_bd_pins ernic_0/ctl_tx_pause_req     ]
    connect_bd_net [get_bd_pins cmac_uplus_0/ctl_tx_resend_pause] [get_bd_pins ernic_0/ctl_tx_resend_pause  ]
    connect_bd_net [get_bd_pins cmac_uplus_0/stat_rx_pause_req  ] [get_bd_pins ernic_0/stat_rx_pause_req    ]
    #Addresses
    assign_bd_address
    # for axi_lite based on doc 3.1 registers spans from 0x00 to 0x202B0 + ((255) x 0x0100) = 0x302B0
    assign_bd_address -offset 0x00040000 -range 0x00040000 -target_address_space [get_bd_addr_spaces /s_axi_lite]    [get_bd_addr_segs ernic_0/s_axi_lite/reg0] -force
    assign_bd_address -offset 0x00000000 -range 0x00010000 -target_address_space [get_bd_addr_spaces /s_axi_lite]    [get_bd_addr_segs cmac_uplus_0/s_axi/Reg ] -force
    # for all other m_axi interfaces exiting from ERNIC allocate the max memory space size
    assign_bd_address -offset 0x00000000 -range 0x1000000000 -target_address_space [get_bd_addr_spaces /ernic_0/m_axi] [get_bd_addr_segs /qp_mgr_m_axi/Reg]               -force
    assign_bd_address -offset 0x00000000 -range 0x1000000000 -target_address_space [get_bd_addr_spaces /ernic_0/m_axi] [get_bd_addr_segs /resp_hndler_m_axi/Reg]          -force
    assign_bd_address -offset 0x00000000 -range 0x1000000000 -target_address_space [get_bd_addr_spaces /ernic_0/m_axi] [get_bd_addr_segs /rx_pkt_hndler_ddr_m_axi/Reg]    -force
    assign_bd_address -offset 0x00000000 -range 0x1000000000 -target_address_space [get_bd_addr_spaces /ernic_0/m_axi] [get_bd_addr_segs /rx_pkt_hndler_rdrsp_m_axi/Reg]  -force
    assign_bd_address -offset 0x00000000 -range 0x1000000000 -target_address_space [get_bd_addr_spaces /ernic_0/m_axi] [get_bd_addr_segs /wqe_proc_top_m_axi/Reg]         -force
    assign_bd_address -offset 0x00000000 -range 0x1000000000 -target_address_space [get_bd_addr_spaces /ernic_0/m_axi] [get_bd_addr_segs /wqe_proc_wr_ddr_m_axi/Reg]      -force
}
create_ernic 
save_bd_design

validate_bd_design
#save_bd_design
#
generate_target all [get_files  /home/danielep/ACCL_ERNIC/demo/build/ERNIC/project_ernic/project_ernic.srcs/sources_1/bd/design_ernic_ip/design_ernic_ip.bd]
catch { config_ip_cache -export [get_ips -all design_ernic_ip_ernic_0_0] }
catch { config_ip_cache -export [get_ips -all design_ernic_ip_axis_reg_cq_pi_db_0] }
catch { config_ip_cache -export [get_ips -all design_ernic_ip_axis_reg_rq_pi_db_0] }
catch { config_ip_cache -export [get_ips -all design_ernic_ip_axis_reg_rq_ci_db_0] }
catch { config_ip_cache -export [get_ips -all design_ernic_ip_axis_reg_sq_pi_db_0] }
export_ip_user_files -of_objects [get_files /home/danielep/ACCL_ERNIC/demo/build/ERNIC/project_ernic/project_ernic.srcs/sources_1/bd/design_ernic_ip/design_ernic_ip.bd] -no_script -sync -force -quiet
create_ip_run [get_files -of_objects [get_fileset sources_1] /home/danielep/ACCL_ERNIC/demo/build/ERNIC/project_ernic/project_ernic.srcs/sources_1/bd/design_ernic_ip/design_ernic_ip.bd]
make_wrapper -files [get_files /home/danielep/ACCL_ERNIC/demo/build/ERNIC/project_ernic/project_ernic.srcs/sources_1/bd/design_ernic_ip/design_ernic_ip.bd] -top
add_files -norecurse /home/danielep/ACCL_ERNIC/demo/build/ERNIC/project_ernic/project_ernic.gen/sources_1/bd/design_ernic_ip/hdl/design_ernic_ip_wrapper.v
launch_runs design_ernic_ip_ernic_0_0_synth_1 design_ernic_ip_axis_reg_cq_pi_db_0_synth_1 design_ernic_ip_axis_reg_rq_pi_db_0_synth_1 design_ernic_ip_axis_reg_rq_ci_db_0_synth_1 design_ernic_ip_axis_reg_sq_pi_db_0_synth_1 -jobs 16
wait 