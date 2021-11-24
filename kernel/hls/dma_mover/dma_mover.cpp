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
    stream<ap_uint<128> > &dma_cmd_channel,
    unsigned int btt,
    ap_uint<64> addr,
    unsigned int tag) {
  ap_uint<128> dma_cmd;
  // 31=DRR 30=EOF 29-24=DSA 23=Type 22-0=BTT 15-12=xCACHE 11-8=xUSER 7-4=RSVD 3-0=TAG
  dma_cmd.range( 31,  0) 	= 0xC0800000 | btt;  
  dma_cmd.range( 95, 32) 	= addr;
  dma_cmd.range(127, 96) 	= 	  0x2000 | tag;
  dma_cmd_channel.write(dma_cmd);
}

void start_dmas(unsigned int which_dma, 
	stream<ap_uint<128> > &DMA0_RX_CMD,	
	stream<ap_uint<128> > &DMA1_RX_CMD,	
	stream<ap_uint<128> > &DMA2_RX_CMD,	
	stream<ap_uint<128> > &DMA1_TX_CMD,	
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

	if( which_dma & USE_RES_DMA){		
		start_dma( DMA1_TX_CMD							, btt, res_addr, dma_tag);
	} else if( which_dma & USE_RES_DMA_WITHOUT_TLAST){
		start_dma( DMA1_TX_CMD							, btt, res_addr, dma_tag);
	} 
}

ap_uint <32>  ack_dmas(unsigned int which_dma, 
	stream<ap_uint<32> > &DMA0_RX_STS,
	stream<ap_uint<32> > &DMA1_RX_STS,
	stream<ap_uint<32> > &DMA2_RX_STS,
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

	if( which_dma & USE_RES_DMA){
		err.range( 3*DMA_ERR_BITS-1, 2*DMA_ERR_BITS) 	= ack_dma( DMA1_TX_STS, expected_btt, dma_tag, 1  );
	} 
	else if( which_dma & USE_RES_DMA_WITHOUT_TLAST){
		err.range( 3*DMA_ERR_BITS-1, 2*DMA_ERR_BITS)	= ack_dma( DMA1_TX_STS, expected_btt, dma_tag, 0  );
	} else {
		err.range( 3*DMA_ERR_BITS-1, 2*DMA_ERR_BITS)	= DMA_SUCCESS;
	}
	return err;
}

