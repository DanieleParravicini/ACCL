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

#include "ap_axi_sdata.h"
#include "hls_stream.h"
#include "ap_int.h"

using namespace hls;
using namespace std;

#define DATA_WIDTH 512
#define DST_START 		   0
#define DST_END			   DST_START+15
#define HEADER_COUNT_START DST_END+1
#define HEADER_COUNT_END   HEADER_COUNT_START+31
#define HEADER_TAG_START   HEADER_COUNT_END+1
#define HEADER_TAG_END	   HEADER_TAG_START+31
#define HEADER_SRC_START   HEADER_TAG_END+1
#define HEADER_SRC_END	   HEADER_SRC_START+31
#define HEADER_SEQ_START   HEADER_SRC_END+1
#define HEADER_SEQ_END	   HEADER_SEQ_START+31

void vnx_packetizer(	stream<ap_axiu<DATA_WIDTH,0,0,0> > & in,
			stream<ap_axiu<DATA_WIDTH,0,0,16> > & out,
			stream<ap_uint<DATA_WIDTH> > & cmd,
			stream<ap_uint<32> > & sts,
			unsigned int max_pktsize) {
	#pragma HLS INTERFACE axis register both port=in
	#pragma HLS INTERFACE axis register both port=out
	#pragma HLS INTERFACE axis register both port=cmd
	#pragma HLS INTERFACE axis register both port=sts
	#pragma HLS INTERFACE s_axilite port=max_pktsize
	#pragma HLS INTERFACE s_axilite port=return

	unsigned const bytes_per_word = DATA_WIDTH/8;

	//read commands from the command stream
	//cmd are in the following form
	//16 bits identifies the destination (port number)
	//32 bits number of bytes of the message
	//32 bits mpi_tag of the message
	//32 bits src of the message
	//32 bits sequence_number of the message
	ap_uint<DATA_WIDTH> cmd_data 	= cmd.read();
	unsigned int destination	    = cmd_data.range(DST_END			, DST_START			);
	ap_uint<DATA_WIDTH-16-1> header = cmd_data.range(DATA_WIDTH-1		, HEADER_COUNT_START);
	int message_bytes 		 		= cmd_data.range(HEADER_COUNT_END	, HEADER_COUNT_START);
	int message_seq					= cmd_data.range(HEADER_SEQ_END		, HEADER_SEQ_START	);
	int bytes_to_process 			= message_bytes + bytes_per_word;

	unsigned int pktsize = 0;
	int bytes_processed  = 0;

	while(bytes_processed < bytes_to_process){
	#pragma HLS PIPELINE II=1
		ap_axiu<DATA_WIDTH,0,0,16> outword;
		outword.dest = destination;
		//if this is the first word, put the count in a header
		if(bytes_processed == 0){
			outword.data.range(HEADER_SEQ_END - HEADER_COUNT_START, 0) 	= header;
		} else {
			outword.data = in.read().data;
		}
		//signal ragged tail
		int bytes_left = (bytes_to_process - bytes_processed);
		if(bytes_left < bytes_per_word){
			ap_uint<bytes_per_word> keep = 1;
			keep = keep << bytes_left;
			keep -= 1;
			outword.keep = keep;
			bytes_processed += bytes_left;
		}else{
			outword.keep = -1;
			bytes_processed += bytes_per_word;
		}
		pktsize++;
		//after every max_pktsize words, or if we run out of bytes, assert TLAST
		if((pktsize == max_pktsize) || (bytes_left <= bytes_per_word)){
			outword.last = 1;
			pktsize = 0;
		}else{
			outword.last = 0;
		}
		//write output stream
		out.write(outword);
	}
	//acknowledge that message_seq has been sent successfully
	sts.write(message_seq);
}