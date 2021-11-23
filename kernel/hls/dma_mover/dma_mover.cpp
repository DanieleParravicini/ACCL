#include "dma_mover.h"

// check DMA status and in case recalls the exception handler
int ack_dma(
	stream<ap_uint<32> > &dma_sts_channel,
    unsigned int expected_btt,
    unsigned int tag,
    unsigned int indeterminate_btt_mode_active) {
	
	ap_uint< 32> status = dma_sts_channel.read();
  // 3-0 TAG
  // 4 INTERNAL 	ERROR usually a btt=0 trigger this
  // 5 DECODE 		ERROR address decode error timeout
  // 6 SLAVE 		ERROR DMA encountered a slave reported error
  // 7 OKAY			the associated transfer command has been completed with
  // the OKAY response on all intermediate transfers.
  if ((status & 0x000f) != tag) {
    return DMA_TAG_MISMATCH_ERROR;
  }
  if (status & 0x0010) {
    return DMA_INTERNAL_ERROR;
  }
  if (status & 0x0020) {
    return DMA_DECODE_ERROR;
  }
  if (status & 0x0040) {
    return DMA_SLAVE_ERROR;
  }
  if (!(status & 0x0080)) {
    return DMA_NOT_OKAY_ERROR;
  }
  if (indeterminate_btt_mode_active == 1) {
    // 30-8 		BYTES received
    // 31 		END OF PACKET indicates that S2MM received a TLAST
    if (!(status & 0x80000000)) {
      return DMA_NOT_END_OF_PACKET_ERROR;
    }

    if (((status & 0x7FFFFF00) >> 8) != expected_btt) {
      return DMA_NOT_EXPECTED_BTT_ERROR;
    }
  }
  return 0;
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

int schedule_dma(
	unsigned int 			segment_size,
    unsigned int 			max_dma_in_flight,
    unsigned int 			len,
    ap_uint<64> 	 			addr,
	stream< ap_uint<128> > &DMA_CMD, 
	stream< ap_uint<32> >  &DMA_STS,
	unsigned int 			indeterminate_btt_mode_active 
){
	
	// issue multiple DMA commands to move data around.
	// len keeps number of bytes still to transfer
	unsigned int remaining_to_move, remaining_to_ack,  curr_len_move,
		curr_len_ack, i, dma_tag_start, dma_tag_acked, err = DMA_SUCCESS;
	remaining_to_move 	= len;
	remaining_to_ack  	= len;
	dma_tag_start		= 0;
	dma_tag_acked		= 0;

	// default size of transaction is full size
	curr_len_move = segment_size;
	curr_len_ack  = segment_size;
	// 1. issue at most max_dma_in_flight of segment_size
	for (i = 0; remaining_to_move > 0 && i < max_dma_in_flight; i++) {
		if (remaining_to_move < segment_size) {
		curr_len_move = remaining_to_move;
		}
		// start DMAs
		start_dma(DMA_CMD, curr_len_move, addr, dma_tag_start);
		// update reference 
		remaining_to_move 	-= curr_len_move;
		addr	 			+= curr_len_move;
		dma_tag_start = (dma_tag_start + 1) & 0xf;
	}
	// 2.ack 1 and issue another dma transfer up until there's no more dma move to
	// issue
	while (remaining_to_move > 0 && err == DMA_SUCCESS) {
		if (remaining_to_ack < segment_size) {
			curr_len_ack = remaining_to_ack;
		}
		// wait for DMAs to finish
		err = ack_dma(DMA_STS, curr_len_ack, dma_tag_acked, indeterminate_btt_mode_active);

		if (remaining_to_move < segment_size) {
		curr_len_move = remaining_to_move;
		}
		// start DMAs
		start_dma(DMA_CMD, curr_len_move, addr, dma_tag_start);
		// update reference 
		remaining_to_ack 	-= curr_len_ack;
		remaining_to_move 	-= curr_len_move;
		addr	 			+= curr_len_move;
		dma_tag_start 		 = (dma_tag_start + 1) & 0xf;
		dma_tag_acked 		 = (dma_tag_acked + 1) & 0xf;
	}

	// 3. finish ack the remaining
	while (remaining_to_ack > 0 && err == DMA_SUCCESS) {
		if (remaining_to_ack < segment_size) {
			curr_len_ack = remaining_to_ack;
		}
		// wait for DMAs to finish
		err = ack_dma(DMA_STS, curr_len_ack, dma_tag_acked, indeterminate_btt_mode_active);
		// update reference 
		remaining_to_ack 	-= curr_len_ack;
		dma_tag_acked 		 = (dma_tag_acked + 1) & 0xf;
	}
	return err;
}

int schedule_op0(
	unsigned int 			which_dma,
	unsigned int 			segment_size,
    unsigned int 			max_dma_in_flight,
    unsigned int 			len,
    ap_uint<64> 	 			addr,
	stream< ap_uint<128> > &DMA0_RX_CMD, 
	stream< ap_uint<32> >  &DMA0_RX_STS
	){
	
	if( which_dma & USE_OP0_DMA){
		return schedule_dma( segment_size, max_dma_in_flight, len, addr, DMA0_RX_CMD, DMA0_RX_STS, 0 );
	}else{
		return DMA_SUCCESS;
	}
}

int schedule_op1(
	unsigned int 			which_dma,
	unsigned int 			segment_size,
    unsigned int 			max_dma_in_flight,
    unsigned int 			len,
    ap_uint<64> 	 			addr,
	stream< ap_uint<128> > &DMA1_RX_CMD, 
	stream< ap_uint<32> >  &DMA1_RX_STS
	){
	
	if( which_dma & USE_OP1_DMA){
		return schedule_dma( segment_size, max_dma_in_flight, len, addr, DMA1_RX_CMD, DMA1_RX_STS, 0 );
	}else{
		return DMA_SUCCESS;
	}
	
}

int schedule_res(
	unsigned int 			which_dma,
	unsigned int 			segment_size,
    unsigned int 			max_dma_in_flight,
	unsigned int 			use_tcp,
    unsigned int 			len,
    ap_uint<64> 	 			addr,
	stream< ap_uint<128> > &DMA1_TX_CMD, 
	stream< ap_uint<32> >  &DMA1_TX_STS)
{


	if( which_dma & USE_RES_DMA){
		return schedule_dma( segment_size, max_dma_in_flight, len, addr, DMA1_TX_CMD, DMA1_TX_STS, 1 );
	} 
	else if( which_dma & USE_RES_DMA_WITHOUT_TLAST){
		return schedule_dma( segment_size, max_dma_in_flight, len, addr, DMA1_TX_CMD, DMA1_TX_STS, 0 );
	} 
	else if( which_dma & USE_PACKETIZER){
		//start packetizer
		//start_packetDMA_NOT_OKAY_ERRORizer_message(dst_rank, cfg->res_len, msg_tag);
		if (use_tcp){
			return DMA_NOT_OKAY_ERROR;
		}else{
			return DMA_NOT_OKAY_ERROR;
		}
	}else{
		return 0;
	}
}



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
    unsigned int res_len,
    ap_uint<64> op0_addr,
    ap_uint<64> op1_addr,
    ap_uint<64> res_addr,
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
	#pragma HLS INTERFACE axis port = op0_len
	#pragma HLS INTERFACE axis port = op1_len
	#pragma HLS INTERFACE axis port = res_len
	#pragma HLS INTERFACE axis port = op0_addr
	#pragma HLS INTERFACE axis port = op1_addr
	#pragma HLS INTERFACE axis port = res_addr
	#pragma HLS INTERFACE axis port = which_dma
	#pragma HLS INTERFACE axis port = use_tcp

  	// issue multiple DMA commands to move data around.
  	
	ap_uint <96> err = 0;

	#pragma HLS DATAFLOW
	err.range(31, 0) = schedule_op0(which_dma, segment_size, max_dma_in_flight			, op0_len, op0_addr, DMA0_RX_CMD, DMA0_RX_STS);
	err.range(63,32) = schedule_op1(which_dma, segment_size, max_dma_in_flight			, op1_len, op1_addr, DMA1_RX_CMD, DMA1_RX_STS);
	err.range(95,64) = schedule_res(which_dma, segment_size, max_dma_in_flight, use_tcp, res_len, res_addr, DMA1_TX_CMD, DMA1_TX_STS);

	 
	
	
	
	return err;
}
