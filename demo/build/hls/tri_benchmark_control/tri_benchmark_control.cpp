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

ap_uint<96> tri_benchmark_control(
    ap_uint<32> scenario_0,
    ap_uint<32> len_0,
    ap_uint<32> comm_0,
    ap_uint<32> root_src_dst_0,
    ap_uint<32> function_0,
    ap_uint<32> msg_tag_0,
    ap_uint<32> buf0_type_0,
    ap_uint<32> buf1_type_0,
    ap_uint<32> buf2_type_0,
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
    ) {
#pragma HLS INTERFACE s_axilite port = scenario_0
#pragma HLS INTERFACE s_axilite port = len_0
#pragma HLS INTERFACE s_axilite port = comm_0
#pragma HLS INTERFACE s_axilite port = root_src_dst_0
#pragma HLS INTERFACE s_axilite port = function_0
#pragma HLS INTERFACE s_axilite port = msg_tag_0
#pragma HLS INTERFACE s_axilite port = buf0_type_0
#pragma HLS INTERFACE s_axilite port = buf1_type_0
#pragma HLS INTERFACE s_axilite port = buf2_type_0
#pragma HLS INTERFACE s_axilite port = addra_0
#pragma HLS INTERFACE s_axilite port = addrb_0
#pragma HLS INTERFACE s_axilite port = addrc_0
#pragma HLS INTERFACE s_axilite port = scenario_1
#pragma HLS INTERFACE s_axilite port = len_1
#pragma HLS INTERFACE s_axilite port = comm_1
#pragma HLS INTERFACE s_axilite port = root_src_dst_1
#pragma HLS INTERFACE s_axilite port = function_1
#pragma HLS INTERFACE s_axilite port = msg_tag_1
#pragma HLS INTERFACE s_axilite port = buf0_type_1
#pragma HLS INTERFACE s_axilite port = buf1_type_1
#pragma HLS INTERFACE s_axilite port = buf2_type_1
#pragma HLS INTERFACE s_axilite port = addra_1
#pragma HLS INTERFACE s_axilite port = addrb_1
#pragma HLS INTERFACE s_axilite port = addrc_1
#pragma HLS INTERFACE s_axilite port = scenario_2
#pragma HLS INTERFACE s_axilite port = len_2
#pragma HLS INTERFACE s_axilite port = comm_2
#pragma HLS INTERFACE s_axilite port = root_src_dst_2
#pragma HLS INTERFACE s_axilite port = function_2
#pragma HLS INTERFACE s_axilite port = msg_tag_2
#pragma HLS INTERFACE s_axilite port = buf0_type_2
#pragma HLS INTERFACE s_axilite port = buf1_type_2
#pragma HLS INTERFACE s_axilite port = buf2_type_2
#pragma HLS INTERFACE s_axilite port = addra_2
#pragma HLS INTERFACE s_axilite port = addrb_2
#pragma HLS INTERFACE s_axilite port = addrc_2
#pragma HLS INTERFACE axis port = cmd_0
#pragma HLS INTERFACE axis port = sts_0
#pragma HLS INTERFACE axis port = cmd_1
#pragma HLS INTERFACE axis port = sts_1
#pragma HLS INTERFACE axis port = cmd_2
#pragma HLS INTERFACE axis port = sts_2

#pragma HLS INTERFACE s_axilite port = return
  ap_uint<96> ret;
  ap_uint<32> count_0 = 0, count_1 = 0, count_2 = 0;
  ap_uint <1> flag_0 = 1, flag_1 = 1, flag_2 = 1;
  ap_uint<512> in_data_0, in_data_1, in_data_2;
  // Input stream needs to be optimized in the same way as hostctrl
  in_data_0.range(END_SCENARIO	    ,START_SCENARIO		  ) = scenario_0;
  in_data_0.range(END_LEN			      ,START_LEN			    ) = len_0;
  in_data_0.range(END_COMM		      ,START_COMM			    ) = comm_0;
  in_data_0.range(END_ROOT_SRC_DST  ,START_ROOT_SRC_DST	) = root_src_dst_0;
  in_data_0.range(END_FUNCTION	    ,START_FUNCTION		  ) = function_0;
  in_data_0.range(END_TAG			      ,START_TAG			    ) = msg_tag_0;
  in_data_0.range(END_BUFF_0_TYPE	  ,START_BUFF_0_TYPE	) = buf0_type_0;
  in_data_0.range(END_BUFF_1_TYPE	  ,START_BUFF_1_TYPE	) = buf1_type_0;
  in_data_0.range(END_BUFF_2_TYPE	  ,START_BUFF_2_TYPE	) = buf2_type_0;
  in_data_0.range(END_ADDR_A		    ,START_ADDR_A		    ) = addra_0;
  in_data_0.range(END_ADDR_B		    ,START_ADDR_B		    ) = addrb_0;
  in_data_0.range(END_ADDR_C		    ,START_ADDR_C		    ) = addrc_0;

  in_data_1.range(END_SCENARIO	    ,START_SCENARIO		  ) = scenario_1;
  in_data_1.range(END_LEN			      ,START_LEN			    ) = len_1;
  in_data_1.range(END_COMM		      ,START_COMM			    ) = comm_1;
  in_data_1.range(END_ROOT_SRC_DST  ,START_ROOT_SRC_DST	) = root_src_dst_1;
  in_data_1.range(END_FUNCTION	    ,START_FUNCTION		  ) = function_1;
  in_data_1.range(END_TAG			      ,START_TAG			    ) = msg_tag_1;
  in_data_1.range(END_BUFF_0_TYPE	  ,START_BUFF_0_TYPE	) = buf0_type_1;
  in_data_1.range(END_BUFF_1_TYPE	  ,START_BUFF_1_TYPE	) = buf1_type_1;
  in_data_1.range(END_BUFF_2_TYPE	  ,START_BUFF_2_TYPE	) = buf2_type_1;
  in_data_1.range(END_ADDR_A		    ,START_ADDR_A		    ) = addra_1;
  in_data_1.range(END_ADDR_B		    ,START_ADDR_B		    ) = addrb_1;
  in_data_1.range(END_ADDR_C		    ,START_ADDR_C		    ) = addrc_1;

  in_data_2.range(END_SCENARIO	    ,START_SCENARIO		  ) = scenario_2;
  in_data_2.range(END_LEN			      ,START_LEN			    ) = len_2;
  in_data_2.range(END_COMM		      ,START_COMM			    ) = comm_2;
  in_data_2.range(END_ROOT_SRC_DST  ,START_ROOT_SRC_DST	) = root_src_dst_2;
  in_data_2.range(END_FUNCTION	    ,START_FUNCTION		  ) = function_2;
  in_data_2.range(END_TAG			      ,START_TAG			    ) = msg_tag_2;
  in_data_2.range(END_BUFF_0_TYPE	  ,START_BUFF_0_TYPE	) = buf0_type_2;
  in_data_2.range(END_BUFF_1_TYPE	  ,START_BUFF_1_TYPE	) = buf1_type_2;
  in_data_2.range(END_BUFF_2_TYPE	  ,START_BUFF_2_TYPE	) = buf2_type_2;
  in_data_2.range(END_ADDR_A		    ,START_ADDR_A		    ) = addra_2;
  in_data_2.range(END_ADDR_B		    ,START_ADDR_B		    ) = addrb_2;
  in_data_2.range(END_ADDR_C		    ,START_ADDR_C		    ) = addrc_2;

  {
    #pragma HLS DATAFLOW
    cmd_0.write(in_data_0);
    cmd_1.write(in_data_1);
    cmd_2.write(in_data_2);
  }

  while(sts_0.empty() || sts_1.empty() || sts_2.empty()){
#pragma HLS protocol fixed
    ap_wait();
    if(flag_0 )
    {
      count_0 +=1;
      flag_0   = sts_0.empty();
    }
    if(flag_1 )
    {
      count_1 +=1;
      flag_1   = sts_1.empty();
    }
    if(flag_2 )
    {
      count_2 +=1;
      flag_2   = sts_2.empty();
    }
  }
  sts_0.read();
  sts_1.read();
  sts_2.read();
  ret.range(31,  0) = count_0;
  ret.range(63, 32) = count_1;
  ret.range(95, 64) = count_2;
  return ret;
}