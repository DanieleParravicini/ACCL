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

void test_even(int use_tcp, int num_test_spares=10){
	stream<ap_uint<32>> cmd_dma_tcp, cmd_dma_udp;
	stream<ap_uint<64>> inflight_queue;

	void * ptr 					 = malloc(sizeof(rx_buffer)*num_test_spares+sizeof(unsigned int));
	//create a fake memory
	*((unsigned int *)ptr) 	=  num_test_spares;
	rx_buffer * buffers 	= ( (rx_buffer *) (((uint *)ptr) + 1 ) );
	cout << "Exchange memory: " << ptr << endl;
	//fill buffers with dummy data only even buffer are enqueued
	for(int i = 0; i < num_test_spares; i++ ){
		buffers[i].status = (i % 2) == 0 ? STATUS_IDLE : STATUS_RESERVED;
		buffers[i].addrl  = 0xdeadbeef + i ;
		buffers[i].addrh  = 0;
		cout << "Buffer "<< i << " status:" << buffers[i].status << " addr: " << hex << buffers[i].addrh << hex << buffers[i].addrl << endl;
	}
	
	dma_enqueue(	
				use_tcp,
				cmd_dma_udp,
				cmd_dma_tcp,
				inflight_queue,
				(uint *) ptr
	);

	for (int i = 0; i < num_test_spares; i++)
	{
		//only even buffers are enqueued
		if ( (i % 2 ) == 1 ) continue;
		cout << "Buffer "<< i << " status:" << buffers[i].status << endl;
		//printf("cmd 0x%08x%08x%08x%08x\n",cmd_dma_tcp.read(), cmd_dma_tcp.read(), cmd_dma_tcp.read(), cmd_dma_tcp.read());
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
		
		long long int buffer_addr = inflight_queue.read();
		cout << hex << dma_cmd[0]  << hex << dma_cmd[1] << hex << dma_cmd[2] << hex << dma_cmd[3] << endl;
		cout << "inflight queue " << buffer_addr << endl;
		assert( buffers[i].status == STATUS_ENQUEUED );
		assert( dma_cmd[0] == (0xC0800000 | DMA_MAX_BTT) );
		assert( dma_cmd[1] == buffers[i].addrl) ;
		assert( dma_cmd[2] == buffers[i].addrh);
		assert( dma_cmd[3] == 0x2000);

		assert( (rx_buffer *)buffer_addr == &buffers[i]);

	}
}

int main(){
	int 		 nerrors 		 = 0;
	
	test_even(1);
	test_even(0);
	
	cout << "TB passed" << endl;


	return 0;
}
