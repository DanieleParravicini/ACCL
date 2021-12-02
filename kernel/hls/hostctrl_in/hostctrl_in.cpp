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
#include "hostctrl_in.h"

using namespace hls;
using namespace std;

#define DATA_WIDTH 512

void hostctrl_in_io(	
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
				stream<ap_uint<32>> &cmd,
				stream<ap_uint<32>> &sts,
				stream<ap_uint<32>> &sts_out
){
#pragma HLS INTERFACE axis port=cmd
#pragma HLS INTERFACE axis port=sts
#pragma HLS INTERFACE axis port=sts_out

io_section:{
	#pragma HLS protocol fixed
	cmd.write(scenario);
	ap_wait();
	cmd.write(len);
	ap_wait();
	cmd.write(comm);
	ap_wait();
	cmd.write(root_src_dst);
	ap_wait();
	cmd.write(function);
	ap_wait();
	cmd.write(msg_tag);
	ap_wait();
	cmd.write(buf0_type);
	ap_wait();
	cmd.write(buf1_type);
	ap_wait();
	cmd.write(buf2_type);
	ap_wait();
	cmd.write(addra(31,0));
	ap_wait();
	cmd.write(addra(63,32));
	ap_wait();
	cmd.write(addrb(31,0));
	ap_wait();
	cmd.write(addrb(63,32));
	ap_wait();
	cmd.write(addrc(31,0));
	ap_wait();
	cmd.write(addrc(63,32));
	ap_wait();
	sts_out.write(sts.read());
}

}

void hostctrl_in(	
                stream<ap_uint<DATA_WIDTH> > & cmd_in,
				stream<ap_uint<32> > & 		   cmd_out,
				stream<ap_uint<32> > & 		   sts_in,
				stream<ap_uint<32> > & 		   sts_out
) {

#pragma HLS INTERFACE axis port=cmd_in
#pragma HLS INTERFACE axis port=cmd_out
#pragma HLS INTERFACE axis port=sts_in
#pragma HLS INTERFACE axis port=sts_out
#pragma HLS INTERFACE ap_ctrl_none  port=return

    ap_uint<DATA_WIDTH> in_data = cmd_in.read();
    //Input stream needs to be optimized in the same way as hostctrl 
    ap_uint<32> scenario 	= in_data.range(END_SCENARIO,START_SCENARIO);
    ap_uint<32> len 		= in_data.range(END_LEN,START_LEN);
    ap_uint<32> comm		= in_data.range(END_COMM,START_COMM);
    ap_uint<32> root_src_dst= in_data.range(END_ROOT_SRC_DST,START_ROOT_SRC_DST);
    ap_uint<32> function 	= in_data.range(END_FUNCTION,START_FUNCTION);
    ap_uint<32> msg_tag 	= in_data.range(END_TAG,START_TAG);
    ap_uint<32> buf0_type 	= in_data.range(END_BUFF_0_TYPE,START_BUFF_0_TYPE);
    ap_uint<32> buf1_type 	= in_data.range(END_BUFF_1_TYPE,START_BUFF_1_TYPE);
    ap_uint<32> buf2_type 	= in_data.range(END_BUFF_2_TYPE,START_BUFF_2_TYPE);
    ap_uint<64> addra 	  	= in_data.range(END_ADDR_A,START_ADDR_A);
    ap_uint<64> addrb    	= in_data.range(END_ADDR_B,START_ADDR_B);
    ap_uint<64> addrc 		= in_data.range(END_ADDR_C,START_ADDR_C);
    // stream<ap_uint<32>> cmd;

	hostctrl_in_io(scenario, len, comm, root_src_dst, function, msg_tag, buf0_type, buf1_type, buf2_type, addra, addrb, addrc, cmd_out, sts_in, sts_out);
    
}
