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

#include "dma_mover.h"

using namespace hls;
using namespace std;

void dma_mover(
    stream<ap_uint<104> > &DMA0_RX_CMD	, stream<ap_uint<32> > &DMA0_RX_STS, 
	stream<ap_uint<104> > &DMA1_RX_CMD	, stream<ap_uint<32> > &DMA1_RX_STS,
	stream<ap_uint<104> > &DMA1_TX_CMD	, stream<ap_uint<32> > &DMA1_TX_STS,
	stream<ap_uint<512> > &UDP_PKT_CMD	, stream<ap_uint<32> > &UDP_PKT_STS,
    stream<ap_uint<512> > &TCP_PKT_CMD	, stream<ap_uint<32> > &TCP_PKT_STS,
    unsigned int segment_size,
    unsigned int max_dma_in_flight,
    unsigned int * exchange_mem,
    stream< ap_uint<PKT_SIZE> >  & pkt_stream,
	stream< ap_uint<32> 	  >  & return_stream);

ap_uint<32> dma_mover_unpacked(
    stream<ap_uint<104> > &DMA0_RX_CMD	, stream<ap_uint<32> > &DMA0_RX_STS, 
	stream<ap_uint<104> > &DMA1_RX_CMD	, stream<ap_uint<32> > &DMA1_RX_STS,
	stream<ap_uint<104> > &DMA1_TX_CMD	, stream<ap_uint<32> > &DMA1_TX_STS,
	stream<ap_uint<512> > &UDP_PKT_CMD	, stream<ap_uint<32> > &UDP_PKT_STS,
    stream<ap_uint<512> > &TCP_PKT_CMD	, stream<ap_uint<32> > &TCP_PKT_STS,
    unsigned int segment_size,
    unsigned int max_dma_in_flight,
    unsigned int len,
    ap_uint<64> op0_addr,
    ap_uint<64> op1_addr,
    ap_uint<64> res_addr,
    unsigned int dst_rank,
    unsigned int mpi_tag,
    unsigned int * exchange_mem,
    unsigned int which_dma) {

		ap_uint< PKT_SIZE> pkt;
		pkt.range(PKT_LEN_END		  , PKT_LEN_START			) = len;
		pkt.range(PKT_DST_RANK_END	  , PKT_DST_RANK_START		) =	dst_rank;	
		pkt.range(PKT_MPI_TAG_END	  , PKT_MPI_TAG_START		) =	mpi_tag;
		pkt.range(PKT_OP0_ADDR_END	  , PKT_OP0_ADDR_START		) =	op0_addr;	
		pkt.range(PKT_OP1_ADDR_END	  , PKT_OP1_ADDR_START		) =	op1_addr;	
		pkt.range(PKT_RES_ADDR_END	  , PKT_RES_ADDR_START		) =	res_addr;	
		pkt.range(PKT_WHICH_DMA_END	  , PKT_WHICH_DMA_START		) =	which_dma;	
		pkt.range(PKT_COMM_OFFSET_END , PKT_COMM_OFFSET_START	) =	0;	
		stream < ap_uint< PKT_SIZE> > pkt_stream; 
		stream < ap_uint<32> > 	      return_stream;
		pkt_stream.write(pkt);
		dma_mover(
			DMA0_RX_CMD,DMA0_RX_STS,
			DMA1_RX_CMD,DMA1_RX_STS,
			DMA1_TX_CMD,DMA1_TX_STS,
			UDP_PKT_CMD,UDP_PKT_STS,
			TCP_PKT_CMD,TCP_PKT_STS,
			segment_size,
			max_dma_in_flight,
			exchange_mem,
			pkt_stream, return_stream
		);

		return return_stream.read();
	}



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

ap_uint <104> create_dma_cmd( unsigned int btt, ap_uint<64> addr, unsigned int tag, unsigned expected_tlast=1){

  ap_uint<104> ret;
  
  ret.range( 31,  0) 	= 0xC0800000 | btt;  // 31=DRR 30=EOF 29-24=DSA 23=Type 22-0=BTT 15-12=xCACHE 11-8=xUSER 7-4=RSVD 3-0=TAG
  ret.range( 30, 30)	= expected_tlast;
  ret.range( 95, 32) 	= addr;
  ret.range(103, 96) 	= tag;
  return ret;
}

ap_uint <512> create_pkt_cmd( unsigned int dst, unsigned int btt, unsigned int mpi_tag,
							  unsigned int src_rank, unsigned int sequence_number){

  ap_uint<512> ret = 0;
  ret.range( 31,  0) 	= dst;  
  ret.range( 63, 32) 	= btt;
  ret.range( 95, 64) 	= mpi_tag;
  ret.range(127, 96) 	= src_rank;
  ret.range(159, 128) 	= sequence_number;
  return ret;
}

