#testciao
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

namespace eval _tcl {
proc get_script_folder {} {
   set script_path [file normalize [info script]]
   set script_folder [file dirname $script_path]
   return $script_folder
}
}
variable script_folder
set script_folder [_tcl::get_script_folder]

################################################################
# Check if script is running in correct Vivado version.
################################################################
set scripts_vivado_version 2020.2
set current_vivado_version [version -short]

if { [string first $scripts_vivado_version $current_vivado_version] == -1 } {
   puts ""
   catch {common::send_gid_msg -ssname BD::TCL -id 2041 -severity "ERROR" "This script was generated using Vivado <$scripts_vivado_version> and is being run in <$current_vivado_version> of Vivado. Please run the script in Vivado <$scripts_vivado_version> then open the design in Vivado <$current_vivado_version>. Upgrade the design by running \"Tools => Report => Report IP Status...\", then run write_bd_tcl to create an updated script."}

   return 1
}

################################################################
# START
################################################################

# To test this script, run the following commands from Vivado Tcl console:
# source ccl_offload_bd_script.tcl

# If there is no project opened, this script will create a
# project, but make sure you do not have an existing project
# <./myproj/project_1.xpr> in the current working folder.

set list_projs [get_projects -quiet]
if { $list_projs eq "" } {
   create_project project_1 myproj -part xcu280-fsvh2892-2L-e
   set_property BOARD_PART xilinx.com:au280:part0:1.1 [current_project]
}


# CHANGE DESIGN NAME HERE
variable design_name
set design_name ccl_offload_bd

# If you do not already have an existing IP Integrator design open,
# you can create a design using the following command:
#    create_bd_design $design_name

# Creating design if needed
set errMsg ""
set nRet 0

set cur_design [current_bd_design -quiet]
set list_cells [get_bd_cells -quiet]

if { ${design_name} eq "" } {
   # USE CASES:
   #    1) Design_name not set

   set errMsg "Please set the variable <design_name> to a non-empty value."
   set nRet 1

} elseif { ${cur_design} ne "" && ${list_cells} eq "" } {
   # USE CASES:
   #    2): Current design opened AND is empty AND names same.
   #    3): Current design opened AND is empty AND names diff; design_name NOT in project.
   #    4): Current design opened AND is empty AND names diff; design_name exists in project.

   if { $cur_design ne $design_name } {
      common::send_gid_msg -ssname BD::TCL -id 2001 -severity "INFO" "Changing value of <design_name> from <$design_name> to <$cur_design> since current design is empty."
      set design_name [get_property NAME $cur_design]
   }
   common::send_gid_msg -ssname BD::TCL -id 2002 -severity "INFO" "Constructing design in IPI design <$cur_design>..."

} elseif { ${cur_design} ne "" && $list_cells ne "" && $cur_design eq $design_name } {
   # USE CASES:
   #    5) Current design opened AND has components AND same names.

   set errMsg "Design <$design_name> already exists in your project, please set the variable <design_name> to another value."
   set nRet 1
} elseif { [get_files -quiet ${design_name}.bd] ne "" } {
   # USE CASES: 
   #    6) Current opened design, has components, but diff names, design_name exists in project.
   #    7) No opened design, design_name exists in project.

   set errMsg "Design <$design_name> already exists in your project, please set the variable <design_name> to another value."
   set nRet 2

} else {
   # USE CASES:
   #    8) No opened design, design_name not in project.
   #    9) Current opened design, has components, but diff names, design_name not in project.

   common::send_gid_msg -ssname BD::TCL -id 2003 -severity "INFO" "Currently there is no design <$design_name> in project, so creating one..."

   create_bd_design $design_name

   common::send_gid_msg -ssname BD::TCL -id 2004 -severity "INFO" "Making design <$design_name> as current_bd_design."
   current_bd_design $design_name

}

common::send_gid_msg -ssname BD::TCL -id 2005 -severity "INFO" "Currently the variable <design_name> is equal to \"$design_name\"."

if { $nRet != 0 } {
   catch {common::send_gid_msg -ssname BD::TCL -id 2006 -severity "ERROR" $errMsg}
   return $nRet
}

set bCheckIPsPassed 1
##################################################################
# CHECK IPs
##################################################################
set bCheckIPs 1
if { $bCheckIPs == 1 } {
   set list_check_ips "\ 
xilinx.com:ip:axis_switch:1.1\
xilinx.com:hls:reduce_arith:1.0\
xilinx.com:ip:util_vector_logic:2.0\
xilinx.com:ip:util_reduced_logic:2.0\
xilinx.com:ip:axis_data_fifo:2.0\
xilinx.com:ip:mdm:3.2\
xilinx.com:ip:microblaze:11.0\
xilinx.com:ip:proc_sys_reset:5.0\
xilinx.com:ip:axi_datamover:5.1\
xilinx.com:ip:axis_dwidth_converter:1.1\
xilinx.com:ip:axis_subset_converter:1.1\
xilinx.com:hls:vnx_depacketizer:1.0\
xilinx.com:hls:vnx_packetizer:1.0\
xilinx.com:ip:axi_bram_ctrl:4.1\
xilinx.com:ip:blk_mem_gen:8.4\
xilinx.com:ip:axi_crossbar:2.1\
xilinx.com:ip:axi_gpio:2.0\
xilinx.com:ip:axi_register_slice:2.1\
xilinx.com:hls:hostctrl:1.0\
xilinx.com:ip:xlslice:1.0\
xilinx.com:ip:lmb_bram_if_cntlr:4.0\
xilinx.com:ip:lmb_v10:3.0\
"

   set list_ips_missing ""
   common::send_gid_msg -ssname BD::TCL -id 2011 -severity "INFO" "Checking if the following IPs exist in the project's IP catalog: $list_check_ips ."

   foreach ip_vlnv $list_check_ips {
      set ip_obj [get_ipdefs -all $ip_vlnv]
      if { $ip_obj eq "" } {
         lappend list_ips_missing $ip_vlnv
      }
   }

   if { $list_ips_missing ne "" } {
      catch {common::send_gid_msg -ssname BD::TCL -id 2012 -severity "ERROR" "The following IPs are not found in the IP Catalog:\n  $list_ips_missing\n\nResolution: Please add the repository containing the IP(s) to the project." }
      set bCheckIPsPassed 0
   }

}

if { $bCheckIPsPassed != 1 } {
  common::send_gid_msg -ssname BD::TCL -id 2023 -severity "WARNING" "Will not continue with creation of design due to the error(s) above."
  return 3
}

