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


//void dma_enqueue_internal(
//	ap_uint<32> 		nbufs,
//	stream<ap_uint<32>> &cmd_dma_queue,
//	stream<ap_uint<32>> &inflight_queue,
//	rx_buffer* rx_buffers
//);


void dma_enqueue(	
				ap_uint<32>  		use_tcp,
				ap_uint<32>  		nbufs,
				stream< ap_uint<32> > &cmd_dma_udp,
				stream< ap_uint<32> > &cmd_dma_tcp,
				stream< ap_uint<32> > &inflight_queue,
				rx_buffer* rx_buffers
) {
#pragma HLS INTERFACE s_axilite port=use_tcp
#pragma HLS INTERFACE s_axilite port=nbufs
#pragma HLS INTERFACE axis 		port=cmd_dma_udp	depth=20
#pragma HLS INTERFACE axis 		port=cmd_dma_tcp	depth=20
#pragma HLS INTERFACE axis 		port=inflight_queue	depth=20
#pragma HLS INTERFACE m_axi 	port=rx_buffers	
#pragma HLS INTERFACE s_axilite port=return

	//iterate until you run out of spare buffers
	for(ap_uint<32> i=0; i < nbufs; i++){
		#pragma HLS unroll factor=2
		#pragma HLS pipeline II=1
		//look for IDLE spare buffers
		if(rx_buffers[i].status   == STATUS_IDLE){
			//issue a cmd
			if (use_tcp){
				cmd_dma_tcp.write( 0xC0800000 | DMA_MAX_BTT	); // 31=DRR 30=EOF 29-24=DSA 23=Type 22-0=BTT
				cmd_dma_tcp.write( rx_buffers[i].addrl		);
				cmd_dma_tcp.write( rx_buffers[i].addrh		);
				cmd_dma_tcp.write( 0x2000 					); 	 // 15-12=xCACHE 11-8=xUSER 7-4=RSVD 3-0=TAG
			}else{
				cmd_dma_udp.write( 0xC0800000 | DMA_MAX_BTT	); // 31=DRR 30=EOF 29-24=DSA 23=Type 22-0=BTT
				cmd_dma_udp.write( rx_buffers[i].addrl		);
				cmd_dma_udp.write( rx_buffers[i].addrh		);
				cmd_dma_udp.write( 0x2000 					); 	 // 15-12=xCACHE 11-8=xUSER 7-4=RSVD 3-0=TAG
			}
			//write to the inflight queue the spare buffer address in the exchange memory
			inflight_queue.write(i);
			//update spare buffer status
			rx_buffers[i].status 	= STATUS_ENQUEUED;
		}
	}

}


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