int test_pkts( unsigned int which_dma, 
             unsigned int len = 20){
    stream<ap_uint <104> > DMA0_RX_CMD  ; stream<ap_uint <32>  > DMA0_RX_STS  ; 
	stream<ap_uint <104> > DMA1_RX_CMD  ; stream<ap_uint <32>  > DMA1_RX_STS  ;
	stream<ap_uint <104> > DMA1_TX_CMD  ; stream<ap_uint <32>  > DMA1_TX_STS  ;
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
    unsigned int mpi_tag 	= 0xffff;
    //create dummy communicator with 4 ranks
	unsigned int n_ranks = 4;
	unsigned int local_rank 		= 3;
	unsigned int base_ip 			= 0xbeef;
	unsigned int base_port			= 0xdead;
	unsigned int base_session 		= 0xcafe;
	unsigned int base_inbound_seq   = 0x0042;
	unsigned int base_outbound_seq  = 42	;
	void * ptr = malloc( sizeof(communicator) -4 + sizeof(comm_rank)* n_ranks);
	communicator* comm 	= ( communicator *) ptr;
	comm->size 			= n_ranks;
	comm->local_rank 	= local_rank;
	comm_rank * ranks 	= (comm_rank *) (ptr + 8) ;
	for(int i =0; i < n_ranks; i++){
		ranks[i].ip 			= base_ip 			+ i;
		ranks[i].port			= base_port			+ i;
		ranks[i].session 		= base_session 		+ i;
		ranks[i].inbound_seq  = base_inbound_seq 	+ i;
		ranks[i].outbound_seq = base_outbound_seq + i;
	}

    unsigned int segment_number;
    ap_uint <96> sts;
    ap_uint <512> pkt_cmd;
    

	//test: ensure that dma_mover segments correctly the buffers for the pkts and
	//send them the correct header
	for( unsigned int dst_rank = 0; dst_rank < n_ranks; dst_rank++){
			
		len_tmp		= len;
		segment_number = base_outbound_seq + dst_rank + 1;
		//check that there's no data left to move
		while(  which_dma && len_tmp > 0 ){

			tmp_to_move = (len_tmp > segment_size ? segment_size : len_tmp);
			if ( (which_dma & USE_PACKETIZER_TCP) && len_tmp > 0){
				//if there are too many element to transfer: according to segment size
				//put segment size elements
				TCP_PKT_STS.write( segment_number);
			}
			if ( (which_dma & USE_PACKETIZER_UDP) && len_tmp > 0){
				UDP_PKT_STS.write( segment_number);
			}

			len_tmp -=tmp_to_move;
			segment_number +=1;
		}
		//recall segmentator
		sts = dma_mover_unpacked(
			DMA0_RX_CMD, DMA0_RX_STS,
			DMA1_RX_CMD, DMA1_RX_STS,
			DMA1_TX_CMD, DMA1_TX_STS,
			UDP_PKT_CMD, UDP_PKT_STS,
			TCP_PKT_CMD, TCP_PKT_STS,
			segment_size, max_dma_in_flight,
			len,
			op0_addr    , op1_addr  , res_addr,
			dst_rank	, mpi_tag 	, ( (unsigned int *) ptr)	,
			which_dma);
		nerrors +=( sts != 0);
		if (nerrors){ cout << "Dma movement returned non zero! (" << sts << ")" << endl; return 1; }
		len_tmp			= len;	
		segment_number  = base_outbound_seq + dst_rank + 1;
		//check that dma_mover created all the segments
		while( which_dma && len_tmp > 0) {
			tmp_to_move = (len_tmp > segment_size ? segment_size : len_tmp);
			if ( (which_dma & USE_PACKETIZER_TCP) && len_tmp > 0){
				//if there are too many element to transfer: according to segment size
				//put segment size elements
				pkt_cmd = TCP_PKT_CMD.read();
				ap_uint<512> expected_pkt =  create_pkt_cmd(base_session + dst_rank , tmp_to_move, mpi_tag, local_rank,  segment_number );
				nerrors     +=( pkt_cmd != expected_pkt);
				if (nerrors){ cout << "TCP PKT CMD not correct! Got: " << pkt_cmd << " Expected: " << expected_pkt <<  endl; return 1; }
			}
			if ( (which_dma & USE_PACKETIZER_UDP) && len_tmp > 0){
				pkt_cmd = UDP_PKT_CMD.read();
				ap_uint<512> expected_pkt =  create_pkt_cmd(base_port	   + dst_rank , tmp_to_move, mpi_tag, local_rank, segment_number );
				nerrors     +=( pkt_cmd != expected_pkt);
				if (nerrors){ cout << "UDP PKT CMD not correct! Got: " << pkt_cmd << " Expected: " << expected_pkt <<  endl; return 1; }
			}

			len_tmp -= tmp_to_move;
			segment_number +=1;
		}

		for(int i=0; i < n_ranks ; i++){
			if( ranks[i].ip 			!= base_ip 			 + i) { cout << "base_ip of rank " << i << "has been modified!" << endl ; return 1;}
			if( ranks[i].port			!= base_port		 + i) { cout << "base_port of rank " << i << "has been modified!" << endl ; return 1;}
			if( ranks[i].session 		!= base_session 	 + i) { cout << "base_session of rank " << i << "has been modified!" << endl ; return 1;}
			if( ranks[i].inbound_seq  	!= base_inbound_seq  + i) { cout << "base_inbound_seq of rank " << i << "has been modified!" << endl ; return 1;}
			
			if(i <= dst_rank){
				if( ranks[i].outbound_seq  	!= base_outbound_seq + 1 + i + (len + segment_size -1 ) / segment_size ) { cout << "outbound_seq " << i << " has not been updated! obtained: "  << ranks[i].outbound_seq << " expected: " << base_outbound_seq + i + 1+  (len + segment_size -1 ) / segment_size << endl; return 1;}
			}else{
				if( ranks[i].outbound_seq 	!= base_outbound_seq + i) { cout << "base_outbound_seq of rank " << i << " has been modified!" << endl; return 1;}
			}
		}

	}
	if (!DMA0_RX_CMD.empty()){ cout << "DMA0_RX CMD not empty!" << endl; return 1; }
	if (!DMA1_RX_CMD.empty()){ cout << "DMA1_RX CMD not empty!" << endl; return 1; }
	if (!DMA1_TX_CMD.empty()){ cout << "DMA1_TX CMD not empty!" << endl; return 1; }
	if (!DMA0_RX_STS.empty()){ cout << "DMA0_RX STS not empty!" << endl; return 1; }
	if (!DMA1_RX_STS.empty()){ cout << "DMA1_RX STS not empty!" << endl; return 1; }
	if (!DMA1_TX_STS.empty()){ cout << "DMA1_TX STS not empty!" << endl; return 1; }

    return nerrors;
}

