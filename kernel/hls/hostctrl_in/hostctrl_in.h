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

//******************************
//**  XCC COLLECTIVE          **
//******************************
#define XCCL_CONFIG               0
#define XCCL_SEND                 1 
#define XCCL_RECV                 2
#define XCCL_BCAST                3
#define XCCL_SCATTER              4
#define XCCL_GATHER               5
#define XCCL_REDUCE               6
#define XCCL_ALLGATHER            7
#define XCCL_ALLREDUCE            8
#define XCCL_ACC                  9
#define XCCL_COPY                 10
#define XCCL_EXT_STREAM_KRNL      15
#define XCCL_EXT_REDUCE           16
#define XCCL_REDUCE_SCATTER       20

void hostctrl_in(	
                stream<ap_uint<DATA_WIDTH> > & cmd_in,
				stream<ap_uint<32> > & 		   cmd_out,
				stream<ap_uint<32> > & 		   sts_in,
				stream<ap_uint<32> > & 		   sts_out
);

#endif // _HLS_CTRL_IN_H_