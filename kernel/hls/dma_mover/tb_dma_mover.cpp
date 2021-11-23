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
    //unsigned int op0_len,
    //unsigned int op1_len,
    //unsigned int op2_len,
    unsigned int len,
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
             unsigned int len = 20){
    stream<ap_uint <128> > DMA0_RX_CMD  ; stream<ap_uint <32>  > DMA0_RX_STS  ; 
	stream<ap_uint <128> > DMA1_RX_CMD  ; stream<ap_uint <32>  > DMA1_RX_STS  ;
	stream<ap_uint <128> > DMA2_RX_CMD  ; stream<ap_uint <32>  > DMA2_RX_STS  ;
	stream<ap_uint <128> > DMA1_TX_CMD  ; stream<ap_uint <32>  > DMA1_TX_STS  ;
	stream<ap_uint <512> > UDP_PKT_CMD  ; stream<ap_uint <32>  > UDP_PKT_STS  ;
    stream<ap_uint <512> > TCP_PKT_CMD  ; stream<ap_uint <32>  > TCP_PKT_STS  ;
    unsigned int segment_size       = 10;
    unsigned int max_dma_in_flight  = 20; //max_dma_in_flight can't be really tested :/
    ap_uint<64>  op0_addr           = 0xdeadbeef;
    ap_uint<64>  op1_addr           = 0x43;
    ap_uint<64>  res_addr           = 100;
    unsigned     nerrors            = 0;

    unsigned int len_tmp        ;
	
    unsigned int tmp_to_move;
    int tag;
    ap_uint <96> sts;
    ap_uint <128> cmd;
    

	//test: ensure that dma_mover segments correctly the buffers
	//create the dma ack for all segments
	len_tmp		= len;
	tag			= 0;
	//check that there's no data left to move
	while(  which_dma && len_tmp > 0 ){

		tmp_to_move = (len_tmp > segment_size ? segment_size : len_tmp);
		if ( (which_dma & USE_OP0_DMA) && len_tmp > 0){
			//if there are too many element to transfer: according to segment size
			//put segment size elements
			DMA0_RX_STS.write(create_sts(tmp_to_move , 0, tag));
		}
		if ( (which_dma & USE_OP1_DMA) && len_tmp > 0){
			DMA1_RX_STS.write(create_sts(tmp_to_move , 0, tag));
		}
		if ( (which_dma & USE_RES_DMA) && len_tmp > 0){
			DMA1_TX_STS.write(create_sts(tmp_to_move, 0, tag, 1));
		}
		if ( (which_dma & USE_RES_DMA_WITHOUT_TLAST) && len_tmp > 0){
			DMA1_TX_STS.write(create_sts(tmp_to_move, 0, tag, 0));
		}
		len_tmp -=tmp_to_move;
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
		segment_size, max_dma_in_flight,
		len,
		op0_addr    , op1_addr  , res_addr,
		which_dma);
	nerrors +=( sts != 0);
	if (nerrors){ cout << "Dma movement returned non zero!" << endl; return 1; }
	len_tmp		= len;	
	tag         = 0;
	//check that dma_mover created all the segments
	while( which_dma && len_tmp > 0) {
		tmp_to_move = (len_tmp > segment_size ? segment_size : len_tmp);

		if( (which_dma & USE_OP0_DMA) && len_tmp > 0 ){
			cmd         = DMA0_RX_CMD.read();
			nerrors     +=( cmd != create_cmd(tmp_to_move, op0_addr, tag) );
			if (nerrors){ cout << "DMA0_RX CMD not correct!" << endl; return 1; }
			op0_addr    += tmp_to_move;

		}
		if( (which_dma & USE_OP1_DMA) && len_tmp > 0 ){
			cmd         = DMA1_RX_CMD.read();
			nerrors     +=( cmd != create_cmd(tmp_to_move, op1_addr, tag) );
			if (nerrors){ cout << "DMA1_RX CMD not correct!" << endl; return 1; }
			op1_addr    += tmp_to_move;

		}
		if( (which_dma & (USE_RES_DMA | USE_RES_DMA_WITHOUT_TLAST) ) && len_tmp > 0 ){
			cmd         = DMA1_TX_CMD.read();
			nerrors     +=( cmd != create_cmd(tmp_to_move, res_addr, tag) );
			if (nerrors){ cout << "DMA1_TX CMD not correct!" << endl; return 1; }
			res_addr    += tmp_to_move;
		}
		len_tmp -= tmp_to_move;
		tag 	 = (tag + 1) & 0xf;
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

int test_tag_errors( 
	unsigned int which_dma, 
    unsigned int len = 20){
    stream<ap_uint <128> > DMA0_RX_CMD  ; stream<ap_uint <32>  > DMA0_RX_STS  ; 
	stream<ap_uint <128> > DMA1_RX_CMD  ; stream<ap_uint <32>  > DMA1_RX_STS  ;
	stream<ap_uint <128> > DMA2_RX_CMD  ; stream<ap_uint <32>  > DMA2_RX_STS  ;
	stream<ap_uint <128> > DMA1_TX_CMD  ; stream<ap_uint <32>  > DMA1_TX_STS  ;
	stream<ap_uint <512> > UDP_PKT_CMD  ; stream<ap_uint <32>  > UDP_PKT_STS  ;
    stream<ap_uint <512> > TCP_PKT_CMD  ; stream<ap_uint <32>  > TCP_PKT_STS  ;
    unsigned int segment_size       = 10;
    unsigned int max_dma_in_flight  = 20;
    ap_uint<64>  op0_addr           = 0xdeadbeef;
    ap_uint<64>  op1_addr           = 0x43;
    ap_uint<64>  res_addr           = 100;
    unsigned     nerrors            = 0;

    unsigned int len_tmp        ;
	
    unsigned int tmp_to_move;
    int tag;
    ap_uint <96> sts;
    ap_uint <128> cmd;
    

	//test: ensure that dma_mover segments correctly the buffers
	//create the dma ack for all segments
	len_tmp		= len;
	tag			= 1;
	//check that there's no data left to move
	while(  which_dma && len_tmp > 0 ){
		//wrongly supply the tag by starting from tag 1
		tmp_to_move = (len_tmp > segment_size ? segment_size : len_tmp);
		if ( (which_dma & USE_OP0_DMA) && len_tmp > 0){
			//if there are too many element to transfer: according to segment size
			//put segment size elements
			DMA0_RX_STS.write(create_sts(tmp_to_move , 	0, tag ));
		}
		if ( (which_dma & USE_OP1_DMA) && len_tmp > 0){
			DMA1_RX_STS.write(create_sts(tmp_to_move , 	0, tag  ));
		}
		if ( (which_dma & USE_RES_DMA) && len_tmp > 0){
			DMA1_TX_STS.write(create_sts(tmp_to_move, 	0, tag  , 1));
		}
		if ( (which_dma & USE_RES_DMA_WITHOUT_TLAST) && len_tmp > 0){
			DMA1_TX_STS.write(create_sts(tmp_to_move, 	0, tag  , 0));
		}
		tag = (tag + 1) & 0xf;
		len_tmp -=tmp_to_move;
	}
	//recall segmentator
	sts = dma_mover(
		DMA0_RX_CMD, DMA0_RX_STS,
		DMA1_RX_CMD, DMA1_RX_STS,
		DMA2_RX_CMD, DMA2_RX_STS,
		DMA1_TX_CMD, DMA1_TX_STS,
		UDP_PKT_CMD, UDP_PKT_STS,
		TCP_PKT_CMD, TCP_PKT_STS,
		segment_size, max_dma_in_flight,
		len,
		op0_addr    , op1_addr  , res_addr,
		which_dma);
	
	len_tmp		= len;	
	tag         = 0;
	//go further only if len > 0
	if( which_dma && len_tmp > 0) {
		tmp_to_move = (len_tmp > segment_size ? segment_size : len_tmp);

		if( (which_dma & USE_OP0_DMA) && len_tmp > 0 ){
			//check that cmd is issued 
			cmd         = DMA0_RX_CMD.read();
			nerrors     +=( cmd != create_cmd(tmp_to_move, op0_addr, tag) );
			if (nerrors){ cout << "DMA0_RX CMD not correct!" << endl; return 1; }
			op0_addr    += tmp_to_move;
			//check that dma_mover signals a TAG mismatch
			nerrors +=( sts.range(6, 0) 	!= DMA_TAG_MISMATCH_ERROR);
			if (nerrors){ cout << "Dma movement didn't return DMA_TAG_MISMATCH_ERROR!" << endl; return 1; }

		}
		if( (which_dma & USE_OP1_DMA) && len_tmp > 0 ){
			//check that cmd is issued 
			cmd         = DMA1_RX_CMD.read();
			nerrors     +=( cmd != create_cmd(tmp_to_move, op1_addr, tag) );
			if (nerrors){ cout << "DMA1_RX CMD not correct!" << endl; return 1; }
			op1_addr    += tmp_to_move;
			//check that dma_mover signals a TAG mismatch
			nerrors +=( sts.range(13, 7)	!= DMA_TAG_MISMATCH_ERROR);
			if (nerrors){ cout << "Dma movement didn't return DMA_TAG_MISMATCH_ERROR!" << endl; return 1; }

		}
		if( (which_dma & (USE_RES_DMA | USE_RES_DMA_WITHOUT_TLAST) ) && len_tmp > 0 ){
			//check that cmd is issued 
			cmd         = DMA1_TX_CMD.read();
			nerrors     +=( cmd != create_cmd(tmp_to_move, res_addr, tag) );
			if (nerrors){ cout << "DMA1_TX CMD not correct!" << endl; return 1; }
			res_addr    += tmp_to_move;
			//check that dma_mover signals a TAG mismatch
			nerrors +=( sts.range(20, 14) 	!= DMA_TAG_MISMATCH_ERROR);
			if (nerrors){ cout << "Dma movement didn't return DMA_TAG_MISMATCH_ERROR!" << endl; return 1; }
		}


		len_tmp -= tmp_to_move;
		tag 	 = (tag + 1) & 0xf;
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

int test_dma_errors( 
	unsigned int which_dma, 
    unsigned int len = 20){
    stream<ap_uint <128> > DMA0_RX_CMD  ; stream<ap_uint <32>  > DMA0_RX_STS  ; 
	stream<ap_uint <128> > DMA1_RX_CMD  ; stream<ap_uint <32>  > DMA1_RX_STS  ;
	stream<ap_uint <128> > DMA2_RX_CMD  ; stream<ap_uint <32>  > DMA2_RX_STS  ;
	stream<ap_uint <128> > DMA1_TX_CMD  ; stream<ap_uint <32>  > DMA1_TX_STS  ;
	stream<ap_uint <512> > UDP_PKT_CMD  ; stream<ap_uint <32>  > UDP_PKT_STS  ;
    stream<ap_uint <512> > TCP_PKT_CMD  ; stream<ap_uint <32>  > TCP_PKT_STS  ;
    unsigned int segment_size       = 10;
    unsigned int max_dma_in_flight  = 20;
    ap_uint<64>  op0_addr           = 0xdeadbeef;
    ap_uint<64>  op1_addr           = 0x43;
    ap_uint<64>  res_addr           = 100;
    unsigned     nerrors            = 0;

    unsigned int len_tmp        ;
	
    unsigned int tmp_to_move;
    int tag;
    ap_uint <96> sts;
    ap_uint <128> cmd;
    

	//test: ensure that dma_mover correctly notifies errors
	for( int error=1; error < 8; error = error << 1 ){
		len_tmp		= len;
		tag			= 0;
		//check that there's no data left to move
		while(  which_dma && len_tmp > 0 ){
			//wrongly supply the tag by starting from tag 1
			tmp_to_move = (len_tmp > segment_size ? segment_size : len_tmp);
			if ( (which_dma & USE_OP0_DMA) 				 && len_tmp > 0){
				//if there are too many element to transfer: according to segment size
				//put segment size elements
				DMA0_RX_STS.write(create_sts(tmp_to_move , 	error, tag ));
			}
			if ( (which_dma & USE_OP1_DMA) 				 && len_tmp > 0){
				DMA1_RX_STS.write(create_sts(tmp_to_move , 	error, tag  ));
			}
			if ( (which_dma & USE_RES_DMA) 				 && len_tmp > 0){
				DMA1_TX_STS.write(create_sts(tmp_to_move, 	error, tag  , 1));
			}
			if ( (which_dma & USE_RES_DMA_WITHOUT_TLAST) && len_tmp > 0){
				DMA1_TX_STS.write(create_sts(tmp_to_move, 	error, tag  , 0));
			}
			tag = (tag + 1) & 0xf;
			len_tmp -=tmp_to_move;
		}
		//recall segmentator
		sts = dma_mover(
			DMA0_RX_CMD, DMA0_RX_STS,
			DMA1_RX_CMD, DMA1_RX_STS,
			DMA2_RX_CMD, DMA2_RX_STS,
			DMA1_TX_CMD, DMA1_TX_STS,
			UDP_PKT_CMD, UDP_PKT_STS,
			TCP_PKT_CMD, TCP_PKT_STS,
			segment_size, max_dma_in_flight,
			len,
			op0_addr    , op1_addr  , res_addr,
			which_dma);
		
		len_tmp		= len;	
		tag         = 0;
		//go further only if len > 0
		if( which_dma && len_tmp > 0) {
			tmp_to_move = (len_tmp > segment_size ? segment_size : len_tmp);

			if( (which_dma & USE_OP0_DMA) && len_tmp > 0 ){
				//check that cmd is issued 
				cmd         = DMA0_RX_CMD.read();
				nerrors     +=( cmd != create_cmd(tmp_to_move, op0_addr, tag) );
				if (nerrors){ cout << "DMA0_RX CMD not correct!" << endl; return 1; }
				op0_addr    += tmp_to_move;
				//check that dma_mover signals a TAG mismatch
				nerrors +=( sts.range(6, 0) 	!= (DMA_NOT_OKAY_ERROR | ( error << 1 ) ));
				if (nerrors){ cout << "Dma movement didn't return error!" << endl; return 1; }

			}
			if( (which_dma & USE_OP1_DMA) && len_tmp > 0 ){
				//check that cmd is issued 
				cmd         = DMA1_RX_CMD.read();
				nerrors     +=( cmd != create_cmd(tmp_to_move, op1_addr, tag) );
				if (nerrors){ cout << "DMA1_RX CMD not correct!" << endl; return 1; }
				op1_addr    += tmp_to_move;
				//check that dma_mover signals a TAG mismatch
				nerrors +=( sts.range(13, 7)	!= (DMA_NOT_OKAY_ERROR | ( error << 1 ) ));
				if (nerrors){ cout << "Dma movement didn't return error!" << endl; return 1; }

			}
			if( (which_dma & (USE_RES_DMA | USE_RES_DMA_WITHOUT_TLAST) ) && len_tmp > 0 ){
				//check that cmd is issued 
				cmd         = DMA1_TX_CMD.read();
				nerrors     +=( cmd != create_cmd(tmp_to_move, res_addr, tag) );
				if (nerrors){ cout << "DMA1_TX CMD not correct!" << endl; return 1; }
				res_addr    += tmp_to_move;
				//check that dma_mover signals a TAG mismatch
				nerrors +=( sts.range(20, 14) 	!= (DMA_NOT_OKAY_ERROR | ( error << 1 ) ));
				if (nerrors){ cout << "Dma movement didn't return error!" << endl; return 1; }
			}


			len_tmp -= tmp_to_move;
			tag 	 = (tag + 1) & 0xf;
		}
	
		if (!DMA0_RX_CMD.empty()){ cout << "DMA0_RX CMD not empty!" << endl; return 1; }
		if (!DMA1_RX_CMD.empty()){ cout << "DMA1_RX CMD not empty!" << endl; return 1; }
		if (!DMA2_RX_CMD.empty()){ cout << "DMA2_RX CMD not empty!" << endl; return 1; }
		if (!DMA1_TX_CMD.empty()){ cout << "DMA1_TX CMD not empty!" << endl; return 1; }
		if (!DMA0_RX_STS.empty()){ cout << "DMA0_RX STS not empty!" << endl; return 1; }
		if (!DMA1_RX_STS.empty()){ cout << "DMA1_RX STS not empty!" << endl; return 1; }
		if (!DMA2_RX_STS.empty()){ cout << "DMA2_RX STS not empty!" << endl; return 1; }
		if (!DMA1_TX_STS.empty()){ cout << "DMA1_TX STS not empty!" << endl; return 1; }
	}
    return nerrors;
}

int main(){
    int nerrors  = 0;
    //multiple segments
    nerrors = 0;
    for(int i = 0; i < 10; i++){
        nerrors 	+= test_dmas(USE_OP0_DMA				, i*10 + i);
        nerrors 	+= test_dmas(USE_OP1_DMA				, i*10 + i);
        nerrors 	+= test_dmas(USE_RES_DMA_WITHOUT_TLAST	, i*10 + i);
        nerrors 	+= test_dmas(USE_RES_DMA				, i*10 + i);
    }
    cout << "Errors: " << nerrors  << " after multiple dma transfer" << endl;
    //multiple dma at the same time
	nerrors = 0;
    for(int i = 0; i < 10; i++){
        nerrors 	+= test_dmas(USE_OP0_DMA | USE_OP1_DMA | USE_RES_DMA_WITHOUT_TLAST	, i*10 + i);
        nerrors 	+= test_dmas(USE_OP0_DMA | USE_OP1_DMA | USE_RES_DMA				, i*10 + i);
        nerrors 	+= test_dmas(USE_OP0_DMA | USE_OP1_DMA 								, i*10 + i);
    }
    cout << "Errors: " << nerrors  << " after multiple dma transfer on multiple dmas " << endl;
	nerrors 	+= test_tag_errors( USE_OP0_DMA							   ,  1);
	nerrors 	+= test_tag_errors( USE_OP0_DMA | USE_OP1_DMA			   ,  1);
	nerrors 	+= test_tag_errors( USE_OP0_DMA | USE_OP1_DMA | USE_RES_DMA,  1);
    cout << "Errors: " << nerrors << " after testing for tag errors" <<  endl;
	nerrors 	+= test_dma_errors( USE_OP0_DMA							   ,  1);
	nerrors 	+= test_dma_errors( USE_OP0_DMA | USE_OP1_DMA			   ,  1);
	nerrors 	+= test_dma_errors( USE_OP0_DMA | USE_OP1_DMA | USE_RES_DMA,  1);
    cout << "Errors: " << nerrors << " after testing for dma errors" <<  endl;
    return (nerrors == 0 ? 0 : 1);
}
