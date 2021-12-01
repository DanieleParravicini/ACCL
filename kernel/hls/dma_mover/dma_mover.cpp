#include "dma_mover.h"

// check DMA status and in case recalls the exception handler
int ack_dma(
	stream<ap_uint<32> > &dma_sts_channel,
    unsigned int expected_btt,
    unsigned int tag,
    unsigned int indeterminate_btt_mode_active) {
	int ret =0;
	ap_uint< 32> status = dma_sts_channel.read();
  // 3-0 TAG
  // 4 INTERNAL 	ERROR usually a btt=0 trigger this
  // 5 DECODE 		ERROR address decode error timeout
  // 6 SLAVE 		ERROR DMA encountered a slave reported error
  // 7 OKAY			the associated transfer command has been completed with
  // the OKAY response on all intermediate transfers.
  if ((status & 0x000f) != tag) {
    ret = ret | DMA_TAG_MISMATCH_ERROR;
  }
  if (status & 0x0010) {
    ret = ret | DMA_INTERNAL_ERROR;
  }
  if (status & 0x0020) {
    ret = ret | DMA_DECODE_ERROR;
  }
  if (status & 0x0040) {
    ret = ret | DMA_SLAVE_ERROR;
  }
  if (!(status & 0x0080)) {
    ret = ret | DMA_NOT_OKAY_ERROR;
  }
  if (indeterminate_btt_mode_active == 1) {
    // 30-8 		BYTES received
    // 31 		END OF PACKET indicates that S2MM received a TLAST
    if (!(status & 0x80000000)) {
       ret = ret | DMA_NOT_END_OF_PACKET_ERROR;
    }

    if (((status & 0x7FFFFF00) >> 8) != expected_btt) {
       ret = ret | DMA_NOT_EXPECTED_BTT_ERROR;
    }
  }
  return ret;
}

// start a DMA operations on a specific channel
void start_dma(
    stream<ap_uint<DMA_CMD_SIZE> > &dma_cmd_channel,
    unsigned int btt,
    ap_uint<64> addr,
    unsigned int tag,
	unsigned int tlast_expected=1) {
  ap_uint<DMA_CMD_SIZE> dma_cmd;

  dma_cmd.range( 31,  0) 	= 0x80800000 | btt;  // 31=DRR 30=EOF 29-24=DSA 23=Type 22-0=BTT 
  dma_cmd.range( 30, 30)    = tlast_expected;
  dma_cmd.range( 95, 32) 	= addr;
  dma_cmd.range(103, 96) 	= 	  0x2000 | tag; //15-12=xCACHE 11-8=xUSER 7-4=RSVD 3-0=TAG
  dma_cmd_channel.write(dma_cmd);
}

void start_dmas(unsigned int which_dma, 
	stream<ap_uint<DMA_CMD_SIZE> > &DMA0_RX_CMD,	
	stream<ap_uint<DMA_CMD_SIZE> > &DMA1_RX_CMD,	
	stream<ap_uint<DMA_CMD_SIZE> > &DMA1_TX_CMD,	
	unsigned int btt,
	ap_uint<64> op0_addr,
	ap_uint<64> op1_addr,
	ap_uint<64> res_addr,
	unsigned int dma_tag 
){

	if( which_dma & USE_OP0_DMA){
		start_dma( DMA0_RX_CMD							, btt, op0_addr, dma_tag);
	}

	if( which_dma & USE_OP1_DMA){
		start_dma( DMA1_RX_CMD							, btt, op1_addr, dma_tag);
	}

	if( which_dma & (USE_RES_DMA | USE_RES_DMA_WITHOUT_TLAST) ){		
		start_dma( DMA1_TX_CMD							, btt, res_addr, dma_tag, ((which_dma & USE_RES_DMA)? 1 : 0) );
	} 
}

