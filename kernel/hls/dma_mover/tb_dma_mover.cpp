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
#include "dma_mover.h"
#include <iostream>

using namespace hls;
using namespace std;

ap_uint<96> dma_mover(
    stream<ap_uint<128> > &DMA0_RX_CMD	, stream<ap_uint<32> > &DMA0_RX_STS, 
	stream<ap_uint<128> > &DMA1_RX_CMD	, stream<ap_uint<32> > &DMA1_RX_STS,
	stream<ap_uint<128> > &DMA2_RX_CMD	, stream<ap_uint<32> > &DMA2_RX_STS,
	stream<ap_uint<128> > &DMA1_TX_CMD	, stream<ap_uint<32> > &DMA1_TX_STS,
	stream<ap_uint<512> > &UDP_PKT_CMD	, stream<ap_uint<32> > &UDP_PKT_STS,
    stream<ap_uint<512> > &TCP_PKT_CMD	, stream<ap_uint<32> > &TCP_PKT_STS,
    unsigned int segment_size,
    unsigned int max_dma_in_flight,
    unsigned int use_tcp,
    unsigned int op0_len,
    unsigned int op1_len,
    unsigned int op2_len,
    ap_uint<64> op0_addr,
    ap_uint<64> op1_addr,
    ap_uint<64> res_addr,
    unsigned int which_dma) ;

ap_uint <32> create_sts(int btt, int error, int tag, int tlast=0){
    ap_uint<32> ret;
    if (error == 0){
        error = 0x8;
    }
    tag     = 0xf & tag;
    error   = 0xf & error;
 
    ret.range(3 ,  0) = tag;
    ret.range(7 ,  4) = error;
    ret.range(30,  8) = btt;
    ret.range(31, 31) = tlast? 1: 0;
    return ret;
}

ap_uint <128> create_cmd( unsigned int btt, ap_uint<64> addr, unsigned int tag){

  ap_uint<128> ret;
  // 31=DRR 30=EOF 29-24=DSA 23=Type 22-0=BTT 15-12=xCACHE 11-8=xUSER 7-4=RSVD 3-0=TAG
  ret.range( 31,  0) 	= 0xC0800000 | btt;  
  ret.range( 95, 32) 	= addr;
  ret.range(127, 96) 	= 	  0x2000 | tag;
  return ret;
}


