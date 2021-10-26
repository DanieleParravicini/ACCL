
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