ap_uint <32>  ack_dmas(unsigned int which_dma, 
	stream<ap_uint<32> > &DMA0_RX_STS,
	stream<ap_uint<32> > &DMA1_RX_STS,
	stream<ap_uint<32> > &DMA1_TX_STS,
	unsigned int expected_btt,
	unsigned int dma_tag
	){
	ap_uint<32> err = 0;
	if( which_dma & USE_OP0_DMA){
		err.range( 1*DMA_ERR_BITS-1, 0*DMA_ERR_BITS) 	= ack_dma( DMA0_RX_STS, expected_btt, dma_tag, 0  );
	}

	if( which_dma & USE_OP1_DMA){
		err.range( 2*DMA_ERR_BITS-1, 1*DMA_ERR_BITS) 	= ack_dma( DMA1_RX_STS, expected_btt, dma_tag, 0 );
	}

	if( which_dma & (USE_RES_DMA | USE_RES_DMA_WITHOUT_TLAST) ){
		err.range( 3*DMA_ERR_BITS-1, 2*DMA_ERR_BITS) 	= ack_dma( DMA1_TX_STS, expected_btt, dma_tag,  ( ( which_dma & USE_RES_DMA) ? 1 : 0 )    );
	} 
	
	return err;
}

void start_pkt(
	stream< ap_uint<PKT_CMD_SIZE> > &pkt_cmd_channel,
	unsigned int len,
	unsigned int dst_rank,
	unsigned int src_rank,
	unsigned int mpi_tag, 
	unsigned int sequence_number
) {
	// prepare header arguments
	ap_uint<PKT_CMD_SIZE> pkt_cmd   = 0;
	// enqueue message headers
	pkt_cmd.range( PKT_CMD_DST_END		,PKT_CMD_DST_START		) = dst_rank;
	pkt_cmd.range( PKT_CMD_LEN_END		,PKT_CMD_LEN_START		) = len;
	pkt_cmd.range( PKT_CMD_MPI_TAG_END	,PKT_CMD_MPI_TAG_START	) = mpi_tag;
	pkt_cmd.range( PKT_CMD_SRC_RANK_END	,PKT_CMD_SRC_RANK_START	) = src_rank;
	pkt_cmd.range( PKT_CMD_SEQ_NUM_END	,PKT_CMD_SEQ_NUM_START	) = sequence_number;
	
	pkt_cmd_channel.write( pkt_cmd );

}

// check that packetizer has finished processing every packet it was supposed to.
int ack_pkt(
	stream< ap_uint<32> > &pkt_ack_channel,
    unsigned int expected_seq_number) {
  	
	unsigned int  ack_seq_num;
  
    ack_seq_num = pkt_ack_channel.read();
    if (  expected_seq_number == ack_seq_num) {
	  return DMA_SUCCESS;
    } else {
      return PACK_SEQ_NUMBER_ERROR;
    }
}

void start_pkts(
	unsigned int which_dma,
	stream<ap_uint<PKT_CMD_SIZE> > &UDP_PKT_CMD,	
	stream<ap_uint<PKT_CMD_SIZE> > &TCP_PKT_CMD,	
	unsigned int btt,
	unsigned int dst_rank,
	unsigned int src_rank,
	unsigned int mpi_tag, 
	unsigned int sequence_number
){
	if( which_dma & USE_PACKETIZER_TCP){
		//start packetizer
		start_pkt( TCP_PKT_CMD, btt, dst_rank, src_rank, mpi_tag, sequence_number);
	} else if( which_dma & USE_PACKETIZER_UDP){
		//start packetizer
		start_pkt( UDP_PKT_CMD, btt, dst_rank, src_rank, mpi_tag, sequence_number);
	}
}

ap_uint <32> ack_pkts(
	unsigned int which_dma, 
	stream<ap_uint<32> > &UDP_PKT_STS,
	stream<ap_uint<32> > &TCP_PKT_STS,
	unsigned int expected_seq_number
){
	ap_uint<32> err = 0;
	if( which_dma & USE_PACKETIZER_TCP){
		//start packetizer
		err.range( 3*DMA_ERR_BITS-1, 2*DMA_ERR_BITS)	= ack_pkt( TCP_PKT_STS, expected_seq_number);
	} else if( which_dma & USE_PACKETIZER_UDP){
		//start packetizer
		//start_packetizer_message(dst_rank, cfg->res_len, msg_tag);
		err.range( 3*DMA_ERR_BITS-1, 2*DMA_ERR_BITS)	= ack_pkt( UDP_PKT_STS, expected_seq_number);
	} else{
		err.range( 3*DMA_ERR_BITS-1, 2*DMA_ERR_BITS)	= DMA_SUCCESS;
	} 
	return err;
}