int test_pkts_error( unsigned int which_dma, 
             unsigned int len = 20){
    stream<ap_uint <104> > DMA0_RX_CMD  ; stream<ap_uint <32>  > DMA0_RX_STS  ; 
	stream<ap_uint <104> > DMA1_RX_CMD  ; stream<ap_uint <32>  > DMA1_RX_STS  ;
	stream<ap_uint <104> > DMA1_TX_CMD  ; stream<ap_uint <32>  > DMA1_TX_STS  ;
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
    unsigned int mpi_tag 	= 0xffff;
    //create dummy communicator with 4 ranks
	unsigned int n_ranks = 4;
	unsigned int local_rank 		= 3;
	unsigned int base_ip 			= 0xbeef;
	unsigned int base_port			= 0xdead;
	unsigned int base_session 		= 0xcafe;
	unsigned int base_inbound_seq   = 0x0042;
	unsigned int base_outbound_seq  = 42	;
	void * ptr = malloc( sizeof(communicator) -4 + sizeof(comm_rank)* n_ranks);
	communicator* comm 	= ( communicator *) ptr;
	comm->size 			= n_ranks;
	comm->local_rank 	= local_rank;
	comm_rank * ranks 	= (comm_rank *) (ptr + 8) ;
	for(int i =0; i < n_ranks; i++){
		ranks[i].ip 			= base_ip 			+ i;
		ranks[i].port			= base_port			+ i;
		ranks[i].session 		= base_session 		+ i;
		ranks[i].inbound_seq  = base_inbound_seq 	+ i;
		ranks[i].outbound_seq = base_outbound_seq + i;
	}

    unsigned int segment_number;
    ap_uint <96> sts;
    ap_uint <512> pkt_cmd;
    

	//test: ensure that dma_mover segments correctly the buffers for the pkts and
	//send them the correct header and recognizes an error condition on the last segment
	for( unsigned int dst_rank = 0; dst_rank < n_ranks; dst_rank++){
			
		len_tmp		= len;
		segment_number = base_outbound_seq + dst_rank + 1;
		//check that there's no data left to move
		while(  which_dma && len_tmp > 0 ){

			tmp_to_move = (len_tmp > segment_size ? segment_size : len_tmp);
			if( len_tmp <= segment_size ){
				//increment an additional time the segment number
				segment_number +=1;
			}

			if ( (which_dma & USE_PACKETIZER_TCP) && len_tmp > 0){
				//if there are too many element to transfer: according to segment size
				//put segment size elements
				TCP_PKT_STS.write( segment_number);
			}
			if ( (which_dma & USE_PACKETIZER_UDP) && len_tmp > 0){
				UDP_PKT_STS.write( segment_number);
			}

			len_tmp -=tmp_to_move;
			segment_number +=1;
		}
		//recall segmentator
		sts = dma_mover_unpacked(
			DMA0_RX_CMD, DMA0_RX_STS,
			DMA1_RX_CMD, DMA1_RX_STS,
			DMA1_TX_CMD, DMA1_TX_STS,
			UDP_PKT_CMD, UDP_PKT_STS,
			TCP_PKT_CMD, TCP_PKT_STS,
			segment_size, max_dma_in_flight,
			len,
			op0_addr    , op1_addr  , res_addr,
			dst_rank	, mpi_tag 	, ( (unsigned int *) ptr)	,
			which_dma);
		
		
		if (sts.range(DMA_ERR_BITS*3-1, DMA_ERR_BITS*2) != PACK_SEQ_NUMBER_ERROR){ cout << "Dma mover didn't return a PACK_SEQ_NUMBER_ERROR error! (" << sts << ")" << endl; return 1; }
		len_tmp			= len;	
		segment_number  = base_outbound_seq + dst_rank + 1;
		//check that dma_mover created all the segments
		while( which_dma && len_tmp > 0) {
			tmp_to_move = (len_tmp > segment_size ? segment_size : len_tmp);
			//all the pkt cmds are generated
			if ( (which_dma & USE_PACKETIZER_TCP) && len_tmp > 0){
				//if there are too many element to transfer: according to segment size
				//put segment size elements
				pkt_cmd = TCP_PKT_CMD.read();
				ap_uint<512> expected_pkt =  create_pkt_cmd(base_session + dst_rank , tmp_to_move, mpi_tag, local_rank,  segment_number );
				nerrors     +=( pkt_cmd != expected_pkt);
				if (nerrors){ cout << "TCP PKT CMD not correct! Got: " << pkt_cmd << " Expected: " << expected_pkt <<  endl; return 1; }
			}
			if ( (which_dma & USE_PACKETIZER_UDP) && len_tmp > 0){
				pkt_cmd = UDP_PKT_CMD.read();
				ap_uint<512> expected_pkt =  create_pkt_cmd(base_port	   + dst_rank , tmp_to_move, mpi_tag, local_rank, segment_number );
				nerrors     +=( pkt_cmd != expected_pkt);
				if (nerrors){ cout << "UDP PKT CMD not correct! Got: " << pkt_cmd << " Expected: " << expected_pkt <<  endl; return 1; }
			}			
			len_tmp -= tmp_to_move;
			segment_number +=1;
		}
		//sequence number are updated as in the normal case but 1 message will be missing
		for(int i=0; i < n_ranks ; i++){
			if( ranks[i].ip 			!= base_ip 			 + i) { cout << "base_ip of rank " << i << "has been modified!" << endl ; return 1;}
			if( ranks[i].port			!= base_port		 + i) { cout << "base_port of rank " << i << "has been modified!" << endl ; return 1;}
			if( ranks[i].session 		!= base_session 	 + i) { cout << "base_session of rank " << i << "has been modified!" << endl ; return 1;}
			if( ranks[i].inbound_seq  	!= base_inbound_seq  + i) { cout << "base_inbound_seq of rank " << i << "has been modified!" << endl ; return 1;}
			
			if(i <= dst_rank){
				if( ranks[i].outbound_seq  	!= base_outbound_seq + i + (len + segment_size -1 ) / segment_size ) { cout << "outbound_seq " << i << " has not been updated! obtained: "  << ranks[i].outbound_seq << " expected: " << base_outbound_seq + i + 1+  (len + segment_size -1 ) / segment_size << endl; return 1;}
			}else{
				if( ranks[i].outbound_seq 	!= base_outbound_seq + i) { cout << "base_outbound_seq of rank " << i << " has been modified!" << endl; return 1;}
			}
		}

	}
	if (!DMA0_RX_CMD.empty()){ cout << "DMA0_RX CMD not empty!" << endl; return 1; }
	if (!DMA1_RX_CMD.empty()){ cout << "DMA1_RX CMD not empty!" << endl; return 1; }
	if (!DMA1_TX_CMD.empty()){ cout << "DMA1_TX CMD not empty!" << endl; return 1; }
	if (!DMA0_RX_STS.empty()){ cout << "DMA0_RX STS not empty!" << endl; return 1; }
	if (!DMA1_RX_STS.empty()){ cout << "DMA1_RX STS not empty!" << endl; return 1; }
	if (!DMA1_TX_STS.empty()){ cout << "DMA1_TX STS not empty!" << endl; return 1; }

    return nerrors;
}


