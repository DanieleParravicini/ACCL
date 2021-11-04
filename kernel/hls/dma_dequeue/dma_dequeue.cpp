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

#include "dma_dequeue.h"

using namespace hls;
using namespace std;

void write_rx_header(
	stream< ap_uint<32> >  &header,
	ap_uint<32> 			btt, 
	ap_uint<32> 			spare_idx, 
	ap_uint<32>*			exchange_mem 		  
){
	exchange_mem[ spare_idx * SPARE_BUFFER_FIELDS + RX_TAG_OFFSET			]	= header.read();
	exchange_mem[ spare_idx * SPARE_BUFFER_FIELDS + RX_LEN_OFFSET			]	= btt;
	exchange_mem[ spare_idx * SPARE_BUFFER_FIELDS + RX_SRC_OFFSET			]	= header.read();
	exchange_mem[ spare_idx * SPARE_BUFFER_FIELDS + SEQUENCE_NUMBER_OFFSET	]	= header.read();
}

void write_status(
	stream< ap_uint<32> > &sts,
	ap_uint<32> 		   btt,
	ap_uint<32> 		   spare_idx,	
	ap_uint<32> *		   exchange_mem
){
	ap_uint<32> status = sts.read(), new_status;
	//interpret dma sts and write new spare_sts
	// 3-0 TAG 
	// 4 INTERNAL 	ERROR usually a btt=0 trigger this
	// 5 DECODE 	ERROR address decode error timeout
	// 6 SLAVE 		ERROR DMA encountered a slave reported error
	// 7 OKAY		the associated transfer command has been completed with the OKAY response on all intermediate transfers.
	if( ((status & 0x00000080) == 0) | (status & 0x00000070) | !(status & 0x80000000)| ( ( (status & 0x7FFFFF00) >> 8) != btt) ){
		new_status = STATUS_ERROR;
	}else{
		new_status = STATUS_RESERVED;
	}
	exchange_mem[ spare_idx * SPARE_BUFFER_FIELDS + STATUS_OFFSET] = new_status;
}

void dma_dequeue(	
				unsigned int use_tcp,
				stream< ap_uint<32> > &sts_dma_udp,
				stream< ap_uint<32> > &sts_dma_tcp,
				stream< ap_uint<32> > &header_udp,
				stream< ap_uint<32> > &header_tcp,
				stream< ap_uint<32> > &inflight_queue,
				ap_uint<32>* 		   exchange_mem
) {
#pragma HLS INTERFACE s_axilite port=use_tcp
#pragma HLS INTERFACE axis 		port=sts_dma_udp
#pragma HLS INTERFACE axis 		port=sts_dma_tcp
#pragma HLS INTERFACE axis 		port=header_udp
#pragma HLS INTERFACE axis 		port=header_tcp
#pragma HLS INTERFACE axis 		port=inflight_queue
#pragma HLS INTERFACE m_axi 	port=exchange_mem depth=16*9 offset=slave num_read_outstanding=4	num_write_outstanding=4  bundle=mem
#pragma HLS INTERFACE s_axilite port=return

	//get rx_buffer pointer from inflight queue
	ap_uint<32> spare_idx = inflight_queue.read(), btt, status, new_status;
	if(use_tcp){

		//if ( *( exchange_mem + (spare_idx * SPARE_BUFFER_FIELDS) + STATUS_OFFSET) != STATUS_ENQUEUED){
		//	 exchange_mem[ spare_idx * SPARE_BUFFER_FIELDS + STATUS_OFFSET] = STATUS_ERROR;
		//}else
		{
			btt 					= header_tcp.read();

			write_rx_header(header_tcp , btt, spare_idx, exchange_mem);
			write_status( 	sts_dma_tcp, btt, spare_idx, exchange_mem);
			
		}

	}else{

		//if (  *( exchange_mem + (spare_idx * SPARE_BUFFER_FIELDS) + STATUS_OFFSET) != STATUS_ENQUEUED){
		//	 exchange_mem[ spare_idx * SPARE_BUFFER_FIELDS + STATUS_OFFSET] = STATUS_ERROR;
		//}else
		{
			btt 					= header_udp.read();

			write_rx_header(header_udp , btt, spare_idx, exchange_mem);
			write_status( 	sts_dma_udp, btt, spare_idx, exchange_mem);
		
		}
	}

	


}


