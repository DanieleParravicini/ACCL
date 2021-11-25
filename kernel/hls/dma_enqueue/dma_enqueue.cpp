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

#include "dma_enqueue.h"

using namespace hls;
using namespace std;

void read_spares(
		ap_uint<32>  		  nbufs,
		ap_uint<32>* 		  spare_buffers,
		stream< ap_uint<32> > &tmp
		);

// start a DMA operations on a specific channel
void start_dma(
    stream<ap_uint<104> > &dma_cmd_channel,
    unsigned int btt,
    ap_uint<64> addr,
    unsigned int tag,
	unsigned int tlast_expected=1) {
  ap_uint<104> dma_cmd;

  dma_cmd.range( 31,  0) 	= 0x80800000 | btt;  // 31=DRR 30=EOF 29-24=DSA 23=Type 22-0=BTT 
  dma_cmd.range( 30, 30)    = tlast_expected;
  dma_cmd.range( 95, 32) 	= addr;
  dma_cmd.range(103, 96) 	= 	  0x2000 | tag; //15-12=xCACHE 11-8=xUSER 7-4=RSVD 3-0=TAG
  dma_cmd_channel.write(dma_cmd);
}


void dma_enqueue(	
				ap_uint<32>  		use_tcp,
				ap_uint<32>  		nbufs,
				stream< ap_uint<104> > &cmd_dma_udp,
				stream< ap_uint<104> > &cmd_dma_tcp,
				stream< ap_uint<32> > &inflight_queue,
				ap_uint<32>* rx_buffers
) {
#pragma HLS INTERFACE s_axilite port=use_tcp
#pragma HLS INTERFACE s_axilite port=nbufs
#pragma HLS INTERFACE axis 		port=cmd_dma_udp
#pragma HLS INTERFACE axis 		port=cmd_dma_tcp
#pragma HLS INTERFACE axis 		port=inflight_queue
#pragma HLS INTERFACE m_axi 	port=rx_buffers	depth=9*16 offset=slave num_read_outstanding=4	num_write_outstanding=4 bundle=mem
#pragma HLS INTERFACE s_axilite port=return
	#pragma HLS PIPELINE II=4 rewind 

	//iterate until you run out of spare buffers
	elaborate_spares: for(ap_uint<32> i=0; i < nbufs; i++){
		#pragma HLS pipeline II=1
		ap_uint<32> status, max_len;
		ap_uint<64> addr;
		status 				= *(rx_buffers + (i * SPARE_BUFFER_FIELDS) + STATUS_OFFSET	);
		addr.range( 31,  0) = *(rx_buffers + (i * SPARE_BUFFER_FIELDS) + ADDRL_OFFSET	);
		addr.range( 63, 32) = *(rx_buffers + (i * SPARE_BUFFER_FIELDS) + ADDRH_OFFSET	);
		max_len 			= *(rx_buffers + (i * SPARE_BUFFER_FIELDS) + MAX_LEN_OFFSET	);

		//look for IDLE spare buffers
		//can't be pipelined fully because of this test.
		if(status   == STATUS_IDLE){
			//issue a cmd
			if (use_tcp){
				start_dma( cmd_dma_tcp, max_len, 0, 1  );
			}else{
				start_dma( cmd_dma_udp, max_len, 0, 1  );
			}
			//update spare buffer status
			rx_buffers[ (i * SPARE_BUFFER_FIELDS) + STATUS_OFFSET ] 	= STATUS_ENQUEUED ;
			//write to the in flight queue the spare buffer address in the exchange memory
			inflight_queue.write(i);
		}
	}

}

