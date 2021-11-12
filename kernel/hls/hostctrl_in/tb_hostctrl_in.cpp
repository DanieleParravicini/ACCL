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
#include <iostream>

using namespace hls;
using namespace std;

#define DATA_WIDTH 512

void hostctrl_in(	
                stream<ap_uint<DATA_WIDTH> > & cmd_in,
				stream<ap_uint<32> > & 		   cmd_out,
				stream<ap_uint<32> > & 		   sts_in,
				stream<ap_uint<32> > & 		   sts_out
);

int main(){
	int nerrors = 0;
	ap_uint<DATA_WIDTH> 		cmd_data_in;
	stream<ap_uint<DATA_WIDTH>> cmd_in;
	stream<ap_uint<32>> 		cmd_out, sts_in, sts_out;
	ap_uint<32> sts_data = 42;
	for(int i=0; i < 10; i++ ){

		cmd_data_in.range(31,0)    				 = i+ 0;
		cmd_data_in.range(63,32)   				 = i+ 1;
		cmd_data_in.range(95,64)   				 = i+ 2;
		cmd_data_in.range(127,96)  				 = i+ 3;
		cmd_data_in.range(159,128) 				 = i+ 4;
		cmd_data_in.range(191,160) 				 = i+ 5;
		cmd_data_in.range(223,192) 				 = i+ 6;
		cmd_data_in.range(255,224) 				 = i+ 7;
		cmd_data_in.range(287,256) 				 = i+ 8;
		cmd_data_in.range(351,288) 				 = i+ 9;
		cmd_data_in.range(415,352) 				 = i+ 10;
		cmd_data_in.range(479,416) 				 = i+ 11;
		cmd_in.write(cmd_data_in);
		sts_in.write(sts_data + i);
		
		hostctrl_in(cmd_in, cmd_out, sts_in, sts_out);

		nerrors += ( cmd_out.read()			     != i + 0);
		nerrors += ( cmd_out.read()			     != i + 1);
		nerrors += ( cmd_out.read()			     != i + 2);
		nerrors += ( cmd_out.read()			     != i + 3);
		nerrors += ( cmd_out.read()				 != i + 4);
		nerrors += ( cmd_out.read()				 != i + 5);
		nerrors += ( cmd_out.read()				 != i + 6);
		nerrors += ( cmd_out.read()				 != i + 7);
		nerrors += ( cmd_out.read()				 != i + 8);
		ap_uint<64> addr_tmp = i + 9;
		ap_uint<32> tmp;
		tmp = cmd_out.read();
		nerrors += ( tmp			 != addr_tmp.range(31,0 ) );
		tmp = cmd_out.read();
		nerrors += ( tmp			 != addr_tmp.range(63,32) );
		addr_tmp = i + 10;
		tmp = cmd_out.read();
		nerrors += ( tmp			 != addr_tmp.range(31,0 ) );
		tmp = cmd_out.read();
		nerrors += ( tmp			 != addr_tmp.range(63,32) );
		addr_tmp = i + 11;
		tmp = cmd_out.read();
		nerrors += ( tmp			 != addr_tmp.range(31,0 ) );
		tmp = cmd_out.read();
		nerrors += ( tmp			 != addr_tmp.range(63,32) );
		nerrors += (sts_out.read() 	 != (sts_data + i) );
	}

	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << "******************************"<< std::endl;
	std::cout << "Found " << nerrors << " errors"<< std::endl;
	std::cout << "******************************"<< std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
	
	return (nerrors ==0 ? 0 : 1) ;
}
