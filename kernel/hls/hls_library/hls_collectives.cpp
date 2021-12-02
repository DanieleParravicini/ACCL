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
#include "hls_stream.h"
#include "ap_int.h"
#include "ap_utils.h"
#include "hostctrl_in.h"

using namespace hls;
using namespace std;

#define DATA_WIDTH 512

int send_in(
    unsigned int comm,
    unsigned int len,
    unsigned int tag,
    unsigned int dst_rank,
    uint64_t buf_addr)
{
    #pragma HLS INTERFACE axis port=cmd_in
    #pragma HLS INTERFACE axis port=cmd_out
    #pragma HLS INTERFACE axis port=sts_in
    #pragma HLS INTERFACE axis port=sts_out
    #pragma HLS INTERFACE ap_ctrl_none  port=return 

    ap_uint<DATA_WIDTH> in_data = 0;
    in_data.range(31,0)  = XCCL_SEND;
    in_data.range(63,32) = len;
    in_data.range(95,64) = comm;
    in_data.range(127,96) = dst_rank;
    in_data.range(191,160) = tag;
    in_data.range(351,288) = buf_addr;

    stream <ap_uint<DATA_WIDTH> > cmd_in;
    cmd_in.write(in_data);
    stream <ap_uint<32> > cmd_out;
    stream <ap_uint<32> > sts_in;
    stream <ap_uint<32> > sts_out;
    hostctrl_in(cmd_in,cmd_out,sts_in,sts_out);
    return sts_out.read();
}

int recv_in(
    unsigned int comm,
    unsigned int len,
    unsigned int tag,
    unsigned int src_rank,
    uint64_t buf_addr)
{
    #pragma HLS INTERFACE axis port=cmd_in
    #pragma HLS INTERFACE axis port=cmd_out
    #pragma HLS INTERFACE axis port=sts_in
    #pragma HLS INTERFACE axis port=sts_out
    #pragma HLS INTERFACE ap_ctrl_none  port=return 
    
    ap_uint<DATA_WIDTH> in_data = 0;
    in_data.range(31,0)  = XCCL_RECV;
    in_data.range(63,32) = len;
    in_data.range(95,64) = comm;
    in_data.range(127,96) = src_rank;
    in_data.range(191,160) = tag;
    in_data.range(351,288) = buf_addr;

    stream <ap_uint<DATA_WIDTH> > cmd_in;
    cmd_in.write(in_data);
    stream <ap_uint<32> > cmd_out;
    stream <ap_uint<32> > sts_in;
    stream <ap_uint<32> > sts_out;
    hostctrl_in(cmd_in,cmd_out,sts_in,sts_out);
    return sts_out.read();
}

int broadcast_in(
    unsigned int comm,
    unsigned int len,
    unsigned int src_rank,
    uint64_t buf_addr)
{
    #pragma HLS INTERFACE axis port=cmd_in
    #pragma HLS INTERFACE axis port=cmd_out
    #pragma HLS INTERFACE axis port=sts_in
    #pragma HLS INTERFACE axis port=sts_out
    #pragma HLS INTERFACE ap_ctrl_none  port=return 

    ap_uint<DATA_WIDTH> in_data = 0;
    in_data.range(31,0)  = XCCL_BCAST;
    in_data.range(63,32) = len;
    in_data.range(95,64) = comm;
    in_data.range(127,96) = src_rank;
    in_data.range(351,288) = buf_addr;

    stream <ap_uint<DATA_WIDTH> > cmd_in;
    cmd_in.write(in_data);
    stream <ap_uint<32> > cmd_out;
    stream <ap_uint<32> > sts_in;
    stream <ap_uint<32> > sts_out;
    hostctrl_in(cmd_in,cmd_out,sts_in,sts_out);
    return sts_out.read();
}