##################################################################
# DESIGN PROCs
##################################################################
# Procedure to create axis_switch_0
proc create_hier_cell_axis_switch_0 { parentCell nameHier } {

   variable script_folder

   if { $parentCell eq "" || $nameHier eq "" } {
      catch {common::send_gid_msg -ssname BD::TCL -id 2092 -severity "ERROR" "create_hier_cell_axis_switch_0() - Empty argument(s)!"}
      return
   }

   # Get object for parentCell
   set parentObj [get_bd_cells $parentCell]
   if { $parentObj == "" } {
      catch {common::send_gid_msg -ssname BD::TCL -id 2090 -severity "ERROR" "Unable to find parent cell <$parentCell>!"}
      return
   }

   # Make sure parentObj is hier blk
   set parentType [get_property TYPE $parentObj]
   if { $parentType ne "hier" } {
      catch {common::send_gid_msg -ssname BD::TCL -id 2091 -severity "ERROR" "Parent <$parentObj> has TYPE = <$parentType>. Expected to be <hier>."}
      return
   }

   # Save current instance; Restore later
   set oldCurInst [current_bd_instance .]

   # Set parent object as current
   current_bd_instance $parentObj

   # Create cell and set as current instance
   set hier_obj [create_bd_cell -type hier $nameHier]
   #set hier_obj [create_bd_cell -type hier axis_switch_00]

   current_bd_instance $hier_obj

   set masters 6
   set slaves 5
   # Create interface pins
   for {set i 0} {$i < $slaves} {incr i} {
   create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 S0${i}_AXIS
   }
   for {set i 0} {$i < $masters} {incr i} {
   create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 M0${i}_AXIS
   }
   create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 scenario_V


   # Create pins
   create_bd_pin -dir I -type clk aclk
   create_bd_pin -dir I -type rst aresetn
   create_bd_pin -dir O -from 4 -to 0 s_decode_err

   set axis_switch_internal_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_switch:1.1 axis_switch_internal_0 ]
   set_property -dict [ list \
   CONFIG.DECODER_REG {0} \
   CONFIG.HAS_TKEEP {1} \
   CONFIG.HAS_TLAST {1} \
   CONFIG.NUM_MI $masters \
   CONFIG.NUM_SI $slaves \
   CONFIG.ROUTING_MODE {0} \
   CONFIG.TDATA_NUM_BYTES {64} \
   ] $axis_switch_internal_0

   create_bd_cell -type ip -vlnv xilinx.com:hls:tdest_creation:1.0 tdest_creation_0
   for {set i 0} {$i < $slaves} {incr i} {
      #create a subset converter for each Slave of the axis_switch0
      create_bd_cell -type ip -vlnv xilinx.com:ip:axis_subset_converter:1.1 axis_subset_converter_${i}
      set_property -dict [list \
         CONFIG.M_HAS_TSTRB.VALUE_SRC PROPAGATED \
         CONFIG.S_HAS_TLAST.VALUE_SRC PROPAGATED \
         CONFIG.S_HAS_TKEEP.VALUE_SRC PROPAGATED \
         CONFIG.S_HAS_TSTRB.VALUE_SRC PROPAGATED \
         CONFIG.M_TDEST_WIDTH.VALUE_SRC USER \
         CONFIG.S_TDEST_WIDTH.VALUE_SRC USER \
         CONFIG.M_HAS_TKEEP.VALUE_SRC PROPAGATED \
         CONFIG.M_HAS_TLAST.VALUE_SRC PROPAGATED \
         CONFIG.S_TDEST_WIDTH {3} \
         CONFIG.M_TDEST_WIDTH {3} \
         CONFIG.TDEST_REMAP {tdest[2:0]} \
         CONFIG.TLAST_REMAP {1'b0} \
      ] [get_bd_cells axis_subset_converter_${i}]

      connect_bd_net [get_bd_pins aclk    ] [get_bd_pins axis_subset_converter_${i}/aclk]
      connect_bd_net [get_bd_pins aresetn ] [get_bd_pins axis_subset_converter_${i}/aresetn]
      #connect 
      connect_bd_intf_net [get_bd_intf_pins axis_subset_converter_${i}/M_AXIS ] [get_bd_intf_pins axis_switch_internal_0/S0${i}_AXIS]
      connect_bd_intf_net [get_bd_intf_pins S0${i}_AXIS ] [get_bd_intf_pins axis_subset_converter_${i}/S_AXIS ]  
      #create a register
      create_bd_cell -type ip -vlnv xilinx.com:ip:c_shift_ram:12.0 register_${i}
      set_property -dict [list \
         CONFIG.Depth {1} \
         CONFIG.CE {true} \
      ] [get_bd_cells register_${i}]
     
      connect_bd_net [get_bd_pins aclk    ] [get_bd_pins register_${i}/CLK]
      #create a FF and use it to drive tdest
      connect_bd_net [get_bd_pins register_${i}/Q  ] [get_bd_pins axis_subset_converter_${i}/s_axis_tdest]
      
   }

   for {set i 0} {$i < $masters} {incr i} {
      connect_bd_intf_net [get_bd_intf_pins axis_switch_internal_0/M0${i}_AXIS] [get_bd_intf_pins M0${i}_AXIS ]
   }
   #connect switch_internal and tdest_creation 
   connect_bd_net [get_bd_pins aclk        ] [get_bd_pins axis_switch_internal_0/aclk   ] [get_bd_pins tdest_creation_0/ap_clk]
   connect_bd_net [get_bd_pins aresetn     ] [get_bd_pins axis_switch_internal_0/aresetn] [get_bd_pins tdest_creation_0/ap_rst_n]
   connect_bd_net [get_bd_pins s_decode_err] [get_bd_pins axis_switch_internal_0/s_decode_err]
   connect_bd_intf_net -boundary_type upper [get_bd_intf_pins scenario_V] [get_bd_intf_pins tdest_creation_0/scenario_V]
   #connect_bd_intf_net -boundary_type upper [get_bd_intf_pins S_AXI_CTRL] [get_bd_intf_pins tdest_creation_0/s_axi_control]
   #connect tdest_creation to register
   connect_bd_net [get_bd_pins tdest_creation_0/MAIN_SWITCH_S_DMA0_RX    ] [get_bd_pins register_0/D]
   connect_bd_net [get_bd_pins tdest_creation_0/MAIN_SWITCH_S_DMA1_RX    ] [get_bd_pins register_1/D]
   connect_bd_net [get_bd_pins tdest_creation_0/MAIN_SWITCH_S_DMA2_RX    ] [get_bd_pins register_2/D]
   connect_bd_net [get_bd_pins tdest_creation_0/MAIN_SWITCH_S_ARITH_RES  ] [get_bd_pins register_3/D]
   connect_bd_net [get_bd_pins tdest_creation_0/MAIN_SWITCH_S_EXT_KRNL   ] [get_bd_pins register_4/D]

   connect_bd_net [get_bd_pins tdest_creation_0/MAIN_SWITCH_S_DMA0_RX_ap_vld   ] [get_bd_pins register_0/CE]
   connect_bd_net [get_bd_pins tdest_creation_0/MAIN_SWITCH_S_DMA1_RX_ap_vld   ] [get_bd_pins register_1/CE]
   connect_bd_net [get_bd_pins tdest_creation_0/MAIN_SWITCH_S_DMA2_RX_ap_vld   ] [get_bd_pins register_2/CE]
   connect_bd_net [get_bd_pins tdest_creation_0/MAIN_SWITCH_S_ARITH_RES_ap_vld ] [get_bd_pins register_3/CE]
   connect_bd_net [get_bd_pins tdest_creation_0/MAIN_SWITCH_S_EXT_KRNL_ap_vld  ] [get_bd_pins register_4/CE]
   # Restore current instance
   current_bd_instance $oldCurInst
}

# Procedure to create entire design; Provide argument to make
# procedure reusable. If parentCell is "", will use root.
proc create_root_design { parentCell } {

  variable script_folder
  variable design_name

  if { $parentCell eq "" } {
     set parentCell [get_bd_cells /]
  }

  # Get object for parentCell
  set parentObj [get_bd_cells $parentCell]
  if { $parentObj == "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2090 -severity "ERROR" "Unable to find parent cell <$parentCell>!"}
     return
  }

  # Make sure parentObj is hier blk
  set parentType [get_property TYPE $parentObj]
  if { $parentType ne "hier" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2091 -severity "ERROR" "Parent <$parentObj> has TYPE = <$parentType>. Expected to be <hier>."}
     return
  }

  # Save current instance; Restore later
  set oldCurInst [current_bd_instance .]

  # Set parent object as current
  current_bd_instance $parentObj


  # Create interface ports
  set bscan_0 [ create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:bscan_rtl:1.0 bscan_0 ]

  set m_axi_0 [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 m_axi_0 ]
  set_property -dict [ list \
   CONFIG.ADDR_WIDTH {64} \
   CONFIG.DATA_WIDTH {512} \
   CONFIG.FREQ_HZ {250000000} \
   CONFIG.HAS_BRESP {0} \
   CONFIG.HAS_BURST {0} \
   CONFIG.HAS_CACHE {0} \
   CONFIG.HAS_LOCK {0} \
   CONFIG.HAS_PROT {0} \
   CONFIG.HAS_QOS {0} \
   CONFIG.HAS_REGION {0} \
   CONFIG.HAS_WSTRB {1} \
   CONFIG.NUM_READ_OUTSTANDING {1} \
   CONFIG.NUM_WRITE_OUTSTANDING {1} \
   CONFIG.PROTOCOL {AXI4} \
   CONFIG.READ_WRITE_MODE {READ_WRITE} \
   ] $m_axi_0

  set m_axi_1 [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 m_axi_1 ]
  set_property -dict [ list \
   CONFIG.ADDR_WIDTH {64} \
   CONFIG.DATA_WIDTH {512} \
   CONFIG.FREQ_HZ {250000000} \
   CONFIG.HAS_BRESP {0} \
   CONFIG.HAS_BURST {0} \
   CONFIG.HAS_CACHE {0} \
   CONFIG.HAS_LOCK {0} \
   CONFIG.HAS_PROT {0} \
   CONFIG.HAS_QOS {0} \
   CONFIG.HAS_REGION {0} \
   CONFIG.HAS_WSTRB {1} \
   CONFIG.NUM_READ_OUTSTANDING {1} \
   CONFIG.NUM_WRITE_OUTSTANDING {1} \
   CONFIG.PROTOCOL {AXI4} \
   CONFIG.READ_WRITE_MODE {READ_WRITE} \
   ] $m_axi_1

  set m_axi_2 [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:aximm_rtl:1.0 m_axi_2 ]
  set_property -dict [ list \
   CONFIG.ADDR_WIDTH {64} \
   CONFIG.DATA_WIDTH {512} \
   CONFIG.FREQ_HZ {250000000} \
   CONFIG.HAS_BRESP {0} \
   CONFIG.HAS_BURST {0} \
   CONFIG.HAS_CACHE {0} \
   CONFIG.HAS_LOCK {0} \
   CONFIG.HAS_PROT {0} \
   CONFIG.HAS_QOS {0} \
   CONFIG.HAS_REGION {0} \
   CONFIG.HAS_WSTRB {1} \
   CONFIG.NUM_READ_OUTSTANDING {1} \
   CONFIG.NUM_WRITE_OUTSTANDING {1} \
   CONFIG.PROTOCOL {AXI4} \
   CONFIG.READ_WRITE_MODE {READ_WRITE} \
   ] $m_axi_2

  set s_axis_udp_rx_data [ create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 s_axis_udp_rx_data ]
  set_property -dict [ list \
   CONFIG.FREQ_HZ {250000000} \
   CONFIG.HAS_TKEEP {1} \
   CONFIG.HAS_TLAST {1} \
   CONFIG.HAS_TREADY {1} \
   CONFIG.HAS_TSTRB {0} \
   CONFIG.LAYERED_METADATA {undef} \
   CONFIG.TDATA_NUM_BYTES {64} \
   CONFIG.TDEST_WIDTH {16} \
   CONFIG.TID_WIDTH {0} \
   CONFIG.TUSER_WIDTH {0} \
   ] $s_axis_udp_rx_data

  set m_axis_udp_tx_data [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 m_axis_udp_tx_data ]
  set_property -dict [ list \
   CONFIG.FREQ_HZ {250000000} \
   ] $m_axis_udp_tx_data

  set s_axis_krnl [ create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 s_axis_krnl ]
  set_property -dict [ list \
   CONFIG.FREQ_HZ {250000000} \
   CONFIG.HAS_TKEEP {1} \
   CONFIG.HAS_TLAST {1} \
   CONFIG.HAS_TREADY {1} \
   CONFIG.HAS_TSTRB {1} \
   CONFIG.LAYERED_METADATA {undef} \
   CONFIG.TDATA_NUM_BYTES {64} \
   CONFIG.TDEST_WIDTH {0} \
   CONFIG.TID_WIDTH {0} \
   CONFIG.TUSER_WIDTH {0} \
   ] $s_axis_krnl

  set m_axis_krnl [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 m_axis_krnl ]
  set_property -dict [ list \
   CONFIG.FREQ_HZ {250000000} \
   CONFIG.HAS_TKEEP {1} \
   CONFIG.HAS_TLAST {1} \
   CONFIG.HAS_TREADY {1} \
   CONFIG.HAS_TSTRB {0} \
   CONFIG.LAYERED_METADATA {undef} \
   CONFIG.TDATA_NUM_BYTES {64} \
   CONFIG.TDEST_WIDTH {0} \
   CONFIG.TID_WIDTH {0} \
   CONFIG.TUSER_WIDTH {0} \
   ] $m_axis_krnl

  set m_axis_arith_op0 [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 m_axis_arith_op0 ]
  set_property -dict [ list \
   CONFIG.FREQ_HZ {250000000} \
   CONFIG.HAS_TKEEP {1} \
   CONFIG.HAS_TLAST {1} \
   CONFIG.HAS_TREADY {1} \
   CONFIG.HAS_TSTRB {0} \
   CONFIG.LAYERED_METADATA {undef} \
   CONFIG.TDATA_NUM_BYTES {64} \
   CONFIG.TDEST_WIDTH {0} \
   CONFIG.TID_WIDTH {0} \
   CONFIG.TUSER_WIDTH {0} \
   ] $m_axis_arith_op0

  set m_axis_arith_op1 [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 m_axis_arith_op1 ]
  set_property -dict [ list \
   CONFIG.FREQ_HZ {250000000} \
   CONFIG.HAS_TKEEP {1} \
   CONFIG.HAS_TLAST {1} \
   CONFIG.HAS_TREADY {1} \
   CONFIG.HAS_TSTRB {0} \
   CONFIG.LAYERED_METADATA {undef} \
   CONFIG.TDATA_NUM_BYTES {64} \
   CONFIG.TDEST_WIDTH {0} \
   CONFIG.TID_WIDTH {0} \
   CONFIG.TUSER_WIDTH {0} \
   ] $m_axis_arith_op1

  set s_axis_arith_res [ create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 s_axis_arith_res ]
  set_property -dict [ list \
   CONFIG.FREQ_HZ {250000000} \
   CONFIG.HAS_TKEEP {1} \
   CONFIG.HAS_TLAST {1} \
   CONFIG.HAS_TREADY {1} \
   CONFIG.HAS_TSTRB {0} \
   CONFIG.LAYERED_METADATA {undef} \
   CONFIG.TDATA_NUM_BYTES {64} \
   CONFIG.TDEST_WIDTH {0} \
   CONFIG.TID_WIDTH {0} \
   CONFIG.TUSER_WIDTH {0} \
   ] $s_axis_arith_res

  set s_axi_control [ create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:aximm_rtl:1.0 s_axi_control ]
  set_property -dict [ list \
   CONFIG.ADDR_WIDTH {13} \
   CONFIG.ARUSER_WIDTH {0} \
   CONFIG.AWUSER_WIDTH {0} \
   CONFIG.BUSER_WIDTH {0} \
   CONFIG.DATA_WIDTH {32} \
   CONFIG.FREQ_HZ {250000000} \
   CONFIG.HAS_BRESP {1} \
   CONFIG.HAS_BURST {0} \
   CONFIG.HAS_CACHE {0} \
   CONFIG.HAS_LOCK {0} \
   CONFIG.HAS_PROT {1} \
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
   ] $s_axi_control

   #cmds from hls components
   set hls_control [ create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 hls_control ]
  set_property -dict [ list \
   CONFIG.FREQ_HZ {250000000} \
   CONFIG.TDATA_NUM_BYTES {64} \
   ] $hls_control

  set hls_control_result [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 hls_control_result ]
  set_property -dict [ list \
   CONFIG.FREQ_HZ {250000000} \
   CONFIG.TDATA_NUM_BYTES {4} \
   ] $hls_control_result

  # TCP interfaces
  set m_axis_tcp_listen_port [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 m_axis_tcp_listen_port ]
  set_property -dict [ list \
   CONFIG.FREQ_HZ {250000000} \
   ] $m_axis_tcp_listen_port

  set m_axis_tcp_open_connection [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 m_axis_tcp_open_connection ]
  set_property -dict [ list \
   CONFIG.FREQ_HZ {250000000} \
   ] $m_axis_tcp_open_connection

  set m_axis_tcp_read_pkg [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 m_axis_tcp_read_pkg ]
  set_property -dict [ list \
   CONFIG.FREQ_HZ {250000000} \
   ] $m_axis_tcp_read_pkg

  set m_axis_tcp_tx_data [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 m_axis_tcp_tx_data ]
  set_property -dict [ list \
   CONFIG.FREQ_HZ {250000000} \
   ] $m_axis_tcp_tx_data

  set m_axis_tcp_tx_meta [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 m_axis_tcp_tx_meta ]
  set_property -dict [ list \
   CONFIG.FREQ_HZ {250000000} \
   ] $m_axis_tcp_tx_meta

  set s_axis_tcp_notification [ create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 s_axis_tcp_notification ]
  set_property -dict [ list \
   CONFIG.FREQ_HZ {250000000} \
   CONFIG.HAS_TKEEP {1} \
   CONFIG.HAS_TLAST {1} \
   CONFIG.HAS_TREADY {1} \
   CONFIG.HAS_TSTRB {1} \
   CONFIG.LAYERED_METADATA {undef} \
   CONFIG.TDATA_NUM_BYTES {16} \
   CONFIG.TDEST_WIDTH {0} \
   CONFIG.TID_WIDTH {0} \
   CONFIG.TUSER_WIDTH {0} \
   ] $s_axis_tcp_notification

  set s_axis_tcp_open_status [ create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 s_axis_tcp_open_status ]
  set_property -dict [ list \
   CONFIG.FREQ_HZ {250000000} \
   CONFIG.HAS_TKEEP {1} \
   CONFIG.HAS_TLAST {1} \
   CONFIG.HAS_TREADY {1} \
   CONFIG.HAS_TSTRB {1} \
   CONFIG.LAYERED_METADATA {undef} \
   CONFIG.TDATA_NUM_BYTES {16} \
   CONFIG.TDEST_WIDTH {0} \
   CONFIG.TID_WIDTH {0} \
   CONFIG.TUSER_WIDTH {0} \
   ] $s_axis_tcp_open_status

  set s_axis_tcp_port_status [ create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 s_axis_tcp_port_status ]
  set_property -dict [ list \
   CONFIG.FREQ_HZ {250000000} \
   CONFIG.HAS_TKEEP {1} \
   CONFIG.HAS_TLAST {1} \
   CONFIG.HAS_TREADY {1} \
   CONFIG.HAS_TSTRB {1} \
   CONFIG.LAYERED_METADATA {undef} \
   CONFIG.TDATA_NUM_BYTES {1} \
   CONFIG.TDEST_WIDTH {0} \
   CONFIG.TID_WIDTH {0} \
   CONFIG.TUSER_WIDTH {0} \
   ] $s_axis_tcp_port_status

  set s_axis_tcp_rx_data [ create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 s_axis_tcp_rx_data ]
  set_property -dict [ list \
   CONFIG.FREQ_HZ {250000000} \
   CONFIG.HAS_TKEEP {1} \
   CONFIG.HAS_TLAST {1} \
   CONFIG.HAS_TREADY {1} \
   CONFIG.HAS_TSTRB {1} \
   CONFIG.LAYERED_METADATA {undef} \
   CONFIG.TDATA_NUM_BYTES {64} \
   CONFIG.TDEST_WIDTH {0} \
   CONFIG.TID_WIDTH {0} \
   CONFIG.TUSER_WIDTH {0} \
   ] $s_axis_tcp_rx_data

  set s_axis_tcp_rx_meta [ create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 s_axis_tcp_rx_meta ]
  set_property -dict [ list \
   CONFIG.FREQ_HZ {250000000} \
   CONFIG.HAS_TKEEP {1} \
   CONFIG.HAS_TLAST {1} \
   CONFIG.HAS_TREADY {1} \
   CONFIG.HAS_TSTRB {1} \
   CONFIG.LAYERED_METADATA {undef} \
   CONFIG.TDATA_NUM_BYTES {2} \
   CONFIG.TDEST_WIDTH {0} \
   CONFIG.TID_WIDTH {0} \
   CONFIG.TUSER_WIDTH {0} \
   ] $s_axis_tcp_rx_meta

  set s_axis_tcp_tx_status [ create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 s_axis_tcp_tx_status ]
  set_property -dict [ list \
   CONFIG.FREQ_HZ {250000000} \
   CONFIG.HAS_TKEEP {1} \
   CONFIG.HAS_TLAST {1} \
   CONFIG.HAS_TREADY {1} \
   CONFIG.HAS_TSTRB {1} \
   CONFIG.LAYERED_METADATA {undef} \
   CONFIG.TDATA_NUM_BYTES {8} \
   CONFIG.TDEST_WIDTH {0} \
   CONFIG.TID_WIDTH {0} \
   CONFIG.TUSER_WIDTH {0} \
   ] $s_axis_tcp_tx_status

  # Create ports
  set ap_clk [ create_bd_port -dir I -type clk -freq_hz 250000000 ap_clk ]
  set_property -dict [ list \
   CONFIG.ASSOCIATED_BUSIF {m_axi_0:m_axi_1:s_axi_control:s_axis_tcp_notification:m_axis_tcp_read_pkg:s_axis_tcp_rx_meta:m_axis_tcp_tx_meta:m_axis_tcp_tx_data:s_axis_tcp_open_status:s_axis_tcp_tx_status:m_axis_tcp_open_connection:m_axis_tcp_listen_port:s_axis_tcp_port_status:s_axis_tcp_rx_data:hls_control:hls_control_result} \
 ] $ap_clk
  set ap_rst_n [ create_bd_port -dir I -type rst ap_rst_n ]

  # Create instance: axis_switch_0, and set properties
  create_hier_cell_axis_switch_0 $parentObj "axis_switch_0"
  
   # Create instance: arith_switch_0, and set properties
  set arith_switch_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_switch:1.1 arith_switch_0 ]
  set_property -dict [ list \
   CONFIG.DECODER_REG {1} \
   CONFIG.HAS_TKEEP {1} \
   CONFIG.HAS_TLAST {1} \
   CONFIG.NUM_MI {5} \
   CONFIG.NUM_SI {4} \
   CONFIG.ROUTING_MODE {1} \
   CONFIG.TDATA_NUM_BYTES {64} \
 ] $arith_switch_0

  set control_xbar [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_interconnect:2.1 control_xbar ]
  set_property -dict [ list \
   CONFIG.NUM_MI {6} \
 ] $control_xbar

  source ./tcl/control_bd.tcl
  source ./tcl/dma_bd.tcl
  source ./tcl/rx_bd.tcl
  source ./tcl/tx_bd.tcl

  # Create instance: reduce_arith_0, and set properties
  set reduce_arith_0 [ create_bd_cell -type ip -vlnv xilinx.com:hls:reduce_arith:1.0 reduce_arith_0 ]

  save_bd_design

  # Create control interface connections
  connect_bd_intf_net -intf_net host_control [get_bd_intf_ports s_axi_control] [get_bd_intf_pins control/host_control]

  connect_bd_intf_net -intf_net encore_control           	 [get_bd_intf_pins control_xbar/S00_AXI] [get_bd_intf_pins control/encore_control]
  connect_bd_intf_net -intf_net arith_bypass_control         [get_bd_intf_pins control_xbar/M00_AXI] [get_bd_intf_pins arith_switch_0/S_AXI_CTRL]
  connect_bd_intf_net -intf_net arith_control            	 [get_bd_intf_pins control_xbar/M01_AXI] [get_bd_intf_pins reduce_arith_0/s_axi_control]
  connect_bd_intf_net -intf_net udp_packetizer_control       [get_bd_intf_pins control_xbar/M02_AXI] [get_bd_intf_pins udp_tx_subsystem/s_axi_control]
  connect_bd_intf_net -intf_net udp_depacketizer_control     [get_bd_intf_pins control_xbar/M03_AXI] [get_bd_intf_pins udp_rx_subsystem/s_axi_control]
  connect_bd_intf_net -intf_net tcp_packetizer_control       [get_bd_intf_pins control_xbar/M04_AXI] [get_bd_intf_pins tcp_tx_subsystem/s_axi_control]
  connect_bd_intf_net -intf_net tcp_depacketizer_control     [get_bd_intf_pins control_xbar/M05_AXI] [get_bd_intf_pins tcp_rx_subsystem/s_axi_control]

  connect_bd_intf_net -intf_net bscan_0 [get_bd_intf_ports bscan_0] [get_bd_intf_pins control/bscan_0]

  # Arithmetic-specific connections
  connect_bd_intf_net [get_bd_intf_pins axis_switch_0/M03_AXIS] [get_bd_intf_pins arith_switch_0/S00_AXIS]
  connect_bd_intf_net [get_bd_intf_pins axis_switch_0/M04_AXIS] [get_bd_intf_pins arith_switch_0/S01_AXIS]
  connect_bd_intf_net [get_bd_intf_pins axis_switch_0/S03_AXIS] [get_bd_intf_pins arith_switch_0/M04_AXIS]
  connect_bd_intf_net [get_bd_intf_pins arith_switch_0/M00_AXIS] [get_bd_intf_pins reduce_arith_0/in1]
  connect_bd_intf_net [get_bd_intf_pins arith_switch_0/M01_AXIS] [get_bd_intf_pins reduce_arith_0/in2]
  connect_bd_intf_net [get_bd_intf_pins arith_switch_0/S02_AXIS] [get_bd_intf_pins reduce_arith_0/out_r]
  connect_bd_intf_net [get_bd_intf_pins arith_switch_0/S03_AXIS] [get_bd_intf_pins s_axis_arith_res]
  connect_bd_intf_net [get_bd_intf_pins arith_switch_0/M02_AXIS] [get_bd_intf_pins m_axis_arith_op0]
  connect_bd_intf_net [get_bd_intf_pins arith_switch_0/M03_AXIS] [get_bd_intf_pins m_axis_arith_op1]

  # Streaming kernel interface connections
  connect_bd_intf_net [get_bd_intf_ports s_axis_krnl] [get_bd_intf_pins axis_switch_0/S04_AXIS]
  connect_bd_intf_net [get_bd_intf_ports m_axis_krnl] [get_bd_intf_pins axis_switch_0/M05_AXIS]

  connect_bd_intf_net [get_bd_intf_pins axis_switch_0/M01_AXIS] -boundary_type upper [get_bd_intf_pins tcp_tx_subsystem/s_axis_tcp_tx_data]


  # DMA connections
  for {set i 0} {$i < 3} {incr i} {
    connect_bd_intf_net -intf_net dma${i}_rx_cmd [get_bd_intf_pins control/dma${i}_mm2s_cmd] [get_bd_intf_pins dma_${i}/dma_mm2s_cmd]
    connect_bd_intf_net -intf_net dma${i}_rx_sts [get_bd_intf_pins control/dma${i}_mm2s_sts] [get_bd_intf_pins dma_${i}/dma_mm2s_sts]
    connect_bd_intf_net -intf_net dma${i}_tx_cmd [get_bd_intf_pins control/dma${i}_s2mm_cmd] [get_bd_intf_pins dma_${i}/dma_s2mm_cmd]
    connect_bd_intf_net -intf_net dma${i}_tx_sts [get_bd_intf_pins control/dma${i}_s2mm_sts] [get_bd_intf_pins dma_${i}/dma_s2mm_sts]

    connect_bd_intf_net -intf_net dma_${i}_m00_axi [get_bd_intf_ports m_axi_${i}] [get_bd_intf_pins dma_${i}/dma_aximm]
  }

  connect_bd_intf_net -intf_net dma0_rx [get_bd_intf_pins dma_0/dma_mm2s] [get_bd_intf_pins axis_switch_0/S00_AXIS]
  connect_bd_intf_net -intf_net dma0_tx [get_bd_intf_pins dma_0/dma_s2mm] [get_bd_intf_pins udp_rx_subsystem/m_axis_data]

  connect_bd_intf_net -intf_net dma1_rx [get_bd_intf_pins dma_1/dma_mm2s] [get_bd_intf_pins axis_switch_0/S01_AXIS]
  connect_bd_intf_net -intf_net dma1_tx [get_bd_intf_pins dma_1/dma_s2mm] [get_bd_intf_pins axis_switch_0/M02_AXIS]

  connect_bd_intf_net -intf_net dma2_rx [get_bd_intf_pins dma_2/dma_mm2s] [get_bd_intf_pins axis_switch_0/S02_AXIS]
  connect_bd_intf_net -intf_net dma2_tx [get_bd_intf_pins dma_2/dma_s2mm] [get_bd_intf_pins tcp_rx_subsystem/m_axis_tcp_rx_data]
  
  # Tx/Rx connections
  connect_bd_intf_net -intf_net vnx_depacketizer_in [get_bd_intf_ports s_axis_udp_rx_data] [get_bd_intf_pins udp_rx_subsystem/s_axis_data]
  connect_bd_intf_net -intf_net vnx_depacketizer_sts [get_bd_intf_pins control/udp_depacketizer_sts] [get_bd_intf_pins udp_rx_subsystem/m_axis_status]
  connect_bd_intf_net -intf_net vnx_packetizer_cmd [get_bd_intf_pins control/udp_packetizer_cmd] [get_bd_intf_pins udp_tx_subsystem/s_axis_command]
  connect_bd_intf_net -intf_net vnx_packetizer_in [get_bd_intf_pins axis_switch_0/M00_AXIS] [get_bd_intf_pins udp_tx_subsystem/s_axis_data]
  connect_bd_intf_net -intf_net vnx_packetizer_out [get_bd_intf_ports m_axis_udp_tx_data] [get_bd_intf_pins udp_tx_subsystem/m_axis_data]
  connect_bd_intf_net -intf_net vnx_packetizer_sts [get_bd_intf_pins control/udp_packetizer_sts] [get_bd_intf_pins udp_tx_subsystem/m_axis_sts]

  connect_bd_intf_net [get_bd_intf_pins tcp_rx_subsystem/m_axis_pktsts] [get_bd_intf_pins control/tcp_depacketizer_sts]
  connect_bd_intf_net [get_bd_intf_pins tcp_tx_subsystem/s_axis_pktcmd] [get_bd_intf_pins control/tcp_packetizer_cmd]
  connect_bd_intf_net [get_bd_intf_pins tcp_tx_subsystem/m_axis_tcp_packetizer_sts] [get_bd_intf_pins control/tcp_packetizer_sts]

  connect_bd_intf_net [get_bd_intf_pins tcp_rx_subsystem/m_axis_openport_sts] [get_bd_intf_pins control/tcp_openport_sts]
  connect_bd_intf_net [get_bd_intf_pins tcp_rx_subsystem/s_axis_openport_cmd] [get_bd_intf_pins control/tcp_openport_cmd]

  connect_bd_intf_net [get_bd_intf_pins tcp_tx_subsystem/s_axis_opencon_cmd] [get_bd_intf_pins control/tcp_opencon_cmd]
  connect_bd_intf_net [get_bd_intf_pins tcp_tx_subsystem/m_axis_opencon_sts] [get_bd_intf_pins control/tcp_opencon_sts]

  connect_bd_intf_net [get_bd_intf_ports s_axis_tcp_rx_data] [get_bd_intf_pins tcp_rx_subsystem/s_axis_tcp_rx_data]
  connect_bd_intf_net [get_bd_intf_ports m_axis_tcp_read_pkg] [get_bd_intf_pins tcp_rx_subsystem/m_axis_tcp_read_pkg]
  connect_bd_intf_net [get_bd_intf_ports s_axis_tcp_rx_meta] [get_bd_intf_pins tcp_rx_subsystem/s_axis_tcp_rx_meta]
  connect_bd_intf_net [get_bd_intf_ports s_axis_tcp_notification] [get_bd_intf_pins tcp_rx_subsystem/s_axis_tcp_notification]
  connect_bd_intf_net [get_bd_intf_ports m_axis_tcp_listen_port] [get_bd_intf_pins tcp_rx_subsystem/m_axis_tcp_listen_port]
  connect_bd_intf_net [get_bd_intf_ports s_axis_tcp_port_status] [get_bd_intf_pins tcp_rx_subsystem/s_axis_tcp_port_status]

  connect_bd_intf_net [get_bd_intf_ports m_axis_tcp_tx_meta] [get_bd_intf_pins tcp_tx_subsystem/m_axis_tcp_tx_meta]
  connect_bd_intf_net [get_bd_intf_ports m_axis_tcp_tx_data] [get_bd_intf_pins tcp_tx_subsystem/m_axis_tcp_tx_data]
  connect_bd_intf_net [get_bd_intf_ports m_axis_tcp_open_connection] [get_bd_intf_pins tcp_tx_subsystem/m_axis_tcp_open_connection]
  connect_bd_intf_net [get_bd_intf_ports s_axis_tcp_open_status] [get_bd_intf_pins tcp_tx_subsystem/s_axis_tcp_open_status]
  connect_bd_intf_net [get_bd_intf_ports s_axis_tcp_tx_status] [get_bd_intf_pins tcp_tx_subsystem/s_axis_tcp_tx_status]
  # Connect hls_control and hls_control_result
  connect_bd_intf_net [get_bd_intf_pins hls_control       ] [get_bd_intf_pins control/hls_control       ]
  connect_bd_intf_net [get_bd_intf_pins hls_control_result] [get_bd_intf_pins control/hls_control_result]
  # Create reset and clock connections
  connect_bd_net -net ap_clk [get_bd_ports ap_clk] [get_bd_pins axis_switch_0/aclk] \
                                                   [get_bd_pins arith_switch_0/aclk] \
                                                   [get_bd_pins arith_switch_0/s_axi_ctrl_aclk] \
                                                   [get_bd_pins control/ap_clk] \
                                                   [get_bd_pins dma_0/ap_clk] \
                                                   [get_bd_pins dma_1/ap_clk] \
                                                   [get_bd_pins dma_2/ap_clk] \
                                                   [get_bd_pins reduce_arith_0/ap_clk] \
                                                   [get_bd_pins udp_rx_subsystem/ap_clk] \
                                                   [get_bd_pins udp_tx_subsystem/ap_clk] \
                                                   [get_bd_pins tcp_rx_subsystem/ap_clk] \
                                                   [get_bd_pins tcp_tx_subsystem/ap_clk] \
                                                   [get_bd_pins control_xbar/ACLK] \
                                                   [get_bd_pins control_xbar/S00_ACLK] \
                                                   [get_bd_pins control_xbar/M00_ACLK] \
                                                   [get_bd_pins control_xbar/M01_ACLK] \
                                                   [get_bd_pins control_xbar/M02_ACLK] \
                                                   [get_bd_pins control_xbar/M03_ACLK] \
                                                   [get_bd_pins control_xbar/M04_ACLK] \
                                                   [get_bd_pins control_xbar/M05_ACLK] 
  connect_bd_net -net ap_rst_n [get_bd_ports ap_rst_n] [get_bd_pins control/ap_rst_n]
  connect_bd_net -net ap_rst_n_1 [get_bd_pins control/encore_aresetn] [get_bd_pins axis_switch_0/aresetn] \
                                                                      [get_bd_pins arith_switch_0/aresetn] \
                                                                      [get_bd_pins arith_switch_0/s_axi_ctrl_aresetn] \
                                                                      [get_bd_pins dma_0/ap_rst_n] \
                                                                      [get_bd_pins dma_1/ap_rst_n] \
                                                                      [get_bd_pins dma_2/ap_rst_n] \
                                                                      [get_bd_pins reduce_arith_0/ap_rst_n] \
                                                                      [get_bd_pins udp_rx_subsystem/ap_rst_n] \
                                                                      [get_bd_pins udp_tx_subsystem/ap_rst_n] \
                                                                      [get_bd_pins tcp_rx_subsystem/ap_rst_n] \
                                                                      [get_bd_pins tcp_tx_subsystem/ap_rst_n] \
                                                                      [get_bd_pins control_xbar/ARESETN] \
                                                                      [get_bd_pins control_xbar/S00_ARESETN] \
                                                                      [get_bd_pins control_xbar/M00_ARESETN] \
                                                                      [get_bd_pins control_xbar/M01_ARESETN] \
                                                                      [get_bd_pins control_xbar/M02_ARESETN] \
                                                                      [get_bd_pins control_xbar/M03_ARESETN] \
                                                                      [get_bd_pins control_xbar/M04_ARESETN] \
                                                                      [get_bd_pins control_xbar/M05_ARESETN] 
  # connect axis_switch_0/scenario_V to MB/M11_AXIS
  connect_bd_intf_net [get_bd_intf_pins control/M11_AXIS          ] [get_bd_intf_pins axis_switch_0/scenario_V] 
  save_bd_design
  # Create address segments
  #1. exchange memory module
  #1.1. register in which user writes, to communicate with host control and exchange mem. !!It has to span accross host_ctrl AND exchange mem regions!!
  assign_bd_address -offset 0x00000000 -range 0x00000800 -target_address_space [get_bd_addr_spaces s_axi_control] [get_bd_addr_segs control/microblaze_0_exchange_memory/hostctrl/s_axi_control/Reg] -force 
  #1.2  make hostctrl region accessible to MB
  assign_bd_address -offset 0x00000000 -range 0x00000800 -target_address_space [get_bd_addr_spaces control/microblaze_0/Data] [get_bd_addr_segs control/microblaze_0_exchange_memory/hostctrl/s_axi_control/Reg] -force
  #1.2  actual exchange memory 
  assign_bd_address -offset 0x00001000 -range 0x00001000 -target_address_space [get_bd_addr_spaces s_axi_control] [get_bd_addr_segs control/microblaze_0_exchange_memory/axi_bram_ctrl_0/S_AXI/Mem0] -force
  #1.2  make exchange mem region accessible to MB
  assign_bd_address -offset 0x00001000 -range 0x00001000 -target_address_space [get_bd_addr_spaces control/microblaze_0/Data] [get_bd_addr_segs control/microblaze_0_exchange_memory/axi_bram_ctrl_0/S_AXI/Mem0] -force
  #MB RAM for memory and instructions
  assign_bd_address -offset 0x00010000 -range 0x00008000 -target_address_space [get_bd_addr_spaces control/microblaze_0/Data] [get_bd_addr_segs control/microblaze_0_local_memory/dlmb_bram_if_cntlr/SLMB/Mem] -force
  assign_bd_address -offset 0x00010000 -range 0x00008000 -target_address_space [get_bd_addr_spaces control/microblaze_0/Instruction] [get_bd_addr_segs control/microblaze_0_local_memory/ilmb_bram_if_cntlr/SLMB/Mem] -force
  # arith control logic
  assign_bd_address -offset 0x00020000 -range 0x00010000 -target_address_space [get_bd_addr_spaces control/microblaze_0/Data] [get_bd_addr_segs reduce_arith_0/s_axi_control/Reg] -force
  # udp depacketizer
  assign_bd_address -offset 0x00030000 -range 0x00010000 -target_address_space [get_bd_addr_spaces control/microblaze_0/Data] [get_bd_addr_segs udp_rx_subsystem/vnx_depacketizer_0/s_axi_control/Reg] -force
  # udp packetizer
  assign_bd_address -offset 0x00040000 -range 0x00010000 -target_address_space [get_bd_addr_spaces control/microblaze_0/Data] [get_bd_addr_segs udp_tx_subsystem/vnx_packetizer_0/s_axi_control/Reg] -force
  # tcp depacketizer
  assign_bd_address -offset 0x00050000 -range 0x00010000 -target_address_space [get_bd_addr_spaces control/microblaze_0/Data] [get_bd_addr_segs tcp_rx_subsystem/tcp_depacketizer_0/s_axi_control/Reg] -force
  # tcp packetizer
  assign_bd_address -offset 0x00060000 -range 0x00010000 -target_address_space [get_bd_addr_spaces control/microblaze_0/Data] [get_bd_addr_segs tcp_tx_subsystem/tcp_packetizer_0/s_axi_control/Reg] -force
  # exchange memory hw versioning register+reset
  assign_bd_address -offset 0x40000000 -range 0x00001000 -target_address_space [get_bd_addr_spaces control/microblaze_0/Data] [get_bd_addr_segs control/microblaze_0_exchange_memory/axi_gpio_0/S_AXI/Reg] -force
  # axis_switch in mpi_offload top view
  #assign_bd_address -offset 0x44A00000 -range 0x00010000 -target_address_space [get_bd_addr_spaces control/microblaze_0/Data] [get_bd_addr_segs axis_switch_0/tdest_creation_0/s_axi_control/Reg] -force
  # irq controller
  assign_bd_address -offset 0x44A10000 -range 0x00010000 -target_address_space [get_bd_addr_spaces control/microblaze_0/Data] [get_bd_addr_segs control/proc_irq_control/S_AXI/Reg] -force
  # timer 
  assign_bd_address -offset 0x44A20000 -range 0x00010000 -target_address_space [get_bd_addr_spaces control/microblaze_0/Data] [get_bd_addr_segs control/axi_timer/S_AXI/Reg] -force
  # dma_mover
  assign_bd_address -offset 0x44A30000 -range 0x00010000 -target_address_space [get_bd_addr_spaces control/microblaze_0/Data] [get_bd_addr_segs control/dma_harden/dma_mover_0/s_axi_control/Reg] -force 
  assign_bd_address -offset 0x00001000 -range 0x00001000 -target_address_space [get_bd_addr_spaces control/dma_harden/dma_mover_0/Data_m_axi_mem] [get_bd_addr_segs control/microblaze_0_exchange_memory/axi_bram_ctrl_bypass/S_AXI/Mem0] -force
  # axis_switch for arithm in mpi_offload top view
  assign_bd_address -offset 0x44B00000 -range 0x00010000 -target_address_space [get_bd_addr_spaces control/microblaze_0/Data] [get_bd_addr_segs arith_switch_0/S_AXI_CTRL/Reg] -force
  #dma_enqueu/dequeue
  #dma_enqueue
   #1. access to exchange memory
   assign_bd_address -offset 0x00001000 -range 0x00001000 -target_address_space [get_bd_addr_spaces control/dma_harden/dma_dequeue_0/Data_m_axi_mem] [get_bd_addr_segs control/microblaze_0_exchange_memory/axi_bram_ctrl_bypass/S_AXI/Mem0] -force
   #2. mcu access to ctrl via axilite
   assign_bd_address -offset 0x44C00000 -range 0x00010000 -target_address_space [get_bd_addr_spaces control/microblaze_0/Data] [get_bd_addr_segs control/dma_harden/dma_dequeue_0/s_axi_control/Reg] -force
  #dma_dequeue
   #1. access to exchange memory
   assign_bd_address -offset 0x00001000 -range 0x00001000 -target_address_space [get_bd_addr_spaces control/dma_harden/dma_enqueue_0/Data_m_axi_mem] [get_bd_addr_segs control/microblaze_0_exchange_memory/axi_bram_ctrl_bypass/S_AXI/Mem0] -force
   #2. mcu access to ctrl via axilite
   assign_bd_address -offset 0x44D00000 -range 0x00010000 -target_address_space [get_bd_addr_spaces control/microblaze_0/Data] [get_bd_addr_segs control/dma_harden/dma_enqueue_0/s_axi_control/Reg] -force
  # DMA ddr
  assign_bd_address -offset 0x00000000 -range 0x00010000000000000000 -target_address_space [get_bd_addr_spaces dma_0/axi_datamover_0/Data] [get_bd_addr_segs m_axi_0/Reg] -force
  assign_bd_address -offset 0x00000000 -range 0x00010000000000000000 -target_address_space [get_bd_addr_spaces dma_2/axi_datamover_0/Data] [get_bd_addr_segs m_axi_2/Reg] -force
  assign_bd_address -offset 0x00000000 -range 0x00010000000000000000 -target_address_space [get_bd_addr_spaces dma_1/axi_datamover_0/Data] [get_bd_addr_segs m_axi_1/Reg] -force
  # Restore current instance
  current_bd_instance $oldCurInst

  validate_bd_design
  save_bd_design
}
# End of create_root_design()


##################################################################
# MAIN FLOW
##################################################################

create_root_design ""