//from impl/ip/xdma_enqueue_hw.h
// ==============================================================
// Vivado(TM) HLS - High-Level Synthesis from C, C++ and SystemC v2020.1 (64-bit)
// Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
// ==============================================================
// AXILiteS
// 0x00 : Control signals
//        bit 0  - ap_start (Read/Write/COH)
//        bit 1  - ap_done (Read/COR)
//        bit 2  - ap_idle (Read)
//        bit 3  - ap_ready (Read)
//        bit 7  - auto_restart (Read/Write)
//        others - reserved
// 0x04 : Global Interrupt Enable Register
//        bit 0  - Global Interrupt Enable (Read/Write)
//        others - reserved
// 0x08 : IP Interrupt Enable Register (Read/Write)
//        bit 0  - Channel 0 (ap_done)
//        bit 1  - Channel 1 (ap_ready)
//        others - reserved
// 0x0c : IP Interrupt Status Register (Read/TOW)
//        bit 0  - Channel 0 (ap_done)
//        bit 1  - Channel 1 (ap_ready)
//        others - reserved
// 0x10 : Data signal of use_tcp_V
//        bit 31~0 - use_tcp_V[31:0] (Read/Write)
// 0x14 : reserved
// 0x18 : Data signal of nbufs_V
//        bit 31~0 - nbufs_V[31:0] (Read/Write)
// 0x1c : reserved
// 0x20 : Data signal of rx_buffers_addrl
//        bit 31~0 - rx_buffers_addrl[31:0] (Read/Write)
// 0x24 : reserved
// 0x28 : Data signal of rx_buffers_addrh
//        bit 31~0 - rx_buffers_addrh[31:0] (Read/Write)
// 0x2c : reserved
// 0x30 : Data signal of rx_buffers_max_len
//        bit 31~0 - rx_buffers_max_len[31:0] (Read/Write)
// 0x34 : reserved
// 0x38 : Data signal of rx_buffers_dma_tag
//        bit 31~0 - rx_buffers_dma_tag[31:0] (Read/Write)
// 0x3c : reserved
// 0x40 : Data signal of rx_buffers_status
//        bit 31~0 - rx_buffers_status[31:0] (Read/Write)
// 0x44 : reserved
// 0x48 : Data signal of rx_buffers_rx_tag
//        bit 31~0 - rx_buffers_rx_tag[31:0] (Read/Write)
// 0x4c : reserved
// 0x50 : Data signal of rx_buffers_rx_len
//        bit 31~0 - rx_buffers_rx_len[31:0] (Read/Write)
// 0x54 : reserved
// 0x58 : Data signal of rx_buffers_rx_src
//        bit 31~0 - rx_buffers_rx_src[31:0] (Read/Write)
// 0x5c : reserved
// 0x60 : Data signal of rx_buffers_sequence_number
//        bit 31~0 - rx_buffers_sequence_number[31:0] (Read/Write)
// 0x64 : reserved
// (SC = Self Clear, COR = Clear on Read, TOW = Toggle on Write, COH = Clear on Handshake)

//#define XDMA_ENQUEUE_AXILITES_ADDR_AP_CTRL                         0x00
//#define XDMA_ENQUEUE_AXILITES_ADDR_GIE                             0x04
//#define XDMA_ENQUEUE_AXILITES_ADDR_IER                             0x08
//#define XDMA_ENQUEUE_AXILITES_ADDR_ISR                             0x0c
//#define XDMA_ENQUEUE_AXILITES_ADDR_USE_TCP_V_DATA                  0x10
//#define XDMA_ENQUEUE_AXILITES_BITS_USE_TCP_V_DATA                  32
//#define XDMA_ENQUEUE_AXILITES_ADDR_NBUFS_V_DATA                    0x18
//#define XDMA_ENQUEUE_AXILITES_BITS_NBUFS_V_DATA                    32
//#define XDMA_ENQUEUE_AXILITES_ADDR_RX_BUFFERS_ADDRL_DATA           0x20
//#define XDMA_ENQUEUE_AXILITES_BITS_RX_BUFFERS_ADDRL_DATA           32
//#define XDMA_ENQUEUE_AXILITES_ADDR_RX_BUFFERS_ADDRH_DATA           0x28
//#define XDMA_ENQUEUE_AXILITES_BITS_RX_BUFFERS_ADDRH_DATA           32
//#define XDMA_ENQUEUE_AXILITES_ADDR_RX_BUFFERS_MAX_LEN_DATA         0x30
//#define XDMA_ENQUEUE_AXILITES_BITS_RX_BUFFERS_MAX_LEN_DATA         32
//#define XDMA_ENQUEUE_AXILITES_ADDR_RX_BUFFERS_DMA_TAG_DATA         0x38
//#define XDMA_ENQUEUE_AXILITES_BITS_RX_BUFFERS_DMA_TAG_DATA         32
//#define XDMA_ENQUEUE_AXILITES_ADDR_RX_BUFFERS_STATUS_DATA          0x40
//#define XDMA_ENQUEUE_AXILITES_BITS_RX_BUFFERS_STATUS_DATA          32
//#define XDMA_ENQUEUE_AXILITES_ADDR_RX_BUFFERS_RX_TAG_DATA          0x48
//#define XDMA_ENQUEUE_AXILITES_BITS_RX_BUFFERS_RX_TAG_DATA          32
//#define XDMA_ENQUEUE_AXILITES_ADDR_RX_BUFFERS_RX_LEN_DATA          0x50
//#define XDMA_ENQUEUE_AXILITES_BITS_RX_BUFFERS_RX_LEN_DATA          32
//#define XDMA_ENQUEUE_AXILITES_ADDR_RX_BUFFERS_RX_SRC_DATA          0x58
//#define XDMA_ENQUEUE_AXILITES_BITS_RX_BUFFERS_RX_SRC_DATA          32
//#define XDMA_ENQUEUE_AXILITES_ADDR_RX_BUFFERS_SEQUENCE_NUMBER_DATA 0x60
//#define XDMA_ENQUEUE_AXILITES_BITS_RX_BUFFERS_SEQUENCE_NUMBER_DATA 32



