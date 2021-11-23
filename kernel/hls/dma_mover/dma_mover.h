#ifndef DMA_MOVER_H

    #define DMA_MOVER_H
    #include "hls_stream.h"
	#include "ap_int.h"
	#include "ap_utils.h"

	using namespace hls;
    #define USE_NONE    0
    #define USE_OP0_DMA 1
    #define USE_OP1_DMA 2
    #define USE_RES_DMA 4
    #define USE_OP2_DMA 8
    #define USE_RES_DMA_WITHOUT_TLAST 16
    #define USE_PACKETIZER_TCP 32
    #define USE_PACKETIZER_UDP 64

    #define DMA_SUCCESS                                   0    
    #define DMA_TAG_MISMATCH_ERROR                        1     
    #define DMA_INTERNAL_ERROR                            2     
    #define DMA_DECODE_ERROR                              4  
    #define DMA_SLAVE_ERROR                               8 
    #define DMA_NOT_OKAY_ERROR                            16     
    #define DMA_NOT_END_OF_PACKET_ERROR                   32            
    #define DMA_NOT_EXPECTED_BTT_ERROR                    64
            
    
#endif