int test_dmas( unsigned int which_dma, 
             unsigned int len = 20){
    stream<ap_uint <104> > DMA0_RX_CMD  ; stream<ap_uint <32>  > DMA0_RX_STS  ; 
	stream<ap_uint <104> > DMA1_RX_CMD  ; stream<ap_uint <32>  > DMA1_RX_STS  ;
	stream<ap_uint <104> > DMA1_TX_CMD  ; stream<ap_uint <32>  > DMA1_TX_STS  ;
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
	unsigned int dst_rank	= 0;
    unsigned int mpi_tag 	= 0;
    unsigned int comm		[10];
    unsigned int dma_tag;
    ap_uint <96> sts;
    ap_uint <104> cmd;
    

	//test: ensure that dma_mover segments correctly the buffers
	//create the dma ack for all segments
	len_tmp		= len;
	dma_tag		= 0;
	//check that there's no data left to move
	while(  which_dma && len_tmp > 0 ){

		tmp_to_move = (len_tmp > segment_size ? segment_size : len_tmp);
		if ( (which_dma & USE_OP0_DMA) && len_tmp > 0){
			//if there are too many element to transfer: according to segment size
			//put segment size elements
			DMA0_RX_STS.write(create_sts(tmp_to_move , 0, dma_tag));
		}
		if ( (which_dma & USE_OP1_DMA) && len_tmp > 0){
			DMA1_RX_STS.write(create_sts(tmp_to_move , 0, dma_tag));
		}
		if ( (which_dma & USE_RES_DMA) && len_tmp > 0){
			DMA1_TX_STS.write(create_sts(tmp_to_move, 0, dma_tag, 1));
		}
		if ( (which_dma & USE_RES_DMA_WITHOUT_TLAST) && len_tmp > 0){
			DMA1_TX_STS.write(create_sts(tmp_to_move, 0, dma_tag, 0));
		}
		len_tmp -=tmp_to_move;
		dma_tag = (dma_tag + 1) & 0xf;

	}
	//recall segmentator
	sts = dma_mover_unpacked(
		DMA0_RX_CMD, DMA0_RX_STS,
		DMA1_RX_CMD, DMA1_RX_STS,
		DMA1_TX_CMD, DMA1_TX_STS,
		UDP_PKT_CMD, UDP_PKT_STS,
		TCP_PKT_CMD, TCP_PKT_STS,
		segment_size, max_dma_in_flight,
		len,
		op0_addr    , op1_addr  , res_addr,
		dst_rank	, mpi_tag 	, comm	,
		which_dma);
	nerrors +=( sts != 0);
	if (nerrors){ cout << "Dma movement returned non zero!" << endl; return 1; }
	len_tmp		= len;	
	dma_tag     = 0;
	//check that dma_mover created all the segments
	while( which_dma && len_tmp > 0) {
		tmp_to_move = (len_tmp > segment_size ? segment_size : len_tmp);

		if( (which_dma & USE_OP0_DMA) && len_tmp > 0 ){
			cmd         = DMA0_RX_CMD.read();
			nerrors     +=( cmd != create_dma_cmd(tmp_to_move, op0_addr, dma_tag) );
			if (nerrors){ cout << "DMA0_RX CMD not correct!" << endl; return 1; }
			op0_addr    += tmp_to_move;

		}
		if( (which_dma & USE_OP1_DMA) && len_tmp > 0 ){
			cmd         = DMA1_RX_CMD.read();
			nerrors     +=( cmd != create_dma_cmd(tmp_to_move, op1_addr, dma_tag) );
			if (nerrors){ cout << "DMA1_RX CMD not correct!" << endl; return 1; }
			op1_addr    += tmp_to_move;

		}
		if( (which_dma & (USE_RES_DMA | USE_RES_DMA_WITHOUT_TLAST) ) && len_tmp > 0 ){
			cmd         = DMA1_TX_CMD.read();
			nerrors     +=( cmd != create_dma_cmd(tmp_to_move, res_addr, dma_tag, ( (which_dma & USE_RES_DMA) ? 1 : 0) ) );
			if (nerrors){ cout << "DMA1_TX CMD not correct!" << endl; return 1; }
			res_addr    += tmp_to_move;
		}
		len_tmp -= tmp_to_move;
		dma_tag  = (dma_tag + 1) & 0xf;
	}
	if (!DMA0_RX_CMD.empty()){ cout << "DMA0_RX CMD not empty!" << endl; return 1; }
	if (!DMA1_RX_CMD.empty()){ cout << "DMA1_RX CMD not empty!" << endl; return 1; }
	if (!DMA1_TX_CMD.empty()){ cout << "DMA1_TX CMD not empty!" << endl; return 1; }
	if (!DMA0_RX_STS.empty()){ cout << "DMA0_RX STS not empty!" << endl; return 1; }
	if (!DMA1_RX_STS.empty()){ cout << "DMA1_RX STS not empty!" << endl; return 1; }
	if (!DMA1_TX_STS.empty()){ cout << "DMA1_TX STS not empty!" << endl; return 1; }

    return nerrors;
}

