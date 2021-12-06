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
#ifndef _HLS_CTRL_IN_H_
#define _HLS_CTRL_IN_H_
#define DATA_WIDTH			512
#define START_SCENARIO 		0
#define END_SCENARIO 		(START_SCENARIO+32-1)
#define START_LEN 	 		(END_SCENARIO+1)
#define END_LEN 	 		(START_LEN+32-1)
#define START_COMM			(END_LEN+1)
#define END_COMM			(START_COMM+32-1)
#define START_ROOT_SRC_DST	(END_COMM+1)
#define END_ROOT_SRC_DST	(START_ROOT_SRC_DST+32-1)
#define START_FUNCTION		(END_ROOT_SRC_DST+1)
#define END_FUNCTION		(START_FUNCTION+32-1)
#define START_TAG			(END_FUNCTION+1)
#define END_TAG				(START_TAG+32-1)
#define START_BUFF_0_TYPE	(END_TAG+1)
#define END_BUFF_0_TYPE		(START_BUFF_0_TYPE+32-1)
#define START_BUFF_1_TYPE	(END_BUFF_0_TYPE+1)
#define END_BUFF_1_TYPE		(START_BUFF_1_TYPE+32-1)
#define START_BUFF_2_TYPE	(END_BUFF_1_TYPE+1)
#define END_BUFF_2_TYPE		(START_BUFF_2_TYPE+32-1)
#define START_ADDR_A		(END_BUFF_2_TYPE+1)
#define END_ADDR_A			(START_ADDR_A+64-1)
#define START_ADDR_B		(END_ADDR_A+1)
#define END_ADDR_B			(START_ADDR_B+64-1)
#define START_ADDR_C		(END_ADDR_B+1)
#define END_ADDR_C			(START_ADDR_C+64-1)

void hostctrl_in(	
                stream<ap_uint<DATA_WIDTH> > & cmd_in,
				stream<ap_uint<32> > & 		   cmd_out,
				stream<ap_uint<32> > & 		   sts_in,
				stream<ap_uint<32> > & 		   sts_out
);

#endif // _HLS_CTRL_IN_H_