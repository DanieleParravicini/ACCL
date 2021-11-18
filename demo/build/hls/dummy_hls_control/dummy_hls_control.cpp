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
# *******************************************************************************/

#include "hls_stream.h"
#include "ap_int.h"
#include "ap_utils.h"

using namespace hls;
using namespace std;

void dummy_hls_control(	ap_uint<32> scenario,
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
				stream<ap_uint<512>> &cmd,
				stream<ap_uint<32>>  &sts
) {
#pragma HLS INTERFACE s_axilite port=scenario
#pragma HLS INTERFACE s_axilite port=len
#pragma HLS INTERFACE s_axilite port=comm
#pragma HLS INTERFACE s_axilite port=root_src_dst
#pragma HLS INTERFACE s_axilite port=function
#pragma HLS INTERFACE s_axilite port=msg_tag
#pragma HLS INTERFACE s_axilite port=buf0_type
#pragma HLS INTERFACE s_axilite port=buf1_type
#pragma HLS INTERFACE s_axilite port=buf2_type
#pragma HLS INTERFACE s_axilite port=addra
#pragma HLS INTERFACE s_axilite port=addrb
#pragma HLS INTERFACE s_axilite port=addrc
#pragma HLS INTERFACE axis port=cmd
#pragma HLS INTERFACE axis port=sts
#pragma HLS INTERFACE s_axilite port=return
	
	ap_uint<512> in_data ;
	//Input stream needs to be optimized in the same way as hostctrl 
    in_data.range(31,0) 	= scenario 		;
    in_data.range(63,32) 	= len 			;
    in_data.range(95,64) 	= comm			;
    in_data.range(127,96) 	= root_src_dst	;
    in_data.range(159,128) 	= function 		;
    in_data.range(191,160) 	= msg_tag 		;
    in_data.range(223,192) 	= buf0_type 	;
    in_data.range(255,224) 	= buf1_type 	;
    in_data.range(287,256) 	= buf2_type 	;
    in_data.range(351,288) 	= addra 	  	;
    in_data.range(415,352) 	= addrb    		;
    in_data.range(479,416) 	= addrc 		;
    {
		#pragma HLS protocol fixed 
		cmd.write(in_data);
		ap_wait();
		sts.read();
	}
}