void start_pkt(
	stream< ap_uint<512> > &pkt_cmd_channel,
	unsigned int len,
	unsigned int dst_rank,
	unsigned int src_rank,
	unsigned int mpi_tag, 
	unsigned int sequence_number
) {
	// prepare header arguments
	ap_uint<512> pkt_cmd   = 0;
	// enqueue message headers
	pkt_cmd.range( 31,  0) = dst_rank;
	pkt_cmd.range( 63, 32) = len;
	pkt_cmd.range( 95, 64) = mpi_tag;
	pkt_cmd.range(127, 96) = src_rank;
	pkt_cmd.range(159,128) = sequence_number;
	
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
	stream<ap_uint<512> > &UDP_PKT_CMD,	
	stream<ap_uint<512> > &TCP_PKT_CMD,	
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

ap_uint<32> dma_mover(
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
    //unsigned int res_len,
    unsigned int len,
    ap_uint<64> op0_addr,
    ap_uint<64> op1_addr,
    ap_uint<64> res_addr,
    unsigned int dst_rank,
    unsigned int mpi_tag,
    unsigned int * comm,
    unsigned int which_dma) 
{
	#pragma HLS INTERFACE s_axilite port = segment_size
	#pragma HLS INTERFACE s_axilite port = max_dma_in_flight
	#pragma HLS INTERFACE ap_ctrl_chain port = return
	#pragma HLS INTERFACE axis port = DMA0_RX_CMD
	#pragma HLS INTERFACE axis port = DMA0_RX_STS
	#pragma HLS INTERFACE axis port = DMA1_RX_CMD
	#pragma HLS INTERFACE axis port = DMA1_RX_STS
	#pragma HLS INTERFACE axis port = DMA2_RX_CMD
	#pragma HLS INTERFACE axis port = DMA2_RX_STS
	#pragma HLS INTERFACE axis port = DMA1_TX_CMD
	#pragma HLS INTERFACE axis port = DMA1_TX_STS
	#pragma HLS INTERFACE axis port = UDP_PKT_CMD
	#pragma HLS INTERFACE axis port = UDP_PKT_STS
	#pragma HLS INTERFACE axis port = TCP_PKT_CMD
	#pragma HLS INTERFACE axis port = TCP_PKT_STS
	//#pragma HLS INTERFACE axis port = op0_len
	//#pragma HLS INTERFACE axis port = op1_len
	//#pragma HLS INTERFACE axis port = res_len
	#pragma HLS INTERFACE axis port = len
	#pragma HLS INTERFACE axis port = op0_addr
	#pragma HLS INTERFACE axis port = op1_addr
	#pragma HLS INTERFACE axis port = res_addr
	#pragma HLS INTERFACE axis port = which_dma
	#pragma HLS INTERFACE axis port = dst_rank
	#pragma HLS INTERFACE axis port = mpi_tag
	#pragma HLS INTERFACE m_axi port= comm	depth=24 offset=slave num_read_outstanding=40	num_write_outstanding=40 bundle=mem
	#pragma HLS PIPELINE II=1 style=flp

	unsigned int src_rank = 0, sequence_number = 0, dst = 0;
	if ( which_dma & (USE_PACKETIZER_TCP | USE_PACKETIZER_UDP) ) {
		src_rank 		= *(comm + COMM_LOCAL_RANK_OFFSET);
		sequence_number = *(comm + COMM_RANKS_OFFSET + (dst_rank * RANK_SIZE) + RANK_OUTBOUND_SEQ_OFFSET) + 1;
		if ( which_dma & USE_PACKETIZER_TCP ) {
			dst = *(comm + COMM_RANKS_OFFSET + dst_rank * RANK_SIZE + RANK_SESSION_OFFSET);
		} else {
			dst = *(comm + COMM_RANKS_OFFSET + dst_rank * RANK_SIZE +    RANK_PORT_OFFSET);
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
		start_dmas(which_dma, DMA0_RX_CMD, DMA1_RX_CMD, DMA2_RX_CMD, DMA1_TX_CMD, curr_len_move, op0_addr, op1_addr, res_addr, dma_tag_start);
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
		err |= ack_dmas(which_dma, DMA0_RX_STS, DMA1_RX_STS, DMA2_RX_STS, DMA1_TX_STS, curr_len_ack, dma_tag_acked);
		err |= ack_pkts(which_dma, UDP_PKT_STS, TCP_PKT_STS, sequence_number_acked  );

		if (remaining_to_move < segment_size) {
			curr_len_move = remaining_to_move;
		}
		// start DMAs
		start_dmas(which_dma, DMA0_RX_CMD, DMA1_RX_CMD, DMA2_RX_CMD, DMA1_TX_CMD, curr_len_move, op0_addr, op1_addr, res_addr, dma_tag_start);
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
		if( err == DMA_SUCCESS){
			sequence_number_acked += 1;
		}
	}

	// 3. finish ack the remaining
	while (remaining_to_ack > 0 && err == DMA_SUCCESS) 
		ack_dma:{
		if (remaining_to_ack < segment_size) {
			curr_len_ack = remaining_to_ack;
		}
		// wait for DMAs to finish
		err |= ack_dmas(which_dma, DMA0_RX_STS, DMA1_RX_STS, DMA2_RX_STS, DMA1_TX_STS, curr_len_ack, dma_tag_acked);
		err |= ack_pkts(which_dma, UDP_PKT_STS, TCP_PKT_STS, sequence_number_acked  );

		// update reference 
		remaining_to_ack 	-= curr_len_ack;
		dma_tag_acked 		 = (dma_tag_acked + 1) & 0xf;
		if( err == DMA_SUCCESS){
			sequence_number_acked += 1;
		}
	}
	
	if( which_dma & (USE_PACKETIZER_UDP | USE_PACKETIZER_TCP)){
		*(comm + COMM_RANKS_OFFSET +  dst_rank * RANK_SIZE + RANK_OUTBOUND_SEQ_OFFSET) = sequence_number_acked;
	}
	return err;
}