int test_dmas( unsigned int which_dma, 
             unsigned int op0_len = 10, unsigned int op1_len = 5, unsigned int res_len = 20){
    stream<ap_uint <128> > DMA0_RX_CMD  ; stream<ap_uint <32>  > DMA0_RX_STS  ; 
	stream<ap_uint <128> > DMA1_RX_CMD  ; stream<ap_uint <32>  > DMA1_RX_STS  ;
	stream<ap_uint <128> > DMA2_RX_CMD  ; stream<ap_uint <32>  > DMA2_RX_STS  ;
	stream<ap_uint <128> > DMA1_TX_CMD  ; stream<ap_uint <32>  > DMA1_TX_STS  ;
	stream<ap_uint <512> > UDP_PKT_CMD  ; stream<ap_uint <32>  > UDP_PKT_STS  ;
    stream<ap_uint <512> > TCP_PKT_CMD  ; stream<ap_uint <32>  > TCP_PKT_STS  ;
    unsigned int segment_size       = 10;
    unsigned int max_dma_in_flight  = 20;
    unsigned int use_tcp            = 0;
    ap_uint<64>  op0_addr           = 0xdeadbeef;
    ap_uint<64>  op1_addr           = 0x43;
    ap_uint<64>  res_addr           = 100;
    unsigned     nerrors            = 0;

    unsigned int op0_len_tmp        ;
    unsigned int op1_len_tmp        ;
    unsigned int res_len_tmp        ;
    unsigned int tmp_to_move;
    int tag;
    ap_uint <96> sts;
    ap_uint <128> cmd;
    

	//test: ensure that dma_mover segments correctly the buffers
	//create the dma ack for all segments
	op0_len_tmp = op0_len;
	op1_len_tmp = op1_len;
	res_len_tmp = res_len;
	tag=0;
	//check that there's no data left to move
	while(  ((which_dma & USE_OP0_DMA) && op0_len_tmp > 0) ||
			((which_dma & USE_OP1_DMA) && op1_len_tmp > 0) ||
			((which_dma & (USE_RES_DMA | USE_RES_DMA_WITHOUT_TLAST) ) && res_len_tmp > 0) ){

		if ( (which_dma & USE_OP0_DMA) && op0_len_tmp > 0){
			//if there are too many element to transfer: according to segment size
			//put segment size elements
			tmp_to_move = (op0_len_tmp > segment_size ? segment_size : op0_len_tmp);
			DMA0_RX_STS.write(create_sts(tmp_to_move , 0, tag));
			op0_len_tmp -=tmp_to_move;
		}
		if ( (which_dma & USE_OP1_DMA) && op1_len_tmp > 0){
			tmp_to_move = (op1_len_tmp > segment_size ? segment_size : op1_len_tmp);
			DMA1_RX_STS.write(create_sts(tmp_to_move , 0, tag));
			op1_len_tmp -=tmp_to_move;
		}
		if ( (which_dma & USE_RES_DMA) && res_len_tmp > 0){
			tmp_to_move = (res_len_tmp > segment_size ? segment_size : res_len_tmp);
			DMA1_TX_STS.write(create_sts(tmp_to_move, 0, tag, 1));
			res_len_tmp -= tmp_to_move;
		}
		if ( (which_dma & USE_RES_DMA_WITHOUT_TLAST) && res_len_tmp > 0){
			tmp_to_move = (res_len_tmp > segment_size ? segment_size : res_len_tmp);
			DMA1_TX_STS.write(create_sts(tmp_to_move, 0, tag, 0));
			res_len_tmp -= tmp_to_move;
		}
		tag = (tag + 1) & 0xf;

	}
	//recall segmentator
	sts = dma_mover(
		DMA0_RX_CMD, DMA0_RX_STS,
		DMA1_RX_CMD, DMA1_RX_STS,
		DMA2_RX_CMD, DMA2_RX_STS,
		DMA1_TX_CMD, DMA1_TX_STS,
		UDP_PKT_CMD, UDP_PKT_STS,
		TCP_PKT_CMD, TCP_PKT_STS,
		segment_size, max_dma_in_flight, use_tcp,
		op0_len     , op1_len   , res_len,
		op0_addr    , op1_addr  , res_addr,
		which_dma);
	nerrors +=( sts != 0);
	if (nerrors){ cout << "Dma movement returned non zero!" << endl; return 1; }
	op0_len_tmp = op0_len;
	op1_len_tmp = op1_len;
	res_len_tmp = res_len;
	tag         = 0;
	//check that dma_mover created all the segments
	while(  ((which_dma & USE_OP0_DMA) && op0_len_tmp > 0) ||
			((which_dma & USE_OP1_DMA) && op1_len_tmp > 0) ||
			((which_dma & (USE_RES_DMA | USE_RES_DMA_WITHOUT_TLAST) ) && res_len_tmp > 0) ){

		if( (which_dma & USE_OP0_DMA) && op0_len_tmp > 0 ){
			tmp_to_move = (op0_len_tmp > segment_size ? segment_size : op0_len_tmp);
			cmd         = DMA0_RX_CMD.read();
			nerrors     +=( cmd != create_cmd(tmp_to_move, op0_addr, tag) );
			if (nerrors){ cout << "DMA0_RX CMD not correct!" << endl; return 1; }
			op0_len_tmp -= tmp_to_move;
			op0_addr    += tmp_to_move;

		}
		if( (which_dma & USE_OP1_DMA) && op1_len_tmp > 0 ){
			tmp_to_move = (op1_len_tmp > segment_size ? segment_size : op1_len_tmp);
			cmd         = DMA1_RX_CMD.read();
			nerrors     +=( cmd != create_cmd(tmp_to_move, op1_addr, tag) );
			if (nerrors){ cout << "DMA1_RX CMD not correct!" << endl; return 1; }
			op1_len_tmp -= tmp_to_move;
			op1_addr    += tmp_to_move;

		}
		if( (which_dma & (USE_RES_DMA | USE_RES_DMA_WITHOUT_TLAST) ) && res_len_tmp > 0 ){
			tmp_to_move = (res_len_tmp > segment_size ? segment_size : res_len_tmp);
			cmd         = DMA1_TX_CMD.read();
			nerrors     +=( cmd != create_cmd(tmp_to_move, res_addr, tag) );
			if (nerrors){ cout << "DMA1_TX CMD not correct!" << endl; return 1; }
			res_len_tmp -= tmp_to_move;
			res_addr    += tmp_to_move;
		}
		tag = (tag + 1) & 0xf;
	}
	if (!DMA0_RX_CMD.empty()){ cout << "DMA0_RX CMD not empty!" << endl; return 1; }
	if (!DMA1_RX_CMD.empty()){ cout << "DMA1_RX CMD not empty!" << endl; return 1; }
	if (!DMA2_RX_CMD.empty()){ cout << "DMA2_RX CMD not empty!" << endl; return 1; }
	if (!DMA1_TX_CMD.empty()){ cout << "DMA1_TX CMD not empty!" << endl; return 1; }
	if (!DMA0_RX_STS.empty()){ cout << "DMA0_RX STS not empty!" << endl; return 1; }
	if (!DMA1_RX_STS.empty()){ cout << "DMA1_RX STS not empty!" << endl; return 1; }
	if (!DMA2_RX_STS.empty()){ cout << "DMA2_RX STS not empty!" << endl; return 1; }
	if (!DMA1_TX_STS.empty()){ cout << "DMA1_TX STS not empty!" << endl; return 1; }

    return nerrors;
}

