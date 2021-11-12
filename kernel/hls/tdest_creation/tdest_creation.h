#ifndef _DMA_DEQUEUE_H_
    #define _DMA_DEQUEUE_H_
	#include "hls_stream.h"
	#include "ap_int.h"
	#include "ap_utils.h"

	using namespace hls;

    #define NUM_BITS_MASTER 3

    void tdest_creation(	
				stream< ap_uint<32> > &scenario,
				ap_uint<NUM_BITS_MASTER> * MAIN_SWITCH_S_DMA0_RX,
                ap_uint<NUM_BITS_MASTER> * MAIN_SWITCH_S_DMA1_RX,
				ap_uint<NUM_BITS_MASTER> * MAIN_SWITCH_S_DMA2_RX,
				ap_uint<NUM_BITS_MASTER> * MAIN_SWITCH_S_ARITH_RES,
                ap_uint<NUM_BITS_MASTER> * MAIN_SWITCH_S_EXT_KRNL

    );

    
    #define DATAPATH_DMA_LOOPBACK          1
    #define DATAPATH_DMA_REDUCTION         2
    #define DATAPATH_OFFCHIP_TX_UDP        3
    #define DATAPATH_OFFCHIP_TX_TCP        4
    #define DATAPATH_OFFCHIP_UDP_REDUCTION 5
    #define DATAPATH_OFFCHIP_TCP_REDUCTION 6
    #define DATAPATH_DMA_EXT_LOOPBACK      7
    //#define DATAPATH_OFFCHIP_RX_UDP      8 not used up to now since DMA are physically linked to depacketizer
    //#define DATAPATH_OFFCHIP_RX_TCP      9 not used up to now since DMA are physically linked to depacketizer

    #define MAIN_SWITCH_M_UDP_TX    0
    #define MAIN_SWITCH_M_TCP_TX    1
    #define MAIN_SWITCH_M_DMA1_TX   2
    #define MAIN_SWITCH_M_ARITH_OP0 3
    #define MAIN_SWITCH_M_ARITH_OP1 4
    #define MAIN_SWITCH_M_EXT_KRNL  5
    #define MAIN_SWITCH_M_DISABLE   6
    
    //#define MAIN_SWITCH_S_DMA0_RX   0
    //#define MAIN_SWITCH_S_DMA1_RX   1
    //#define MAIN_SWITCH_S_DMA2_RX   2
    //#define MAIN_SWITCH_S_ARITH_RES 3
    //#define MAIN_SWITCH_S_EXT_KRNL  4

#endif
