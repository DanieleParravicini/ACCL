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
#ifndef _HLS_COLLECTIVES_H_
#define _HLS_COLLECTIVES_H_

//******************************
//**  ACC COLLECTIVE          **
//******************************
#define ACCL_CONFIG               0
#define ACCL_SEND                 1 
#define ACCL_RECV                 2
#define ACCL_BCAST                3
#define ACCL_SCATTER              4
#define ACCL_GATHER               5
#define ACCL_REDUCE               6
#define ACCL_ALLGATHER            7
#define ACCL_ALLREDUCE            8
#define ACCL_ACC                  9
#define ACCL_COPY                 10
#define ACCL_EXT_STREAM_KRNL      15
#define ACCL_EXT_REDUCE           16
#define ACCL_REDUCE_SCATTER       20

ap_uint<32> send_in(
    unsigned int comm,
    unsigned int len,
    unsigned int tag,
    unsigned int dst_rank,
    uint64_t buf_addr);

ap_uint<32> recv_in(
    unsigned int comm,
    unsigned int len,
    unsigned int tag,
    unsigned int src_rank,
    uint64_t buf_addr);

ap_uint<32> broadcast_in(
    unsigned int comm,
    unsigned int len,
    unsigned int src_rank,
    uint64_t buf_addr);

ap_uint<32> scatter_in(
    unsigned int comm,
    unsigned int len,
    unsigned int src_rank,
    uint64_t src_buf_addr,
    uint64_t dst_buf_addr);

ap_uint<32> gather_in(
    unsigned int comm,
    unsigned int len,
    unsigned int root_rank,
    uint64_t src_buf_addr,
    uint64_t dst_buf_addr);

ap_uint<32> allgather_in(
    unsigned int comm,
    unsigned int len,
    uint64_t src_buf_addr,
    uint64_t dst_buf_addr);

ap_uint<32> reduce_in(
    unsigned int comm,
    unsigned int len,
    unsigned int function,
    unsigned int root_rank,
    uint64_t src_addr,
    uint64_t dst_addr);

ap_uint<32> allreduce_in(
    unsigned int comm,
    unsigned int len,
    unsigned int function,
    uint64_t src_addr,
    uint64_t dst_addr);

ap_uint<32> config_in(unsigned int function);

ap_uint<32> accumulate_in(
    unsigned int len,
    unsigned int function,
    uint64_t op0_addr,
    uint64_t op1_addr);

ap_uint<32> copy_in(
    unsigned int len,
    uint64_t src_addr,
    uint64_t dst_addr);

ap_uint<32> ext_kernel_stream_in(
    unsigned int len,
    uint64_t src_addr,
    uint64_t dst_addr);

ap_uint<32> reduce_ext_in(
    unsigned int len,
    uint64_t op1_addr,
    uint64_t op2_addr,
    uint64_t dst_addr);

ap_uint<32> scatter_reduce_in(
    unsigned int comm,
    unsigned int len,
    unsigned int function,
    uint64_t src_addr,
    uint64_t dst_addr);

#endif //_HLS_COLLECTIVES_H_

