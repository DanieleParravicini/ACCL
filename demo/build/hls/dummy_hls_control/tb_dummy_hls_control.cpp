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
#include<iostream>

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
				stream<ap_uint<32>> &sts
);

int main(){
	int nerrors = 0;

	stream<ap_uint<512>> cmd;
	stream<ap_uint<32>> sts;
	
	for(int i = 0; i < 10; i++){

		ap_uint<32> scenario 	= 1 + i; 
		ap_uint<32> len 		= 2 + i; 
		ap_uint<32> comm		= 3 + i; 
		ap_uint<32> root_src_dst= 4 + i; 
		ap_uint<32> function 	= 5 + i; 
		ap_uint<32> msg_tag 	= 6 + i; 
		ap_uint<32> buf0_type 	= 7 + i; 
		ap_uint<32> buf1_type 	= 8 + i; 
		ap_uint<32> buf2_type 	= 9 + i; 
		ap_uint<64> addra 	  	= 10 + i; 
		ap_uint<64> addrb    	= 11 + i; 
		ap_uint<64> addrc 		= 12 + i; 
		sts.write( 1 + i );
		dummy_hls_control(scenario, len, comm, root_src_dst, function, msg_tag, buf0_type, buf1_type, buf2_type, addra, addrb, addrc, cmd, sts);
		ap_uint<512> cmd_tmp = cmd.read();
		nerrors += (     scenario 	  != cmd_tmp.range(31,0)	);
		nerrors += (     len 		  != cmd_tmp.range(63,32)	);
		nerrors += (     comm		  != cmd_tmp.range(95,64)	);
		nerrors += (     root_src_dst != cmd_tmp.range(127,96)	);
		nerrors += (     function 	  != cmd_tmp.range(159,128) );
		nerrors += (     msg_tag 	  != cmd_tmp.range(191,160) );
		nerrors += (     buf0_type 	  != cmd_tmp.range(223,192) );
		nerrors += (     buf1_type 	  != cmd_tmp.range(255,224) );
		nerrors += (     buf2_type 	  != cmd_tmp.range(287,256) );
		nerrors += (     addra 	  	  != cmd_tmp.range(351,288) );
		nerrors += (     addrb    	  != cmd_tmp.range(415,352) );
		nerrors += (     addrc 		  != cmd_tmp.range(479,416) );
	}

	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << "******************************"<< std::endl;
	std::cout << "Found " << nerrors << " errors"<< std::endl;
	std::cout << "******************************"<< std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
	return 0;
}
