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
#include "dma_enqueue.h"
#include <iostream>
#include <stdio.h>

using namespace hls;
using namespace std;

/*int test_even(int use_tcp, int num_spare_buffers=16){
	stream< ap_uint<32> > cmd_dma_tcp, cmd_dma_udp;
	stream< ap_uint<32> > inflight_queue;
	//create a fake memory to hold spare buffers 
	rx_buffer buffers[num_spare_buffers];

	//fill buffers with dummy data only even buffer are enqueued
	for(int i = 0; i < num_spare_buffers; i++ ){
		buffers[i].status = (i % 2) == 0 ? STATUS_IDLE : STATUS_RESERVED;
		buffers[i].addrl  = 0xdeadbeef + i ;
		buffers[i].addrh  = 0;
		cout << "Buffer "<< i << " status:" << buffers[i].status << " addr: " << hex << buffers[i].addrh << hex << buffers[i].addrl << endl;
	}
	
	dma_enqueue(	
				use_tcp,
				num_spare_buffers,
				cmd_dma_udp,
				cmd_dma_tcp,
				inflight_queue,
				buffers,
				buffers
	);

	for (int i = 0; i < num_spare_buffers; i++)
	{
		//only even buffers are enqueued
		cout << "Buffer "<< i << " status:" << buffers[i].status << endl;
		if ( (i % 2 ) == 1 ){
			if( buffers[i].status != STATUS_RESERVED ) return 1;
			if( buffers[i].addrl  != 0xdeadbeef + i  ) return 1;
			if( buffers[i].addrh  != 0 				 ) return 1;
			continue;
		}
		int dma_cmd [4];
		if (use_tcp){
			dma_cmd[0] = cmd_dma_tcp.read();
			dma_cmd[1] = cmd_dma_tcp.read();
			dma_cmd[2] = cmd_dma_tcp.read();
			dma_cmd[3] = cmd_dma_tcp.read();
		}else{
			dma_cmd[0] = cmd_dma_udp.read();
			dma_cmd[1] = cmd_dma_udp.read();
			dma_cmd[2] = cmd_dma_udp.read();
			dma_cmd[3] = cmd_dma_udp.read();
		}
		
		uint buffer_idx = inflight_queue.read();
		cout << hex << dma_cmd[0]  << hex << dma_cmd[1] << hex << dma_cmd[2] << hex << dma_cmd[3] << endl;
		cout << "inflight queue "  << buffer_idx << endl;
		if( buffers[i].status  != STATUS_ENQUEUED 			) return 1;
		if( buffers[i].addrl   != 0xdeadbeef + i  			) return 1;
		if( buffers[i].addrh   != 0 				 		) return 1;
		if( 		dma_cmd[0] != (0xC0800000 | DMA_MAX_BTT)) return 1;
		if( 		dma_cmd[1] != buffers[i].addrl		 	) return 1;
		if( 		dma_cmd[2] != buffers[i].addrh		 	) return 1;
		if( 		dma_cmd[3] != 0x2000				 	) return 1;
		if( 		buffer_idx != i						 	) return 1;

	}
	return 0;
}*/

int test_all(int use_tcp, int num_spare_buffers=4){
	stream< ap_uint<32> > cmd_dma_tcp, cmd_dma_udp;
	stream< ap_uint<32> > inflight_queue;
	//create a fake memory to hold spare buffers
	ap_uint<32>  buffers [num_spare_buffers*SPARE_BUFFER_FIELDS];
	//ap_uint<32>  buffers1[num_spare_buffers*SPARE_BUFFER_FIELDS];
	ap_uint<32>* buffers_addr = buffers;
	cout << "Buffer_in  base addr " << hex << buffers  << endl;
	//cout << "Buffer_out base addr " << hex << buffers1 << endl;

 	//fill buffers with dummy data only even buffer are enqueued
	for(int i = 0; i < num_spare_buffers; i++ ){
		buffers[i*SPARE_BUFFER_FIELDS + STATUS_OFFSET] = STATUS_IDLE ;
		buffers[i*SPARE_BUFFER_FIELDS + ADDRL_OFFSET ] = 0xdeadbeef + i ;
		buffers[i*SPARE_BUFFER_FIELDS + ADDRH_OFFSET ] = 0;
		cout << "Buffer "<< i << " status:" << buffers[i*SPARE_BUFFER_FIELDS + STATUS_OFFSET] << " addr: " << hex << buffers[i*SPARE_BUFFER_FIELDS + ADDRH_OFFSET] << hex << buffers[i*SPARE_BUFFER_FIELDS + ADDRL_OFFSET] << endl;
	}

	dma_enqueue(
				use_tcp,
				num_spare_buffers,
				cmd_dma_udp,
				cmd_dma_tcp,
				inflight_queue,
				buffers_addr,
				buffers_addr
	);

	for (ap_uint<32> i = 0; i < num_spare_buffers; i++)
	{
		//only even buffers are enqueued
		cout << "Buffer "<< i << " status:" << buffers[(i*SPARE_BUFFER_FIELDS) + STATUS_OFFSET] << endl;

		uint dma_cmd [4];
		if (use_tcp){
			dma_cmd[0] = cmd_dma_tcp.read();
			dma_cmd[1] = cmd_dma_tcp.read();
			dma_cmd[2] = cmd_dma_tcp.read();
			dma_cmd[3] = cmd_dma_tcp.read();
		}else{
			dma_cmd[0] = cmd_dma_udp.read();
			dma_cmd[1] = cmd_dma_udp.read();
			dma_cmd[2] = cmd_dma_udp.read();
			dma_cmd[3] = cmd_dma_udp.read();
		}

		ap_uint<32> buffer_idx = inflight_queue.read();
		cout << hex << dma_cmd[0]  << hex << dma_cmd[1] << hex << dma_cmd[2] << hex << dma_cmd[3] << endl;
		cout << "inflight queue "  << buffer_idx << endl;
		if( buffers[i*SPARE_BUFFER_FIELDS + STATUS_OFFSET]  != STATUS_ENQUEUED 			) return 1;
		if( buffers[i*SPARE_BUFFER_FIELDS + ADDRL_OFFSET]  != 0xdeadbeef + i  			) return 1;
		if( buffers[i*SPARE_BUFFER_FIELDS + ADDRH_OFFSET]  != 0 				 		) return 1;
		if( 		dma_cmd[0] != (0xC0800000 | DMA_MAX_BTT)) return 1;
		if( 		dma_cmd[1] != buffers[i*SPARE_BUFFER_FIELDS + ADDRL_OFFSET]		 	) return 1;
		if( 		dma_cmd[2] != buffers[i*SPARE_BUFFER_FIELDS + ADDRH_OFFSET]		 	) return 1;
		if( 		dma_cmd[3] != 0x2000				 	) return 1;
		if( 		buffer_idx != i						 	) return 1;

	}
	return 0;
}


int main(){
	int 		 nerrors 		 = 0;
	nerrors += test_all(1);
	//nerrors += test_even(1);
	//nerrors += test_even(0);
	if (nerrors == 0) {
		cout << "TB passed" << endl;
	}

	return nerrors;
}