//inline void dma_cmd_addrh_addrl(stream<ap_uint<32>> &cmd_dmax, unsigned int btt, unsigned int addrh, unsigned int addrl, unsigned int tag){
//	cmd_dmax.write( 0xC0800000 | btt); // 31=DRR 30=EOF 29-24=DSA 23=Type 22-0=BTT
//	cmd_dmax.write( addrl);
//	cmd_dmax.write( addrh);
//	cmd_dmax.write( 0x2000 | tag); 	 // 15-12=xCACHE 11-8=xUSER 7-4=RSVD 3-0=TAG
//}
//
//ap_uint<32> cur_buf_idx = 0;
//void dma_enqueue_internal(
//	ap_uint<32>			 nbufs,
//	stream<ap_uint<32>> &cmd_dma_queue,
//	stream<ap_uint<32>> &inflight_queue,
//	rx_buffer* rx_buffers
//){
//	//iterate until you run out of spare buffers
//	for(ap_uint<32> i=0; i < nbufs; i++, rx_buffers++){
//		//look for IDLE spare buffers
//		if(rx_buffers->status   == STATUS_IDLE){
//			//issue a cmd
//			dma_cmd_addrh_addrl(cmd_dma_queue , DMA_MAX_BTT, rx_buffers->addrh, rx_buffers->addrl, 0);
//			//write to the inflight queue the spare buffer address in the exchange memory
//			inflight_queue.write(i);
//			//update spare buffer status
//			rx_buffers->status 	= STATUS_ENQUEUED;
//		}
//	}
//}


//enques cmd from DMA that receives from network stack. 
//RX address queue management
//maintaint a list of N buffers in device memory
//queue those buffers for receives
/*
unsigned int enqueue_rx_buffers(void){
	unsigned int nbufs = Xil_In32(RX_BUFFER_COUNT_OFFSET);
	rx_buffer *rx_buf_list = (rx_buffer*)(RX_BUFFER_COUNT_OFFSET+4);
	unsigned int ret = 0, cmd_queue;
	int i,new_dma_tag;
	cmd_queue = use_tcp ? CMD_DMA2_TX : cmd_dma_udp_TX;
	for(i=0; i<nbufs; i++){
		//if((rx_buf_list[i].enqueued == 1) && (rx_buf_list[i].dma_tag == next_rx_tag)) return;
		if(num_rx_enqueued >= 16) return ret;
		if(rx_buf_list[i].status   != STATUS_IDLE) continue;
		//found a spare buffer to enqueue 
		//look for a new dma tag
		for(new_dma_tag=0; new_dma_tag < MAX_DMA_TAGS && dma_tag_lookup[new_dma_tag] != -1; new_dma_tag++);
		//new_dma_tag now holds the new dma tag to use
		if( new_dma_tag >= MAX_DMA_TAGS) return ret; //but something probably wrong in num_rx_enqueued
	
		//whatever we find now we can enqueue
		dma_cmd_addrh_addrl(cmd_queue, DMA_MAX_BTT, rx_buf_list[i].addrh, rx_buf_list[i].addrl, new_dma_tag);
		rx_buf_list[i].status 	= STATUS_ENQUEUED;
		rx_buf_list[i].dma_tag 	= new_dma_tag;
		dma_tag_lookup[new_dma_tag] = i;
		//next_rx_tag = (next_rx_tag + 1) & 0xf;
		num_rx_enqueued++;
		ret ++;
	}

	return ret;
}

static inline void dma_cmd_addrh_addrl(unsigned int channel, unsigned int btt, unsigned int addrh, unsigned int addrl, unsigned int tag){
	putd(channel, 0xC0800000 | btt); // 31=DRR 30=EOF 29-24=DSA 23=Type 22-0=BTT
	putd(channel, addrl);
	putd(channel, addrh);
	cputd(channel, 0x2000 | tag); 	 // 15-12=xCACHE 11-8=xUSER 7-4=RSVD 3-0=TAG
}


*/