int main(){
    int nerrors  = 0;
    //1 dma transfer 
    nerrors 	+= test_dmas(USE_OP0_DMA, 10, 0, 0);
    nerrors 	+= test_dmas(USE_OP1_DMA,  0, 10, 0);
    nerrors 	+= test_dmas(USE_RES_DMA_WITHOUT_TLAST, 0, 0, 10);
    nerrors 	+= test_dmas(USE_RES_DMA, 0, 0, 10);
    cout << "Errors: " << nerrors  << " single dma transfer" << endl;
    //erroneous size
    nerrors 	 = test_dmas(USE_OP0_DMA, 10, 10, 30);
    nerrors 	+= test_dmas(USE_OP1_DMA,  2, 10, 40);
    nerrors 	+= test_dmas(USE_RES_DMA_WITHOUT_TLAST, 3, 40, 10);
    nerrors 	+= test_dmas(USE_RES_DMA, 1, 50, 10);
    cout << "Errors: " << nerrors  << " op len but not right dma size" << endl;
    //multiple segments
    nerrors = 0;
    for(int i = 0; i < 10; i++){
        nerrors 	+= test_dmas(USE_OP0_DMA, i*10, 0, 0);
        nerrors 	+= test_dmas(USE_OP1_DMA,  0, i*10, 0);
        nerrors 	+= test_dmas(USE_RES_DMA_WITHOUT_TLAST, 0, 0, i*10);
        nerrors 	+= test_dmas(USE_RES_DMA, 0, 0, i*10);
    }
    cout << "Errors: " << nerrors  << " multiple dma transfer" << endl;
    //multiple dma at the same time
    nerrors 	 = test_dmas(USE_OP0_DMA | USE_OP1_DMA | USE_RES_DMA_WITHOUT_TLAST, 100, 1, 50);
    cout << "Errors: " << nerrors  << " multiple dmas" << endl;
    nerrors 	 = test_dmas(USE_OP0_DMA | USE_OP1_DMA , 100, 1, 50);
    cout << "Errors: " << nerrors  << " multiple dmas" << endl;
    cout << "Errors: " << nerrors << endl;
    return (nerrors == 0 ? 0 : 1);
}
