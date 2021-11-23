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
	stream<ap_uint<512> > &UDP_PKT_CMD,	
	stream<ap_uint<512> > &TCP_PKT_CMD,	
	unsigned int btt,
	ap_uint<64> op0_addr,
	ap_uint<64> op1_addr,
	ap_uint<64> res_addr,
	unsigned int tag
){

	if( which_dma & USE_OP0_DMA){
		start_dma( DMA0_RX_CMD	, btt, op0_addr, tag);
	}

	if( which_dma & USE_OP1_DMA){
		start_dma( DMA1_RX_CMD	, btt, op1_addr, tag);
	}

	if( which_dma & USE_RES_DMA){
		start_dma( DMA1_TX_CMD	, btt, res_addr, tag);
	} else if( which_dma & USE_RES_DMA_WITHOUT_TLAST){
		start_dma( DMA1_TX_CMD	, btt, res_addr, tag);
	} else if( which_dma & USE_PACKETIZER_TCP){
		//start packetizer
		//start_packetizer_message(dst_rank, cfg->res_len, msg_tag);
	} else if( which_dma & USE_PACKETIZER_UDP){
		//start packetizer
		//start_packetizer_message(dst_rank, cfg->res_len, msg_tag);
	}
}

ap_uint <32>  ack_dmas(unsigned int which_dma, 
	stream<ap_uint<32> > &DMA0_RX_STS,
	stream<ap_uint<32> > &DMA1_RX_STS,
	stream<ap_uint<32> > &DMA2_RX_STS,
	stream<ap_uint<32> > &DMA1_TX_STS,
	stream<ap_uint<32> > &UDP_PKT_STS,
	stream<ap_uint<32> > &TCP_PKT_STS,
	unsigned int expected_btt,
	unsigned int expected_tag
	){
	ap_uint<32> err = 0;
	if( which_dma & USE_OP0_DMA){
		err.range( 6, 0) 		= ack_dma( DMA0_RX_STS, expected_btt, expected_tag, 0  );
	}

	if( which_dma & USE_OP1_DMA){
		err.range( 13, 7) 		= ack_dma( DMA1_RX_STS, expected_btt, expected_tag, 0 );
	}

	if( which_dma & USE_RES_DMA){
		err.range( 20, 14) 		= ack_dma( DMA1_TX_STS, expected_btt, expected_tag, 1  );
	} 
	else if( which_dma & USE_RES_DMA_WITHOUT_TLAST){
		err.range( 20, 14) 		= ack_dma( DMA1_TX_STS, expected_btt, expected_tag, 0  );
	} else if( which_dma & USE_PACKETIZER_TCP){
		//start packetizer
		//start_packetizer_message(dst_rank, cfg->res_len, msg_tag);
		err.range( 20, 14) 	= DMA_NOT_OKAY_ERROR;
	} else if( which_dma & USE_PACKETIZER_UDP){
		//start packetizer
		//start_packetizer_message(dst_rank, cfg->res_len, msg_tag);
		err.range( 20, 14) 	= DMA_NOT_OKAY_ERROR;
	} else{
		err.range( 20, 14) 		= DMA_SUCCESS;
	}
	return err;
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
    //unsigned int op0_len,
    //unsigned int op1_len,
    //unsigned int res_len,
    unsigned int len,
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
	//#pragma HLS INTERFACE axis port = op0_len
	//#pragma HLS INTERFACE axis port = op1_len
	//#pragma HLS INTERFACE axis port = res_len
	#pragma HLS INTERFACE axis port = len
	#pragma HLS INTERFACE axis port = op0_addr
	#pragma HLS INTERFACE axis port = op1_addr
	#pragma HLS INTERFACE axis port = res_addr
	#pragma HLS INTERFACE axis port = which_dma

	ap_uint <32> err = 0;
	
	// issue multiple DMA commands to move data around.
	// len keeps number of bytes still to transfer
	unsigned int remaining_to_move, remaining_to_ack,  curr_len_move,
		curr_len_ack, i, dma_tag_start, dma_tag_acked;

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
		start_dmas(which_dma, DMA0_RX_CMD, DMA1_RX_CMD, DMA2_RX_CMD, DMA1_TX_CMD, UDP_PKT_CMD, TCP_PKT_CMD,  curr_len_move, op0_addr, op1_addr, res_addr, dma_tag_start);
		// update reference 
		remaining_to_move 	-= curr_len_move;
		op0_addr 			+= curr_len_move;
		op1_addr 			+= curr_len_move;
		res_addr 			+= curr_len_move;
		dma_tag_start 		 = (dma_tag_start + 1) & 0xf;
	}
	// 2.ack 1 and issue another dma transfer up until there's no more dma move to
	// issue
	while (remaining_to_move > 0 && err == DMA_SUCCESS) {
		if (remaining_to_ack < segment_size) {
			curr_len_ack = remaining_to_ack;
		}
		// wait for DMAs to finish
		err = ack_dmas(which_dma, DMA0_RX_STS, DMA1_RX_STS, DMA2_RX_STS, DMA1_TX_STS, UDP_PKT_STS, TCP_PKT_STS, curr_len_ack, dma_tag_acked);

		if (remaining_to_move < segment_size) {
			curr_len_move = remaining_to_move;
		}
		// start DMAs
		start_dmas(which_dma, DMA0_RX_CMD, DMA1_RX_CMD, DMA2_RX_CMD, DMA1_TX_CMD, UDP_PKT_CMD, TCP_PKT_CMD, curr_len_move, op0_addr, op1_addr, res_addr, dma_tag_start);
		// update reference 
		remaining_to_ack 	-= curr_len_ack;
		remaining_to_move 	-= curr_len_move;
		op0_addr 			+= curr_len_move;
		op1_addr 			+= curr_len_move;
		res_addr 			+= curr_len_move;
		dma_tag_start 		 = (dma_tag_start + 1) & 0xf;
		dma_tag_acked 		 = (dma_tag_acked + 1) & 0xf;
	}

	// 3. finish ack the remaining
	while (remaining_to_ack > 0 && err == DMA_SUCCESS) {
		if (remaining_to_ack < segment_size) {
			curr_len_ack = remaining_to_ack;
		}
		// wait for DMAs to finish
		err = ack_dmas(which_dma, DMA0_RX_STS, DMA1_RX_STS, DMA2_RX_STS, DMA1_TX_STS, UDP_PKT_STS, TCP_PKT_STS, curr_len_ack, dma_tag_acked);

		// update reference 
		remaining_to_ack 	-= curr_len_ack;
		dma_tag_acked 		 = (dma_tag_acked + 1) & 0xf;
	}
	
	return err;
}
