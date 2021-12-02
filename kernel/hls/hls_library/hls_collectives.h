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

int send_in(
    unsigned int comm,
    unsigned int len,
    unsigned int tag,
    unsigned int dst_rank,
    uint64_t buf_addr);

int recv_in(
    unsigned int comm,
    unsigned int len,
    unsigned int tag,
    unsigned int src_rank,
    uint64_t buf_addr);

int broadcast_in(
    unsigned int comm,
    unsigned int len,
    unsigned int src_rank,
    uint64_t buf_addr);

int scatter_in(
    unsigned int comm,
    unsigned int len,
    unsigned int src_rank,
    uint64_t src_buf_addr,
    uint64_t dst_buf_addr);

int gather_in(
    unsigned int comm,
    unsigned int len,
    unsigned int root_rank,
    uint64_t src_buf_addr,
    uint64_t dst_buf_addr);

int allgather_in(
    unsigned int comm,
    unsigned int len,
    uint64_t src_buf_addr,
    uint64_t dst_buf_addr);

int reduce_in(
    unsigned int comm,
    unsigned int len,
    unsigned int function,
    unsigned int root_rank,
    uint64_t src_addr,
    uint64_t dst_addr);

int allreduce_in(
    unsigned int comm,
    unsigned int len,
    unsigned int function,
    uint64_t src_addr,
    uint64_t dst_addr);

int config_in(unsigned int function);

int accumulate_in(
    unsigned int len,
    unsigned int function,
    uint64_t op0_addr,
    uint64_t op1_addr);

int copy_in(
    unsigned int len,
    uint64_t src_addr,
    uint64_t dst_addr);

int ext_kernel_stream_in(
    unsigned int len,
    uint64_t src_addr,
    uint64_t dst_addr);

int reduce_ext_in(
    unsigned int len,
    uint64_t op1_addr,
    uint64_t op2_addr,
    uint64_t dst_addr);

int scatter_reduce_in(
    unsigned int comm,
    unsigned int len,
    unsigned int function,
    uint64_t src_addr,
    uint64_t dst_addr);

#endif //_HLS_COLLECTIVES_H_

