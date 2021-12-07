/*******************************************************************************
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
#
*******************************************************************************/

#include <iostream>

#include "ap_int.h"
#include "hls_stream.h"

using namespace hls;
using namespace std;

ap_uint<96> tri_benchmark_control(
    ap_uint<32> scenario0,
    ap_uint<32> len0,
    ap_uint<32> comm0,
    ap_uint<32> root_src_dst0,
    ap_uint<32> function0,
    ap_uint<32> msg_tag0,
    ap_uint<32> buf0_type0,
    ap_uint<32> buf1_type0,
    ap_uint<32> buf2_type0,
    ap_uint<64> addra_0,
    ap_uint<64> addrb_0,
    ap_uint<64> addrc_0,

    ap_uint<32> scenario_1,
    ap_uint<32> len_1,
    ap_uint<32> comm_1,
    ap_uint<32> root_src_dst_1,
    ap_uint<32> function_1,
    ap_uint<32> msg_tag_1,
    ap_uint<32> buf0_type_1,
    ap_uint<32> buf1_type_1,
    ap_uint<32> buf2_type_1,
    ap_uint<64> addra_1,
    ap_uint<64> addrb_1,
    ap_uint<64> addrc_1,

    ap_uint<32> scenario_2,
    ap_uint<32> len_2,
    ap_uint<32> comm_2,
    ap_uint<32> root_src_dst_2,
    ap_uint<32> function_2,
    ap_uint<32> msg_tag_2,
    ap_uint<32> buf0_type_2,
    ap_uint<32> buf1_type_2,
    ap_uint<32> buf2_type_2,
    ap_uint<64> addra_2,
    ap_uint<64> addrb_2,
    ap_uint<64> addrc_2,

    stream<ap_uint<512>> &cmd_0,
    stream<ap_uint<32>> &sts_0,
    stream<ap_uint<512>> &cmd_1,
    stream<ap_uint<32>> &sts_1,
    stream<ap_uint<512>> &cmd_2,
    stream<ap_uint<32>> &sts_2
    );