ap_uint<32> dma_mover_unpacked_internal(
    stream<ap_uint<DMA_CMD_SIZE> > &DMA0_RX_CMD	, stream<ap_uint<32> > &DMA0_RX_STS, 
	stream<ap_uint<DMA_CMD_SIZE> > &DMA1_RX_CMD	, stream<ap_uint<32> > &DMA1_RX_STS,
	stream<ap_uint<DMA_CMD_SIZE> > &DMA1_TX_CMD	, stream<ap_uint<32> > &DMA1_TX_STS,
	stream<ap_uint<PKT_CMD_SIZE> > &UDP_PKT_CMD	, stream<ap_uint<32> > &UDP_PKT_STS,
    stream<ap_uint<PKT_CMD_SIZE> > &TCP_PKT_CMD	, stream<ap_uint<32> > &TCP_PKT_STS,
    unsigned int segment_size,
    unsigned int max_dma_in_flight,
    //unsigned int op0_len,
    //unsigned int op1_len,
    //unsigned int res_len,
    unsigned int len,
    ap_uint<64> op0_addr,
    ap_uint<64> op1_addr,
    ap_uint<64> res_addr,
    unsigned int dst_rank,
    unsigned int mpi_tag,
    unsigned int * exchange_mem,
	ap_uint <DMA_MOVER_CMD_COMM_OFFSET_END - DMA_MOVER_CMD_COMM_OFFSET_START  > comm_offset,
    unsigned int which_dma) 
{
	#pragma HLS PIPELINE II=1 style=flp
	// latency: 4, 7, 6 for each loop
	unsigned int src_rank = 0, sequence_number = 0, dst = 0;
	if ( which_dma & (USE_PACKETIZER_TCP | USE_PACKETIZER_UDP) ) {
		src_rank 		= *(exchange_mem + comm_offset +  COMM_LOCAL_RANK_OFFSET);
		sequence_number = *(exchange_mem + comm_offset +  COMM_RANKS_OFFSET + (dst_rank * RANK_SIZE) + RANK_OUTBOUND_SEQ_OFFSET) + 1;
		if ( which_dma & USE_PACKETIZER_TCP ) {
			dst = *(exchange_mem  + comm_offset + COMM_RANKS_OFFSET + dst_rank * RANK_SIZE + RANK_SESSION_OFFSET);
		} else {
			dst = *(exchange_mem  + comm_offset + COMM_RANKS_OFFSET + dst_rank * RANK_SIZE +    RANK_PORT_OFFSET);
		}
	}

	ap_uint <32> err = 0;
	
	// issue multiple DMA commands to move data around.
	// len keeps number of bytes still to transfer
	unsigned int remaining_to_move, remaining_to_ack,  curr_len_move,
		curr_len_ack, i, dma_tag_start, dma_tag_acked, sequence_number_acked;

	remaining_to_move 	= len;
	remaining_to_ack  	= len;
	dma_tag_start		= 0;
	dma_tag_acked		= 0;
	sequence_number_acked = sequence_number;
	// default size of transaction is full size
	curr_len_move = segment_size;
	curr_len_ack  = segment_size;
	// 1. issue at most max_dma_in_flight of segment_size
	for (i = 0; remaining_to_move > 0 && i < max_dma_in_flight; i++) 
	start_dma:{
		if (remaining_to_move < segment_size) {
			curr_len_move = remaining_to_move;//need to rephrase it to unroll loop
		}
		// start DMAs
		start_dmas(which_dma, DMA0_RX_CMD, DMA1_RX_CMD, DMA1_TX_CMD, curr_len_move, op0_addr, op1_addr, res_addr, dma_tag_start);
		start_pkts(which_dma, UDP_PKT_CMD, TCP_PKT_CMD, curr_len_move, dst, src_rank, mpi_tag, sequence_number );
		// update reference 
		remaining_to_move 	-= curr_len_move;
		op0_addr 			+= curr_len_move;
		op1_addr 			+= curr_len_move;
		res_addr 			+= curr_len_move;
		dma_tag_start 		 = (dma_tag_start + 1) & 0xf;
		sequence_number		+= 1;
	}
	// 2.ack 1 and issue another dma transfer up until there's no more dma move to
	// issue
	while (remaining_to_move > 0 && err == DMA_SUCCESS) 
	ack_and_start_dma:{
		if (remaining_to_ack < segment_size) {
			curr_len_ack = remaining_to_ack;//need to rephrase it to unroll loop
		}
		// wait for DMAs to finish
		err |= ack_dmas(which_dma, DMA0_RX_STS, DMA1_RX_STS, DMA1_TX_STS, curr_len_ack, dma_tag_acked);
		err |= ack_pkts(which_dma, UDP_PKT_STS, TCP_PKT_STS, sequence_number_acked  );

		if (remaining_to_move < segment_size) {
			curr_len_move = remaining_to_move;
		}
		// start DMAs
		start_dmas(which_dma, DMA0_RX_CMD, DMA1_RX_CMD, DMA1_TX_CMD, curr_len_move, op0_addr, op1_addr, res_addr, dma_tag_start);
		start_pkts(which_dma, UDP_PKT_CMD, TCP_PKT_CMD, curr_len_move, dst, src_rank, mpi_tag, sequence_number );
		// update reference 
		remaining_to_ack 	-= curr_len_ack;
		remaining_to_move 	-= curr_len_move;
		op0_addr 			+= curr_len_move;
		op1_addr 			+= curr_len_move;
		res_addr 			+= curr_len_move;
		dma_tag_start 		 = (dma_tag_start + 1) & 0xf;
		dma_tag_acked 		 = (dma_tag_acked + 1) & 0xf;
		sequence_number		  += 1;
		// uncomment this to get debug info about last sequence number acknowledged 
		// by the packetizer  but at the expense of pipelining
		if( err == DMA_SUCCESS){
			sequence_number_acked += 1;
		}
	}

	// 3. finish ack the remaining
	while (remaining_to_ack > 0 && err == DMA_SUCCESS) 
	ack_dma:{
		#pragma HLS PIPELINE II=1
		if (remaining_to_ack < segment_size) {
			curr_len_ack = remaining_to_ack;
		}
		// wait for DMAs to finish
		err |= ack_dmas(which_dma, DMA0_RX_STS, DMA1_RX_STS, DMA1_TX_STS, curr_len_ack, dma_tag_acked);
		err |= ack_pkts(which_dma, UDP_PKT_STS, TCP_PKT_STS, sequence_number_acked  );

		// update reference 
		remaining_to_ack 	-= curr_len_ack;
		dma_tag_acked 		 = (dma_tag_acked + 1) & 0xf;
		// uncomment this to get debug info about last sequence number acknowledged 
		// by the packetizer  but at the expense of pipelining
		if( err == DMA_SUCCESS){
			sequence_number_acked += 1;
		}
	}
	
	if( which_dma & (USE_PACKETIZER_UDP | USE_PACKETIZER_TCP)){
		*(exchange_mem + comm_offset +COMM_RANKS_OFFSET +  dst_rank * RANK_SIZE + RANK_OUTBOUND_SEQ_OFFSET) = sequence_number_acked;
	}
	return err;
}