int scatter_in(
    unsigned int comm,
    unsigned int len,
    unsigned int src_rank,
    uint64_t src_buf_addr,
    uint64_t dst_buf_addr
)
{
    #pragma HLS INTERFACE axis port=cmd_in
    #pragma HLS INTERFACE axis port=cmd_out
    #pragma HLS INTERFACE axis port=sts_in
    #pragma HLS INTERFACE axis port=sts_out
    #pragma HLS INTERFACE ap_ctrl_none  port=return 
    ap_uint<DATA_WIDTH> in_data = 0;
    
    in_data.range(31,0)  = XCCL_SCATTER;
    in_data.range(63,32) = len;
    in_data.range(95,64) = comm;
    in_data.range(127,96) = src_rank;
    in_data.range(351,288) = src_buf_addr;
    in_data.range(415,352) = dst_buf_addr;

    stream <ap_uint<DATA_WIDTH> > cmd_in;
    cmd_in.write(in_data);
    stream <ap_uint<32> > cmd_out;
    stream <ap_uint<32> > sts_in;
    stream <ap_uint<32> > sts_out;
    hostctrl_in(cmd_in,cmd_out,sts_in,sts_out);
    return sts_out.read();
}

int gather_in(
    unsigned int comm,
    unsigned int len,
    unsigned int root_rank,
    uint64_t src_buf_addr,
    uint64_t dst_buf_addr
){
    #pragma HLS INTERFACE axis port=cmd_in
    #pragma HLS INTERFACE axis port=cmd_out
    #pragma HLS INTERFACE axis port=sts_in
    #pragma HLS INTERFACE axis port=sts_out
    #pragma HLS INTERFACE ap_ctrl_none  port=return 
    ap_uint<DATA_WIDTH> in_data = 0;
    in_data.range(31,0)  = XCCL_GATHER;
    in_data.range(63,32) = len;
    in_data.range(95,64) = comm;
    in_data.range(127,96) = root_rank;
    in_data.range(351,288) = src_buf_addr;
    in_data.range(415,352) = dst_buf_addr;

    stream <ap_uint<DATA_WIDTH> > cmd_in;
    cmd_in.write(in_data);
    stream <ap_uint<32> > cmd_out;
    stream <ap_uint<32> > sts_in;
    stream <ap_uint<32> > sts_out;
    hostctrl_in(cmd_in,cmd_out,sts_in,sts_out);
    return sts_out.read();
}

int allgather_in(
    unsigned int comm,
    unsigned int len,
    uint64_t src_buf_addr,
    uint64_t dst_buf_addr)
{
    #pragma HLS INTERFACE axis port=cmd_in
    #pragma HLS INTERFACE axis port=cmd_out
    #pragma HLS INTERFACE axis port=sts_in
    #pragma HLS INTERFACE axis port=sts_out
    #pragma HLS INTERFACE ap_ctrl_none  port=return 
    ap_uint<DATA_WIDTH> in_data = 0;
    in_data.range(31,0)  = XCCL_ALLGATHER;
    in_data.range(63,32) = len;
    in_data.range(95,64) = comm;
    in_data.range(351,288) = src_buf_addr;
    in_data.range(415,352) = dst_buf_addr;

    stream <ap_uint<DATA_WIDTH> > cmd_in;
    cmd_in.write(in_data);
    stream <ap_uint<32> > cmd_out;
    stream <ap_uint<32> > sts_in;
    stream <ap_uint<32> > sts_out;
    hostctrl_in(cmd_in,cmd_out,sts_in,sts_out);
    return sts_out.read();
}

int reduce_in(
    unsigned int comm,
    unsigned int len,
    unsigned int function,
    unsigned int root_rank,
    uint64_t src_addr,
    uint64_t dst_addr)
{
    #pragma HLS INTERFACE axis port=cmd_in
    #pragma HLS INTERFACE axis port=cmd_out
    #pragma HLS INTERFACE axis port=sts_in
    #pragma HLS INTERFACE axis port=sts_out
    #pragma HLS INTERFACE ap_ctrl_none  port=return 
    ap_uint<DATA_WIDTH> in_data = 0;
    in_data.range(31,0)  = XCCL_REDUCE;
    in_data.range(63,32) = len;
    in_data.range(95,64) = comm;
    in_data.range(127,96) = root_rank;
    in_data.range(159,128) = function;
    in_data.range(351,288) = src_addr;
    in_data.range(415,352) = dst_addr;

    stream <ap_uint<DATA_WIDTH> > cmd_in;
    cmd_in.write(in_data);
    stream <ap_uint<32> > cmd_out;
    stream <ap_uint<32> > sts_in;
    stream <ap_uint<32> > sts_out;
    hostctrl_in(cmd_in,cmd_out,sts_in,sts_out);
    return sts_out.read();
}