int main() {
  int nerrors = 0;

  stream<ap_uint<512>> cmd_0, cmd_1, cmd_2;
  stream<ap_uint<32>>  sts_0, sts_1, sts_2;

  for (int i = 0; i < 10; i++) {
    ap_uint<32> scenario_0      = 0 + 1 + i;
    ap_uint<32> len_0           = 0 + 2 + i;
    ap_uint<32> comm_0          = 0 + 3 + i;
    ap_uint<32> root_src_dst_0  = 0 + 4 + i;
    ap_uint<32> function_0      = 0 + 5 + i;
    ap_uint<32> msg_tag_0       = 0 + 6 + i;
    ap_uint<32> buf0_type_0     = 0 + 7 + i;
    ap_uint<32> buf1_type_0     = 0 + 8 + i;
    ap_uint<32> buf2_type_0     = 0 + 9 + i;
    ap_uint<64> addra_0         = 0 + 10 + i;
    ap_uint<64> addrb_0         = 0 + 11 + i;
    ap_uint<64> addrc_0         = 0 + 12 + i;

    sts_0.write(0 + 1 + i);

    ap_uint<32> scenario_1      = 1 + 1 + i;
    ap_uint<32> len_1           = 1 + 2 + i;
    ap_uint<32> comm_1          = 1 + 3 + i;
    ap_uint<32> root_src_dst_1  = 1 + 4 + i;
    ap_uint<32> function_1      = 1 + 5 + i;
    ap_uint<32> msg_tag_1       = 1 + 6 + i;
    ap_uint<32> buf0_type_1     = 1 + 7 + i;
    ap_uint<32> buf1_type_1     = 1 + 8 + i;
    ap_uint<32> buf2_type_1     = 1 + 9 + i;
    ap_uint<64> addra_1         = 1 + 10 + i;
    ap_uint<64> addrb_1         = 1 + 11 + i;
    ap_uint<64> addrc_1         = 1 + 12 + i;

    sts_1.write(1 + 1 + i);

    ap_uint<32> scenario_2      = 2 + 1 + i;
    ap_uint<32> len_2           = 2 + 2 + i;
    ap_uint<32> comm_2          = 2 + 3 + i;
    ap_uint<32> root_src_dst_2  = 2 + 4 + i;
    ap_uint<32> function_2      = 2 + 5 + i;
    ap_uint<32> msg_tag_2       = 2 + 6 + i;
    ap_uint<32> buf0_type_2     = 2 + 7 + i;
    ap_uint<32> buf1_type_2     = 2 + 8 + i;
    ap_uint<32> buf2_type_2     = 2 + 9 + i;
    ap_uint<64> addra_2         = 2 + 10 + i;
    ap_uint<64> addrb_2         = 2 + 11 + i;
    ap_uint<64> addrc_2         = 2 + 12 + i;
 
    sts_2.write(2 + 1 + i);

    tri_benchmark_control(
        scenario_0,
        len_0,
        comm_0,
        root_src_dst_0,
        function_0,
        msg_tag_0,
        buf0_type_0,
        buf1_type_0,
        buf2_type_0,
        addra_0,
        addrb_0,
        addrc_0,
        scenario_1,
        len_1,
        comm_1,
        root_src_dst_1,
        function_1,
        msg_tag_1,
        buf0_type_1,
        buf1_type_1,
        buf2_type_1,
        addra_1,
        addrb_1,
        addrc_1,
        scenario_2,
        len_2,
        comm_2,
        root_src_dst_2,
        function_2,
        msg_tag_2,
        buf0_type_2,
        buf1_type_2,
        buf2_type_2,
        addra_2,
        addrb_2,
        addrc_2,
        cmd_0, sts_0,
        cmd_1, sts_1,
        cmd_2, sts_2);
    ap_uint<512> cmd_tmp_0 = cmd_0.read();
    ap_uint<512> cmd_tmp_1 = cmd_1.read();
    ap_uint<512> cmd_tmp_2 = cmd_2.read();
    nerrors += (scenario_0      != cmd_tmp_0.range(31, 0));
    nerrors += (len_0           != cmd_tmp_0.range(63, 32));
    nerrors += (comm_0          != cmd_tmp_0.range(95, 64));
    nerrors += (root_src_dst_0  != cmd_tmp_0.range(127, 96));
    nerrors += (function_0      != cmd_tmp_0.range(159, 128));
    nerrors += (msg_tag_0       != cmd_tmp_0.range(191, 160));
    nerrors += (buf0_type_0     != cmd_tmp_0.range(223, 192));
    nerrors += (buf1_type_0     != cmd_tmp_0.range(255, 224));
    nerrors += (buf2_type_0     != cmd_tmp_0.range(287, 256));
    nerrors += (addra_0         != cmd_tmp_0.range(351, 288));
    nerrors += (addrb_0         != cmd_tmp_0.range(415, 352));
    nerrors += (addrc_0         != cmd_tmp_0.range(479, 416));
    nerrors += (scenario_1      != cmd_tmp_1.range(31, 0));
    nerrors += (len_1           != cmd_tmp_1.range(63, 32));
    nerrors += (comm_1          != cmd_tmp_1.range(95, 64));
    nerrors += (root_src_dst_1  != cmd_tmp_1.range(127, 96));
    nerrors += (function_1      != cmd_tmp_1.range(159, 128));
    nerrors += (msg_tag_1       != cmd_tmp_1.range(191, 160));
    nerrors += (buf0_type_1     != cmd_tmp_1.range(223, 192));
    nerrors += (buf1_type_1     != cmd_tmp_1.range(255, 224));
    nerrors += (buf2_type_1     != cmd_tmp_1.range(287, 256));
    nerrors += (addra_1         != cmd_tmp_1.range(351, 288));
    nerrors += (addrb_1         != cmd_tmp_1.range(415, 352));
    nerrors += (addrc_1         != cmd_tmp_1.range(479, 416));
    nerrors += (scenario_2      != cmd_tmp_2.range(31, 0));
    nerrors += (len_2           != cmd_tmp_2.range(63, 32));
    nerrors += (comm_2          != cmd_tmp_2.range(95, 64));
    nerrors += (root_src_dst_2  != cmd_tmp_2.range(127, 96));
    nerrors += (function_2      != cmd_tmp_2.range(159, 128));
    nerrors += (msg_tag_2       != cmd_tmp_2.range(191, 160));
    nerrors += (buf0_type_2     != cmd_tmp_2.range(223, 192));
    nerrors += (buf1_type_2     != cmd_tmp_2.range(255, 224));
    nerrors += (buf2_type_2     != cmd_tmp_2.range(287, 256));
    nerrors += (addra_2         != cmd_tmp_2.range(351, 288));
    nerrors += (addrb_2         != cmd_tmp_2.range(415, 352));
    nerrors += (addrc_2         != cmd_tmp_2.range(479, 416));
  }

  std::cout << std::endl;
  std::cout << std::endl;
  std::cout << "******************************" << std::endl;
  std::cout << "Found " << nerrors << " errors" << std::endl;
  std::cout << "******************************" << std::endl;
  std::cout << std::endl;
  std::cout << std::endl;
  return 0;
}