/*
void dma_dequeue_tcp(
	stream<ap_uint<32>> &sts_dma_tcp,
	stream<ap_uint<32>> &header_tcp,
	stream<ap_uint<64>> &inflight_queue,
	volatile uint* exchange_mem
){
	//get rx_buffer pointer from inflight queue
	ap_uint<64> tmp = inflight_queue.read();
	exchange_mem 		= (void* )( tmp );
	rx_buffer * spare 	= (rx_buffer *) exchange_mem;

	if (spare->status != STATUS_ENQUEUED){
		spare->status = STATUS_ERROR;
	}else{
		int btt;
		btt 					= header_tcp.read();
		spare->rx_len 		  	= btt;
		spare->rx_tag 		  	= header_tcp.read();
		spare->rx_src 		  	= header_tcp.read();
		spare->sequence_number 	= header_tcp.read();
		int status = sts_dma_tcp.read();
		//dma sts
		// 3-0 TAG 
		// 4 INTERNAL 	ERROR usually a btt=0 trigger this
		// 5 DECODE 	ERROR address decode error timeout
		// 6 SLAVE 		ERROR DMA encountered a slave reported error
		// 7 OKAY		the associated transfer command has been completed with the OKAY response on all intermediate transfers.
		if( (status & 0x000000f0) | !(status & 0x80000000) | ( ( (status & 0x7FFFFF00) >> 8) != btt) ){
			spare->status = STATUS_ERROR;
		}else{
			spare->status = STATUS_RESERVED;
		}

	}

}
*/

/*
//check DMA status and in case recalls the exception handler
void check_DMA_status(unsigned int status, unsigned int expected_btt, unsigned int tag, unsigned int indeterminate_btt_mode_active){
	// 3-0 TAG 
	// 4 INTERNAL 	ERROR usually a btt=0 trigger this
	// 5 DECODE 	ERROR address decode error timeout
	// 6 SLAVE 		ERROR DMA encountered a slave reported error
	// 7 OKAY		the associated transfer command has been completed with the OKAY response on all intermediate transfers.	 
	if ((status & 0x000f) != tag ){
		longjmp(excp_handler, DMA_MISMATCH_ERROR);
	}
	if ( status & 0x0010){
		longjmp(excp_handler, DMA_INTERNAL_ERROR);
	}
	if ( status & 0x0020){
		longjmp(excp_handler, DMA_DECODE_ERROR);
	}
	if ( status & 0x0040){
		longjmp(excp_handler, DMA_SLAVE_ERROR);
	}
	if ( !(status & 0x0080)){
		longjmp(excp_handler, DMA_NOT_OKAY_ERROR);
	}
	if(indeterminate_btt_mode_active==1){
		//30-8 		BYTES received
		//31 		END OF PACKET indicates that S2MM received a TLAST 
		if ( !(status & 0x80000000)){
			longjmp(excp_handler, DMA_NOT_END_OF_PACKET_ERROR);
		}

		if ( ( (status & 0x7FFFFF00) >> 8) != expected_btt ){
			longjmp(excp_handler, DMA_NOT_EXPECTED_BTT_ERROR);
		}
	}
}

int dequeue_rx_buffers(void){
	//test if rx channel is non-empty and if so, read it
	unsigned int invalid, status, dma_tag, dma_tx_id, net_rx_id,count = 0;
	int spare_buffer_idx;
	unsigned int nbufs 		= Xil_In32(RX_BUFFER_COUNT_OFFSET);
	rx_buffer *rx_buf_list 	= (rx_buffer*)(RX_BUFFER_COUNT_OFFSET+4);
	if(use_tcp){
		dma_tx_id = STS_DMA2_TX;
		net_rx_id = STS_TCP_RX;
	}else{
		dma_tx_id = STS_DMA0_TX;
		net_rx_id = STS_UDP_RX;
	}

	do {
		if(timeout != 0 && count >= timeout )
			longjmp(excp_handler, DEQUEUE_BUFFER_TIMEOUT_ERROR);
		count ++;
		invalid = tngetd(dma_tx_id); 
		if(invalid == 0){
			count = 0;
			status 	= getd(dma_tx_id);
			dma_tag = status & 0xf;
			spare_buffer_idx = dma_tag_lookup[dma_tag];
			
			if (rx_buf_list[spare_buffer_idx].status != STATUS_ENQUEUED){
				longjmp(excp_handler, DEQUEUE_BUFFER_SPARE_BUFFER_STATUS_ERROR);
			}
			if( rx_buf_list[spare_buffer_idx].dma_tag != dma_tag ){
				longjmp(excp_handler, DEQUEUE_BUFFER_SPARE_BUFFER_DMATAG_MISMATCH);
			} 
			if(spare_buffer_idx >= nbufs){
				longjmp(excp_handler, DEQUEUE_BUFFER_SPARE_BUFFER_INDEX_ERROR);
			}
			rx_buf_list[spare_buffer_idx].rx_len 		  	= getd(net_rx_id);
			rx_buf_list[spare_buffer_idx].rx_tag 		  	= getd(net_rx_id);
			rx_buf_list[spare_buffer_idx].rx_src 		  	= getd(net_rx_id);
			rx_buf_list[spare_buffer_idx].sequence_number 	= getd(net_rx_id);
			rx_buf_list[spare_buffer_idx].status 			= STATUS_ERROR;
			num_rx_enqueued--;
			dma_tag_lookup[dma_tag] = -1;
			check_DMA_status(status, rx_buf_list[spare_buffer_idx].rx_len, rx_buf_list[spare_buffer_idx].dma_tag, 1);
			rx_buf_list[spare_buffer_idx].status 			= STATUS_RESERVED;
			
		}
	} while (invalid == 0);
	return 0;
}

*/