int allreduce_in(
    unsigned int comm,
    unsigned int len,
    unsigned int function,
    uint64_t src_addr,
    uint64_t dst_addr)
{
    #pragma HLS INTERFACE axis port=cmd_in
    #pragma HLS INTERFACE axis port=cmd_out
    #pragma HLS INTERFACE axis port=sts_in
    #pragma HLS INTERFACE axis port=sts_out
    #pragma HLS INTERFACE ap_ctrl_none  port=return 
    ap_uint<DATA_WIDTH> in_data = 0;
    in_data.range(31,0)  = XCCL_ALLREDUCE;
    in_data.range(63,32) = len;
    in_data.range(95,64) = comm;
    in_data.range(159,128) = function;
    in_data.range(351,288) = src_addr;
    in_data.range(415,352) = dst_addr;

    stream <ap_uint<DATA_WIDTH> > cmd_in;
    cmd_in.write(in_data);
    stream <ap_uint<32> > cmd_out;
    stream <ap_uint<32> > sts_in;
    stream <ap_uint<32> > sts_out;
    hostctrl_in(cmd_in,cmd_out,sts_in,sts_out);
    return sts_out.read();
}

int config_in(unsigned int function)
{
    #pragma HLS INTERFACE axis port=cmd_in
    #pragma HLS INTERFACE axis port=cmd_out
    #pragma HLS INTERFACE axis port=sts_in
    #pragma HLS INTERFACE axis port=sts_out
    #pragma HLS INTERFACE ap_ctrl_none  port=return 
    ap_uint<DATA_WIDTH> in_data = 0;
    in_data.range(31,0)  = XCCL_CONFIG;
    in_data.range(159,128) = function;
    stream <ap_uint<DATA_WIDTH> > cmd_in;
    cmd_in.write(in_data);
    stream <ap_uint<32> > cmd_out;
    stream <ap_uint<32> > sts_in;
    stream <ap_uint<32> > sts_out;
    hostctrl_in(cmd_in,cmd_out,sts_in,sts_out);
    return sts_out.read();
}

int accumulate_in(
    unsigned int len,
    unsigned int function,
    uint64_t op0_addr,
    uint64_t op1_addr)//OP1_ADDR = DST_ADDR
    // uint64_t dst_addr)
{
    #pragma HLS INTERFACE axis port=cmd_in
    #pragma HLS INTERFACE axis port=cmd_out
    #pragma HLS INTERFACE axis port=sts_in
    #pragma HLS INTERFACE axis port=sts_out
    #pragma HLS INTERFACE ap_ctrl_none  port=return 
    ap_uint<DATA_WIDTH> in_data = 0;
    in_data.range(31,0)  = XCCL_ACC;
    in_data.range(63,32) = len;
    in_data.range(159,128) = function;
    in_data.range(351,288) = op0_addr;
    in_data.range(415,352) = op1_addr;
    stream <ap_uint<DATA_WIDTH> > cmd_in;
    cmd_in.write(in_data);
    stream <ap_uint<32> > cmd_out;
    stream <ap_uint<32> > sts_in;
    stream <ap_uint<32> > sts_out;
    hostctrl_in(cmd_in,cmd_out,sts_in,sts_out);
    return sts_out.read();
}