int test_dma_tag_errors( 
	unsigned int which_dma, 
    unsigned int len = 20){
    stream<ap_uint <104> > DMA0_RX_CMD  ; stream<ap_uint <32>  > DMA0_RX_STS  ; 
	stream<ap_uint <104> > DMA1_RX_CMD  ; stream<ap_uint <32>  > DMA1_RX_STS  ;
	stream<ap_uint <104> > DMA1_TX_CMD  ; stream<ap_uint <32>  > DMA1_TX_STS  ;
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
	unsigned int dst_rank	= 0;
    unsigned int mpi_tag 	= 0;
    unsigned int comm		[10];
    unsigned int dma_tag;
    ap_uint <96> sts;
    ap_uint <104> cmd;
    

	//test: ensure that dma_mover segments correctly the buffers
	//create the dma ack for all segments
	len_tmp		= len;
	dma_tag		= 1;
	//check that there's no data left to move
	while(  which_dma && len_tmp > 0 ){
		//wrongly supply the dma_tag by starting from 1
		tmp_to_move = (len_tmp > segment_size ? segment_size : len_tmp);
		if ( (which_dma & USE_OP0_DMA) && len_tmp > 0){
			//if there are too many element to transfer: according to segment size
			//put segment size elements
			DMA0_RX_STS.write(create_sts(tmp_to_move , 	0, dma_tag ));
		}
		if ( (which_dma & USE_OP1_DMA) && len_tmp > 0){
			DMA1_RX_STS.write(create_sts(tmp_to_move , 	0, dma_tag  ));
		}
		if ( (which_dma & USE_RES_DMA) && len_tmp > 0){
			DMA1_TX_STS.write(create_sts(tmp_to_move, 	0, dma_tag  , 1));
		}
		if ( (which_dma & USE_RES_DMA_WITHOUT_TLAST) && len_tmp > 0){
			DMA1_TX_STS.write(create_sts(tmp_to_move, 	0, dma_tag  , 0));
		}
		dma_tag  = (dma_tag + 1) & 0xf;
		len_tmp -=tmp_to_move;
	}
	//recall segmentator
	sts = dma_mover_unpacked(
		DMA0_RX_CMD, DMA0_RX_STS,
		DMA1_RX_CMD, DMA1_RX_STS,
		DMA1_TX_CMD, DMA1_TX_STS,
		UDP_PKT_CMD, UDP_PKT_STS,
		TCP_PKT_CMD, TCP_PKT_STS,
		segment_size, max_dma_in_flight,
		len,
		op0_addr    , op1_addr  , res_addr,
		dst_rank	, mpi_tag 	,  comm	,
		which_dma);
	
	len_tmp		= len;	
	dma_tag     = 0;
	//go further only if len > 0
	if( which_dma && len_tmp > 0) {
		tmp_to_move = (len_tmp > segment_size ? segment_size : len_tmp);

		if( (which_dma & USE_OP0_DMA) && len_tmp > 0 ){
			//check that cmd is issued 
			cmd         = DMA0_RX_CMD.read();
			nerrors     +=( cmd != create_dma_cmd(tmp_to_move, op0_addr, dma_tag) );
			if (nerrors){ cout << "DMA0_RX CMD not correct!" << endl; return 1; }
			op0_addr    += tmp_to_move;
			//check that dma_mover signals a dma_tag mismatch
			nerrors +=( sts.range(1*DMA_ERR_BITS-1, 0*DMA_ERR_BITS) 	!= DMA_TAG_MISMATCH_ERROR);
			if (nerrors){ cout << "Dma movement didn't return DMA_TAG_MISMATCH_ERROR!" << endl; return 1; }

		}
		if( (which_dma & USE_OP1_DMA) && len_tmp > 0 ){
			//check that cmd is issued 
			cmd         = DMA1_RX_CMD.read();
			nerrors     +=( cmd != create_dma_cmd(tmp_to_move, op1_addr, dma_tag) );
			if (nerrors){ cout << "DMA1_RX CMD not correct!" << endl; return 1; }
			op1_addr    += tmp_to_move;
			//check that dma_mover signals a dma_tag mismatch
			nerrors +=( sts.range(2*DMA_ERR_BITS-1, 1*DMA_ERR_BITS)	!= DMA_TAG_MISMATCH_ERROR);
			if (nerrors){ cout << "Dma movement didn't return DMA_TAG_MISMATCH_ERROR!" << endl; return 1; }

		}
		if( (which_dma & (USE_RES_DMA | USE_RES_DMA_WITHOUT_TLAST) ) && len_tmp > 0 ){
			//check that cmd is issued 
			cmd         = DMA1_TX_CMD.read();
			nerrors     +=( cmd != create_dma_cmd(tmp_to_move, res_addr, dma_tag, ( (which_dma & USE_RES_DMA) ? 1 : 0)) );
			if (nerrors){ cout << "DMA1_TX CMD not correct!" << endl; return 1; }
			res_addr    += tmp_to_move;
			//check that dma_mover signals a dma_tag mismatch
			nerrors +=( sts.range(3*DMA_ERR_BITS-1, 2*DMA_ERR_BITS) 	!= DMA_TAG_MISMATCH_ERROR);
			if (nerrors){ cout << "Dma movement didn't return DMA_TAG_MISMATCH_ERROR!" << endl; return 1; }
		}


		len_tmp -= tmp_to_move;
		dma_tag  = (dma_tag + 1) & 0xf;
	}
	if (!DMA0_RX_CMD.empty()){ cout << "DMA0_RX CMD not empty!" << endl; return 1; }
	if (!DMA1_RX_CMD.empty()){ cout << "DMA1_RX CMD not empty!" << endl; return 1; }
	if (!DMA1_TX_CMD.empty()){ cout << "DMA1_TX CMD not empty!" << endl; return 1; }
	if (!DMA0_RX_STS.empty()){ cout << "DMA0_RX STS not empty!" << endl; return 1; }
	if (!DMA1_RX_STS.empty()){ cout << "DMA1_RX STS not empty!" << endl; return 1; }
	if (!DMA1_TX_STS.empty()){ cout << "DMA1_TX STS not empty!" << endl; return 1; }

    return nerrors;
}

