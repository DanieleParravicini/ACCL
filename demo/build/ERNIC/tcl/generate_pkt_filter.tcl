
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

set fpgapart [lindex $::argv 0]
set boardpart [lindex $::argv 1]

create_project project_pk_filter project_pk_filter -part $fpgapart
set_property board_part $boardpart [current_project]

add_files -norecurse ./hdl/src/RDMA_pkt_filter_one_port.sv
update_compile_order -fileset sources_1
set_property top RDMA_pkt_filter_one_port [current_fileset]

ipx::package_project -root_dir . -vendor danielep.org -library user -taxonomy /UserIP
ipx::update_checksums [ipx::current_core]
ipx::save_core [ipx::current_core]

update_compile_order -fileset sources_1
set_property core_revision 2 [ipx::current_core]
ipx::create_xgui_files [ipx::current_core]
ipx::update_checksums [ipx::current_core]
ipx::check_integrity [ipx::current_core]
ipx::save_core [ipx::current_core]
set_property  ip_repo_paths  . [current_project]
update_ip_catalog
