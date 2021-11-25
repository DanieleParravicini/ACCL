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
			unsigned int max_pktsize);

int ntransfers(int nbytes){
	int bytes_per_transfer = DATA_WIDTH/8;
	return (nbytes+bytes_per_transfer-1)/bytes_per_transfer;
}

int main(){

	stream<ap_axiu<DATA_WIDTH,0,0,0> > in;
	stream<ap_axiu<DATA_WIDTH,0,0,16> > out;
	stream<ap_axiu<DATA_WIDTH,0,0,0> > golden;
	stream<ap_uint<DATA_WIDTH> > cmd;
	stream<ap_uint<32> > sts;
	
	ap_axiu<DATA_WIDTH,0,0,0> inword;
	ap_axiu<DATA_WIDTH,0,0,16> outword;
	ap_axiu<DATA_WIDTH,0,0,0> goldenword;
	ap_uint<DATA_WIDTH> cmd_data;
	int dest 	= 3;
	int len 	= 50;
	int tag 	= 5;
	int src 	= 6;
	int seq  	= 7;
	int pkt_size = 1536/64;
	//1024B+64B transfer
	len = 50;
	cmd_data.range(DST_END			, DST_START			)= dest;
	cmd_data.range(HEADER_COUNT_END	, HEADER_COUNT_START)= len;
	cmd_data.range(HEADER_TAG_END	, HEADER_TAG_START	)= tag;
	cmd_data.range(HEADER_SRC_END	, HEADER_SRC_START	)= src;
	cmd_data.range(HEADER_SEQ_END	, HEADER_SEQ_START	)= seq;
	cmd.write(cmd_data);

	for(int i=0; i<ntransfers(len); i++){
		inword.data = i;
		inword.last = (i==(ntransfers(len)-1));
		in.write(inword);
		golden.write(inword);
	}
	
	vnx_packetizer(in, out, cmd, sts, pkt_size);
	
	//parse header
	outword = out.read();
	if(outword.dest != dest			) {cout << "dest  wrong!" << endl; return 1; };
	if(outword.last != 0			) {cout << "last  wrong!" << endl; return 1; };
	if(outword.data(31,0) 	!= len	) {cout << "len  wrong!" << endl; return 1; };
	if(outword.data(63,32) 	!= tag	) {cout << "tag  wrong!" << endl; return 1; };
	if(outword.data(95,64) 	!= src	) {cout << "src  wrong!" << endl; return 1; };
	if(outword.data(127,96) != seq	) {cout << "seq  wrong!" << endl; return 1; };
	//parse data
	for(int i=0; i<ntransfers(len); i++){
		outword = out.read();
		goldenword = golden.read();
		if(outword.data != goldenword.data) { cout << "data mismatch" << endl; return 1;} ;
		if(outword.last != goldenword.last) { cout << "last mismatch" << endl; return 1;} ;
	}
	if(sts.read() != seq)			  {cout << "returned wrong seq !" << endl; return 1; };

	
	//1536B transfer
	seq++;
	len = 1536-64;
	cmd_data.range(DST_END			, DST_START			)= dest;
	cmd_data.range(HEADER_COUNT_END	, HEADER_COUNT_START)= len;
	cmd_data.range(HEADER_TAG_END	, HEADER_TAG_START	)= tag;
	cmd_data.range(HEADER_SRC_END	, HEADER_SRC_START	)= src;
	cmd_data.range(HEADER_SEQ_END	, HEADER_SEQ_START	)= seq;
	cmd.write(cmd_data);
	
	for(int i=0; i<ntransfers(len); i++){
		inword.data = i;
		inword.last = (i==(ntransfers(len)-1));
		in.write(inword);
		golden.write(inword);
	}
	
	vnx_packetizer(in, out, cmd, sts, pkt_size);
	
	//parse header
	outword = out.read();
	if(outword.dest != dest			) {cout << "dest  wrong!" << endl; return 1; };
	if(outword.last != 0			) {cout << "last  wrong!" << endl; return 1; };
	if(outword.data(31,0) 	!= len	) {cout << "len  wrong!" << endl; return 1; };
	if(outword.data(63,32) 	!= tag	) {cout << "tag  wrong!" << endl; return 1; };
	if(outword.data(95,64) 	!= src	) {cout << "src  wrong!" << endl; return 1; };
	if(outword.data(127,96) != seq	) {cout << "seq  wrong!" << endl; return 1; };
	//parse data
	for(int i=0; i<ntransfers(len); i++){
		outword = out.read();
		goldenword = golden.read();
		if(outword.data != goldenword.data) { cout << "data mismatch" << endl; return 1;} ;
		if(outword.last != goldenword.last) { cout << "last mismatch" << endl; return 1;} ;
	}
	if(sts.read() != seq)			  {cout << "returned wrong seq !" << endl; return 1; };
	
	//10KB transfer	
	seq++;
	len = 10*1024;
	cmd_data.range(DST_END			, DST_START			)= dest;
	cmd_data.range(HEADER_COUNT_END	, HEADER_COUNT_START)= len;
	cmd_data.range(HEADER_TAG_END	, HEADER_TAG_START	)= tag;
	cmd_data.range(HEADER_SRC_END	, HEADER_SRC_START	)= src;
	cmd_data.range(HEADER_SEQ_END	, HEADER_SEQ_START	)= seq;
	cmd.write(cmd_data);

	for(int i=0; i<ntransfers(len); i++){
		inword.data = i;
		inword.last = (i==(ntransfers(len)-1));
		in.write(inword);
		golden.write(inword);
	}
	
	vnx_packetizer(in, out, cmd, sts, pkt_size);
	
	//parse header
	outword = out.read();
	if(outword.dest != dest			) {cout << "dest  wrong!" << endl; return 1; };
	if(outword.last != 0			) {cout << "last  wrong!" << endl; return 1; };
	if(outword.data(31,0) 	!= len	) {cout << "len  wrong!" << endl; return 1; };
	if(outword.data(63,32) 	!= tag	) {cout << "tag  wrong!" << endl; return 1; };
	if(outword.data(95,64) 	!= src	) {cout << "src  wrong!" << endl; return 1; };
	if(outword.data(127,96) != seq	) {cout << "seq  wrong!" << endl; return 1; };
	//parse data
	for(int i=0; i<ntransfers(len); i++){
		outword = out.read();
		goldenword = golden.read();
		if(outword.data != goldenword.data) { cout << "data mismatch" << endl; ; return 1;}
		if((i+2)*64 % 1536 == 0){
			int last = outword.last;
			if(last == 0) { cout << "last mismatch" << endl; ; return 1;}
		} else {
			if(outword.last != goldenword.last) { cout << "last mismatch" << endl; ; return 1;}
		}
	}
	if(sts.read() != seq)			  {cout << "returned wrong seq !" << endl; return 1; };

	return 0;
}