int copy_in(
    unsigned int len,
    uint64_t src_addr,
    uint64_t dst_addr)
{
    #pragma HLS INTERFACE axis port=cmd_in
    #pragma HLS INTERFACE axis port=cmd_out
    #pragma HLS INTERFACE axis port=sts_in
    #pragma HLS INTERFACE axis port=sts_out
    #pragma HLS INTERFACE ap_ctrl_none  port=return 
    ap_uint<DATA_WIDTH> in_data = 0;
    in_data.range(31,0)  = XCCL_COPY;
    in_data.range(63,32) = len;
    in_data.range(351,288) = src_addr;
    in_data.range(415,352) = dst_addr;
    stream <ap_uint<DATA_WIDTH> > cmd_in;
    cmd_in.write(in_data);
    stream <ap_uint<32> > cmd_out;
    stream <ap_uint<32> > sts_in;
    stream <ap_uint<32> > sts_out;
    hostctrl_in(cmd_in,cmd_out,sts_in,sts_out);
    return sts_out.read();
}

int ext_kernel_stream_in(
    unsigned int len,
    uint64_t src_addr,
    uint64_t dst_addr)
{
    #pragma HLS INTERFACE axis port=cmd_in
    #pragma HLS INTERFACE axis port=cmd_out
    #pragma HLS INTERFACE axis port=sts_in
    #pragma HLS INTERFACE axis port=sts_out
    #pragma HLS INTERFACE ap_ctrl_none  port=return 
    ap_uint<DATA_WIDTH> in_data = 0;
    in_data.range(31,0)  = XCCL_EXT_STREAM_KRNL;
    in_data.range(63,32) = len;
    in_data.range(351,288) = src_addr;
    in_data.range(415,352) = dst_addr;
    stream <ap_uint<DATA_WIDTH> > cmd_in;
    cmd_in.write(in_data);
    stream <ap_uint<32> > cmd_out;
    stream <ap_uint<32> > sts_in;
    stream <ap_uint<32> > sts_out;
    hostctrl_in(cmd_in,cmd_out,sts_in,sts_out);
    return sts_out.read();
}

int reduce_ext_in(
    unsigned int len,
    uint64_t op1_addr,
    uint64_t op2_addr,
    uint64_t dst_addr)
{
    #pragma HLS INTERFACE axis port=cmd_in
    #pragma HLS INTERFACE axis port=cmd_out
    #pragma HLS INTERFACE axis port=sts_in
    #pragma HLS INTERFACE axis port=sts_out
    #pragma HLS INTERFACE ap_ctrl_none  port=return 
    ap_uint<DATA_WIDTH> in_data = 0;
    in_data.range(31,0)  = XCCL_EXT_REDUCE;
    in_data.range(63,32) = len;
    in_data.range(351,288) = op1_addr;
    in_data.range(415,352) = op2_addr;
    in_data.range(479,416) = dst_addr;
    stream <ap_uint<DATA_WIDTH> > cmd_in;
    cmd_in.write(in_data);
    stream <ap_uint<32> > cmd_out;
    stream <ap_uint<32> > sts_in;
    stream <ap_uint<32> > sts_out;
    hostctrl_in(cmd_in,cmd_out,sts_in,sts_out);
    return sts_out.read();
}

int scatter_reduce_in(
    unsigned int comm,
    unsigned int len,
    unsigned int function,
    uint64_t src_addr,
    uint64_t dst_addr)
{
    #pragma HLS INTERFACE axis port=cmd_in
    #pragma HLS INTERFACE axis port=cmd_out
    #pragma HLS INTERFACE axis port=sts_in
    #pragma HLS INTERFACE axis port=sts_out
    #pragma HLS INTERFACE ap_ctrl_none  port=return 
    ap_uint<DATA_WIDTH> in_data = 0;
    in_data.range(31,0)  = XCCL_REDUCE_SCATTER;
    in_data.range(63,32) = len;
    in_data.range(95,64) = comm;
    in_data.range(159,128) = function;
    in_data.range(351,288) = src_addr;
    in_data.range(415,352) = dst_addr;
    stream <ap_uint<DATA_WIDTH> > cmd_in;
    cmd_in.write(in_data);
    stream <ap_uint<32> > cmd_out;
    stream <ap_uint<32> > sts_in;
    stream <ap_uint<32> > sts_out;
    hostctrl_in(cmd_in,cmd_out,sts_in,sts_out);
    return sts_out.read();
}