void dma_mover(
    stream<ap_uint<DMA_CMD_SIZE> > &DMA0_RX_CMD	, stream<ap_uint<32> > &DMA0_RX_STS, 
	stream<ap_uint<DMA_CMD_SIZE> > &DMA1_RX_CMD	, stream<ap_uint<32> > &DMA1_RX_STS,
	stream<ap_uint<DMA_CMD_SIZE> > &DMA1_TX_CMD	, stream<ap_uint<32> > &DMA1_TX_STS,
	stream<ap_uint<PKT_CMD_SIZE> > &UDP_PKT_CMD	, stream<ap_uint<32> > &UDP_PKT_STS,
    stream<ap_uint<PKT_CMD_SIZE> > &TCP_PKT_CMD	, stream<ap_uint<32> > &TCP_PKT_STS,
    unsigned int segment_size,
    unsigned int max_dma_in_flight,
    unsigned int * exchange_mem,
    stream< ap_uint<DMA_MOVER_CMD_SIZE> > &pkt_stream,
	stream< ap_uint<32> > 		&return_stream){
	#pragma HLS INTERFACE 
	#pragma HLS INTERFACE s_axilite port=segment_size
	#pragma HLS INTERFACE s_axilite port=max_dma_in_flight
	#pragma HLS INTERFACE m_axi port=exchange_mem	depth=24 offset=slave num_read_outstanding=4	num_write_outstanding=4 bundle=mem
	#pragma HLS INTERFACE s_axilite port=return
	#pragma HLS INTERFACE axis port = DMA0_RX_CMD
	#pragma HLS INTERFACE axis port = DMA0_RX_STS
	#pragma HLS INTERFACE axis port = DMA1_RX_CMD
	#pragma HLS INTERFACE axis port = DMA1_RX_STS
	#pragma HLS INTERFACE axis port = DMA1_TX_CMD
	#pragma HLS INTERFACE axis port = DMA1_TX_STS
	#pragma HLS INTERFACE axis port = UDP_PKT_CMD
	#pragma HLS INTERFACE axis port = UDP_PKT_STS
	#pragma HLS INTERFACE axis port = TCP_PKT_CMD
	#pragma HLS INTERFACE axis port = TCP_PKT_STS
	#pragma HLS INTERFACE axis port = pkt_stream
	#pragma HLS INTERFACE axis port = return_stream
	#pragma HLS PIPELINE II=1 style=flp
	ap_uint< DMA_MOVER_CMD_SIZE> cmd  = pkt_stream.read();
	unsigned int len 		= cmd.range(DMA_MOVER_CMD_LEN_END		, DMA_MOVER_CMD_LEN_START			);
	unsigned int dst_rank 	= cmd.range(DMA_MOVER_CMD_DST_RANK_END	, DMA_MOVER_CMD_DST_RANK_START		);	
	unsigned int mpi_tag 	= cmd.range(DMA_MOVER_CMD_MPI_TAG_END	, DMA_MOVER_CMD_MPI_TAG_START	    );
	unsigned int op0_addr 	= cmd.range(DMA_MOVER_CMD_OP0_ADDR_END	, DMA_MOVER_CMD_OP0_ADDR_START		);	
	unsigned int op1_addr 	= cmd.range(DMA_MOVER_CMD_OP1_ADDR_END	, DMA_MOVER_CMD_OP1_ADDR_START		);	
	unsigned int res_addr 	= cmd.range(DMA_MOVER_CMD_RES_ADDR_END	, DMA_MOVER_CMD_RES_ADDR_START		);	
	unsigned int which_dma 	= cmd.range(DMA_MOVER_CMD_WHICH_DMA_END	, DMA_MOVER_CMD_WHICH_DMA_START		);	

	ap_uint <DMA_MOVER_CMD_COMM_OFFSET_END - DMA_MOVER_CMD_COMM_OFFSET_START > comm_offset = 0;
	comm_offset.range(DMA_MOVER_CMD_COMM_OFFSET_END - DMA_MOVER_CMD_COMM_OFFSET_START - 1,  2) = cmd.range(DMA_MOVER_CMD_COMM_OFFSET_END , DMA_MOVER_CMD_COMM_OFFSET_START	+ 2	);	

	ap_uint<32> ret = dma_mover_unpacked_internal(
		DMA0_RX_CMD	,DMA0_RX_STS,
		DMA1_RX_CMD	,DMA1_RX_STS,
		DMA1_TX_CMD	,DMA1_TX_STS,
		UDP_PKT_CMD	,UDP_PKT_STS,
		TCP_PKT_CMD	,TCP_PKT_STS,
		segment_size,
		max_dma_in_flight,
		len,
		op0_addr,
		op1_addr,
		res_addr,
		dst_rank,
		mpi_tag,
		exchange_mem,
		comm_offset, 
		which_dma
	);

	return_stream.write(ret);
}
