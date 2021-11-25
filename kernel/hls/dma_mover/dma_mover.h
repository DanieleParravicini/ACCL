#ifndef DMA_MOVER_H

    #define DMA_MOVER_H
    #include "hls_stream.h"
	#include "ap_int.h"
	#include "ap_utils.h"
    #include <iostream>
    #include <stdio.h>
    using namespace hls;

    //DMA MOVER PKTS
    #define PKT_LEN_START                    0
    #define PKT_LEN_END                     31
    #define PKT_DST_RANK_START              32
    #define PKT_DST_RANK_END                63
    #define PKT_MPI_TAG_START               64
    #define PKT_MPI_TAG_END                 95
    #define PKT_OP0_ADDR_START              96
    #define PKT_OP0_ADDR_END               159
    #define PKT_OP1_ADDR_START             160
    #define PKT_OP1_ADDR_END               223
    #define PKT_RES_ADDR_START             224
    #define PKT_RES_ADDR_END               287
    #define PKT_WHICH_DMA_START            288
    #define PKT_WHICH_DMA_END              295
    #define PKT_COMM_OFFSET_START          296
    #define PKT_COMM_OFFSET_END            307
    #define PKT_SIZE                       320
    //ERR 
    #define DMA_ERR_BITS                     8
    //USE DMA FLAGS
    #define USE_NONE                    	 0
    #define USE_OP0_DMA                 	 1
    #define USE_OP1_DMA                 	 2
    #define USE_RES_DMA                 	 4
    #define USE_OP2_DMA                 	 8
    #define USE_RES_DMA_WITHOUT_TLAST   	16
    #define USE_PACKETIZER_TCP          	32
    #define USE_PACKETIZER_UDP          	64
    //DMA ERROR FLAGS
    #define DMA_SUCCESS                      0    
    #define DMA_TAG_MISMATCH_ERROR           1     
    #define DMA_INTERNAL_ERROR               2     
    #define DMA_DECODE_ERROR                 4  
    #define DMA_SLAVE_ERROR                  8 
    #define DMA_NOT_OKAY_ERROR              16     
    #define DMA_NOT_END_OF_PACKET_ERROR     32            
    #define DMA_NOT_EXPECTED_BTT_ERROR      64
    #define PACK_SEQ_NUMBER_ERROR          128
    //COMMUNICATOR OFFSET
    #define COMM_SIZE_OFFSET                 0
    #define COMM_LOCAL_RANK_OFFSET           1
    #define COMM_RANKS_OFFSET                2
    //RANK OFFSET
    #define RANK_IP_OFFSET                   0
    #define RANK_PORT_OFFSET                 1
    #define RANK_INBOUND_SEQ_OFFSET          2
    #define RANK_OUTBOUND_SEQ_OFFSET         3
    #define RANK_SESSION_OFFSET              4
    #define RANK_SIZE                        5

    typedef struct {
        unsigned int ip;
        unsigned int port;
        unsigned int inbound_seq;
        unsigned int outbound_seq;
        unsigned int session;
    } comm_rank;

    typedef struct {
        unsigned int size;
        unsigned int local_rank;
        comm_rank* ranks;
    } communicator;
        
#endif