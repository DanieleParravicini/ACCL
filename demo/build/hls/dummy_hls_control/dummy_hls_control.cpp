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

#include "ap_int.h"
#include "ap_utils.h"
#include "hls_stream.h"
#include "../../../../kernel/hls/hostctrl_in//hostctrl_in.h" 

using namespace hls;
using namespace std;

void dummy_hls_control(
    ap_uint<32> scenario,
    ap_uint<32> len,
    ap_uint<32> comm,
    ap_uint<32> root_src_dst,
    ap_uint<32> function,
    ap_uint<32> msg_tag,
    ap_uint<32> buf0_type,
    ap_uint<32> buf1_type,
    ap_uint<32> buf2_type,
    ap_uint<64> addra,
    ap_uint<64> addrb,
    ap_uint<64> addrc,
    ap_uint<64> delay,
    stream<ap_uint<512>> &cmd,
    stream<ap_uint<32>> &sts) {
#pragma HLS INTERFACE s_axilite port = scenario
#pragma HLS INTERFACE s_axilite port = len
#pragma HLS INTERFACE s_axilite port = comm
#pragma HLS INTERFACE s_axilite port = root_src_dst
#pragma HLS INTERFACE s_axilite port = function
#pragma HLS INTERFACE s_axilite port = msg_tag
#pragma HLS INTERFACE s_axilite port = buf0_type
#pragma HLS INTERFACE s_axilite port = buf1_type
#pragma HLS INTERFACE s_axilite port = buf2_type
#pragma HLS INTERFACE s_axilite port = addra
#pragma HLS INTERFACE s_axilite port = addrb
#pragma HLS INTERFACE s_axilite port = addrc
#pragma HLS INTERFACE s_axilite port = delay
#pragma HLS INTERFACE axis port = cmd
#pragma HLS INTERFACE axis port = sts
#pragma HLS INTERFACE s_axilite port = return

  ap_wait_n(delay);

  ap_uint<512> in_data;
  // Input stream needs to be optimized in the same way as hostctrl
  in_data.range(END_SCENARIO	    ,START_SCENARIO		  ) = scenario;
  in_data.range(END_LEN			      ,START_LEN			    ) = len;
  in_data.range(END_COMM		      ,START_COMM			    ) = comm;
  in_data.range(END_ROOT_SRC_DST  ,START_ROOT_SRC_DST	) = root_src_dst;
  in_data.range(END_FUNCTION	    ,START_FUNCTION		  ) = function;
  in_data.range(END_TAG			      ,START_TAG			    ) = msg_tag;
  in_data.range(END_BUFF_0_TYPE	  ,START_BUFF_0_TYPE	) = buf0_type;
  in_data.range(END_BUFF_1_TYPE	  ,START_BUFF_1_TYPE	) = buf1_type;
  in_data.range(END_BUFF_2_TYPE	  ,START_BUFF_2_TYPE	) = buf2_type;
  in_data.range(END_ADDR_A		    ,START_ADDR_A		    ) = addra;
  in_data.range(END_ADDR_B		    ,START_ADDR_B		    ) = addrb;
  in_data.range(END_ADDR_C		    ,START_ADDR_C		    ) = addrc;
  {
#pragma HLS protocol fixed
    cmd.write(in_data);
    ap_wait();
    sts.read();
  }
}