#ifndef DMA_MOVER_H

    #define DMA_MOVER_H
    #include "hls_stream.h"
	#include "ap_int.h"
	#include "ap_utils.h"
    #include <iostream>
    #include <stdio.h>
    using namespace hls;

    //DMA MOVER PKTS
    #define DMA_MOVER_CMD_LEN_START              0
    #define DMA_MOVER_CMD_LEN_END                DMA_MOVER_CMD_LEN_START         + 31
    #define DMA_MOVER_CMD_DST_RANK_START         DMA_MOVER_CMD_LEN_END           + 1
    #define DMA_MOVER_CMD_DST_RANK_END           DMA_MOVER_CMD_DST_RANK_START    + 31
    #define DMA_MOVER_CMD_MPI_TAG_START          DMA_MOVER_CMD_DST_RANK_END      + 1  
    #define DMA_MOVER_CMD_MPI_TAG_END            DMA_MOVER_CMD_MPI_TAG_START     + 31
    #define DMA_MOVER_CMD_OP0_ADDR_START         DMA_MOVER_CMD_MPI_TAG_END       + 1
    #define DMA_MOVER_CMD_OP0_ADDR_END           DMA_MOVER_CMD_OP0_ADDR_START    + 63
    #define DMA_MOVER_CMD_OP1_ADDR_START         DMA_MOVER_CMD_OP0_ADDR_END      + 1
    #define DMA_MOVER_CMD_OP1_ADDR_END           DMA_MOVER_CMD_OP1_ADDR_START    + 63
    #define DMA_MOVER_CMD_RES_ADDR_START         DMA_MOVER_CMD_OP1_ADDR_END      + 1
    #define DMA_MOVER_CMD_RES_ADDR_END           DMA_MOVER_CMD_RES_ADDR_START    + 63
    #define DMA_MOVER_CMD_WHICH_DMA_START        DMA_MOVER_CMD_RES_ADDR_END      + 1
    #define DMA_MOVER_CMD_WHICH_DMA_END          DMA_MOVER_CMD_WHICH_DMA_START   + 6
    #define DMA_MOVER_CMD_COMM_OFFSET_START      DMA_MOVER_CMD_WHICH_DMA_END     + 1
    #define DMA_MOVER_CMD_COMM_OFFSET_END        DMA_MOVER_CMD_COMM_OFFSET_START + 13
    #define DMA_MOVER_CMD_SIZE                   320
    //DMA CMD
    #define DMA_CMD_SIZE                         104 
    //PKT CMD
    #define PKT_CMD_DST_START                    0
    #define PKT_CMD_DST_END                      PKT_CMD_DST_START      + 31
    #define PKT_CMD_LEN_START                    PKT_CMD_DST_END        + 1
    #define PKT_CMD_LEN_END                      PKT_CMD_LEN_START      + 31
    #define PKT_CMD_MPI_TAG_START                PKT_CMD_LEN_END        + 1
    #define PKT_CMD_MPI_TAG_END                  PKT_CMD_MPI_TAG_START  + 31
    #define PKT_CMD_SRC_RANK_START               PKT_CMD_MPI_TAG_END    + 1
    #define PKT_CMD_SRC_RANK_END                 PKT_CMD_SRC_RANK_START + 31
    #define PKT_CMD_SEQ_NUM_START                PKT_CMD_SRC_RANK_END   + 1
    #define PKT_CMD_SEQ_NUM_END                  PKT_CMD_SEQ_NUM_START  + 31
    #define PKT_CMD_SIZE                         512 
    //ERR 
    #define DMA_ERR_BITS                     7
    //USE DMA FLAGS
    #define USE_BITS                         7
    #define USE_OP0_DMA_BIT                  0
    #define USE_OP1_DMA_BIT                	 1
    #define USE_OP2_DMA_BIT                	 2
    #define USE_RES_DMA_BIT                	 3
    #define USE_RES_DMA_WITHOUT_TLAST_BIT  	 4
    #define USE_PACKETIZER_TCP_BIT         	 5
    #define USE_PACKETIZER_UDP_BIT         	 6

    #define USE_NONE                    	 DMA_MOVER_CMD_WHICH_DMA_END - DMA_MOVER_CMD_WHICH_DMA_START + 1
    #define USE_OP0_DMA                 	 1<<USE_OP0_DMA_BIT
    #define USE_OP1_DMA                 	 1<<USE_OP1_DMA_BIT
    #define USE_OP2_DMA                 	 1<<USE_OP2_DMA_BIT
    #define USE_RES_DMA                 	 1<<USE_RES_DMA_BIT
    #define USE_RES_DMA_WITHOUT_TLAST   	 1<<USE_RES_DMA_WITHOUT_TLAST_BIT
    #define USE_PACKETIZER_TCP          	 1<<USE_PACKETIZER_TCP_BIT
    #define USE_PACKETIZER_UDP          	 1<<USE_PACKETIZER_UDP_BIT
    //DMA ERROR FLAGS
    #define DMA_SUCCESS                      0    
    #define DMA_TAG_MISMATCH_ERROR           1     
    #define DMA_INTERNAL_ERROR               2     
    #define DMA_DECODE_ERROR                 4  
    #define DMA_SLAVE_ERROR                  8 
    #define DMA_NOT_OKAY_ERROR              16     
    #define DMA_NOT_END_OF_PACKET_ERROR     32            
    #define DMA_NOT_EXPECTED_BTT_ERROR      64
    #define PACK_SEQ_NUMBER_ERROR            1
    #define UDP_PACK_SEQ_NUMBER_ERROR        1<<UDP_PACK_SEQ_NUMBER_ERROR_BIT
    #define TCP_PACK_SEQ_NUMBER_ERROR        1<<TCP_PACK_SEQ_NUMBER_ERROR_BIT
    
    #define UDP_PACK_SEQ_NUMBER_ERROR_BIT   21
    #define TCP_PACK_SEQ_NUMBER_ERROR_BIT   22
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