int test_dma_errors( 
	unsigned int which_dma, 
    unsigned int len = 20){
    stream<ap_uint <104> > DMA0_RX_CMD  ; stream<ap_uint <32>  > DMA0_RX_STS  ; 
	stream<ap_uint <104> > DMA1_RX_CMD  ; stream<ap_uint <32>  > DMA1_RX_STS  ;
	stream<ap_uint <104> > DMA1_TX_CMD  ; stream<ap_uint <32>  > DMA1_TX_STS  ;
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
	unsigned int dst_rank	= 0;
    unsigned int mpi_tag 	= 0;
    unsigned int comm		[10];
    unsigned int dma_tag;
    ap_uint <96> sts;
    ap_uint <104> cmd;
    

	//test: ensure that dma_mover correctly notifies errors
	for( int error=1; error < 8; error = error << 1 ){
		len_tmp		= len;
		dma_tag		= 0;
		//check that there's no data left to move
		while(  which_dma && len_tmp > 0 ){
			//wrongly supply the dma_tag by starting from 1
			tmp_to_move = (len_tmp > segment_size ? segment_size : len_tmp);
			if ( (which_dma & USE_OP0_DMA) 				 && len_tmp > 0){
				//if there are too many element to transfer: according to segment size
				//put segment size elements
				DMA0_RX_STS.write(create_sts(tmp_to_move , 	error, dma_tag ));
			}
			if ( (which_dma & USE_OP1_DMA) 				 && len_tmp > 0){
				DMA1_RX_STS.write(create_sts(tmp_to_move , 	error, dma_tag  ));
			}
			if ( (which_dma & USE_RES_DMA) 				 && len_tmp > 0){
				DMA1_TX_STS.write(create_sts(tmp_to_move, 	error, dma_tag  , 1));
			}
			if ( (which_dma & USE_RES_DMA_WITHOUT_TLAST) && len_tmp > 0){
				DMA1_TX_STS.write(create_sts(tmp_to_move, 	error, dma_tag  , 0));
			}
			dma_tag  = (dma_tag + 1) & 0xf;
			len_tmp -=tmp_to_move;
		}
		//recall segmentator
		sts = dma_mover_unpacked(
			DMA0_RX_CMD, DMA0_RX_STS,
			DMA1_RX_CMD, DMA1_RX_STS,
			DMA1_TX_CMD, DMA1_TX_STS,
			UDP_PKT_CMD, UDP_PKT_STS,
			TCP_PKT_CMD, TCP_PKT_STS,
			segment_size, max_dma_in_flight,
			len,
			op0_addr    , op1_addr  , res_addr,
			dst_rank	, mpi_tag 	,  comm	,
			which_dma);
		
		len_tmp		= len;	
		dma_tag     = 0;
		//go further only if len > 0
		if( which_dma && len_tmp > 0) {
			tmp_to_move = (len_tmp > segment_size ? segment_size : len_tmp);

			if( (which_dma & USE_OP0_DMA) && len_tmp > 0 ){
				//check that cmd is issued 
				cmd         = DMA0_RX_CMD.read();
				nerrors     +=( cmd != create_dma_cmd(tmp_to_move, op0_addr, dma_tag) );
				if (nerrors){ cout << "DMA0_RX CMD not correct!" << endl; return 1; }
				op0_addr    += tmp_to_move;
				//check that dma_mover signals a dma_tag mismatch
				nerrors +=( sts.range(1*DMA_ERR_BITS-1, 0*DMA_ERR_BITS) 	!= (DMA_NOT_OKAY_ERROR | ( error << 1 ) ));
				if (nerrors){ cout << "Dma movement didn't return error!" << endl; return 1; }

			}
			if( (which_dma & USE_OP1_DMA) && len_tmp > 0 ){
				//check that cmd is issued 
				cmd         = DMA1_RX_CMD.read();
				nerrors     +=( cmd != create_dma_cmd(tmp_to_move, op1_addr, dma_tag) );
				if (nerrors){ cout << "DMA1_RX CMD not correct!" << endl; return 1; }
				op1_addr    += tmp_to_move;
				//check that dma_mover signals a dma_tag mismatch
				nerrors +=( sts.range(2*DMA_ERR_BITS-1, 1*DMA_ERR_BITS)	!= (DMA_NOT_OKAY_ERROR | ( error << 1 ) ));
				if (nerrors){ cout << "Dma movement didn't return error!" << endl; return 1; }

			}
			if( (which_dma & (USE_RES_DMA | USE_RES_DMA_WITHOUT_TLAST) ) && len_tmp > 0 ){
				//check that cmd is issued 
				cmd         = DMA1_TX_CMD.read();
				nerrors     +=( cmd != create_dma_cmd(tmp_to_move, res_addr, dma_tag,  ( (which_dma & USE_RES_DMA) ? 1 : 0 )  ) ) ;
				if (nerrors){ cout << "DMA1_TX CMD not correct!" << endl; return 1; }
				res_addr    += tmp_to_move;
				//check that dma_mover signals a dma_tag mismatch
				nerrors +=( sts.range(3*DMA_ERR_BITS-1, 2*DMA_ERR_BITS) 	!= (DMA_NOT_OKAY_ERROR | ( error << 1 ) ));
				if (nerrors){ cout << "Dma movement didn't return error!" << endl; return 1; }
			}


			len_tmp -= tmp_to_move;
			dma_tag  = (dma_tag + 1) & 0xf;
		}
	
		if (!DMA0_RX_CMD.empty()){ cout << "DMA0_RX CMD not empty!" << endl; return 1; }
		if (!DMA1_RX_CMD.empty()){ cout << "DMA1_RX CMD not empty!" << endl; return 1; }
		if (!DMA1_TX_CMD.empty()){ cout << "DMA1_TX CMD not empty!" << endl; return 1; }
		if (!DMA0_RX_STS.empty()){ cout << "DMA0_RX STS not empty!" << endl; return 1; }
		if (!DMA1_RX_STS.empty()){ cout << "DMA1_RX STS not empty!" << endl; return 1; }
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
	nerrors 	+= test_dma_tag_errors( USE_OP0_DMA							   ,  1);
	nerrors 	+= test_dma_tag_errors( USE_OP0_DMA | USE_OP1_DMA			   ,  1);
	nerrors 	+= test_dma_tag_errors( USE_OP0_DMA | USE_OP1_DMA | USE_RES_DMA,  1);
    cout << "Errors: " << nerrors << " after testing for tag errors" <<  endl;
	nerrors 	+= test_dma_errors( USE_OP0_DMA							   ,  1);
	nerrors 	+= test_dma_errors( USE_OP0_DMA | USE_OP1_DMA			   ,  1);
	nerrors 	+= test_dma_errors( USE_OP0_DMA | USE_OP1_DMA | USE_RES_DMA,  1);
    cout << "Errors: " << nerrors << " after testing for dma errors" <<  endl;
	for(int i = 0; i < 10 ; i++){
		nerrors 	+= test_pkts( USE_PACKETIZER_TCP, i*10 + i);
		nerrors 	+= test_pkts( USE_PACKETIZER_UDP, i*10 + i);
    	cout << "Errors: " << nerrors << " after testing pkts" <<  endl;
	}

	nerrors 	+= test_pkts_error( USE_PACKETIZER_TCP, 100);
	nerrors 	+= test_pkts_error( USE_PACKETIZER_UDP, 100);
	cout << "Errors: " << nerrors << " after testing pkts error" <<  endl;
    return (nerrors == 0 ? 0 : 1);
}
