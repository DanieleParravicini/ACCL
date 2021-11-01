#ifndef _DMA_ENQUEUE_H_
    #define _DMA_ENQUEUE_H_
	#include "hls_stream.h"
	#include "ap_int.h"
	#include "ap_utils.h"

	using namespace hls;

    typedef struct {
        unsigned int status;
        unsigned int addrl;
        unsigned int addrh;
        unsigned int max_len;
        unsigned int dma_tag;
        unsigned int rx_tag;
        unsigned int rx_len;
        unsigned int rx_src;
        unsigned int sequence_number;
    } rx_buffer;
    #define STATUS_IDLE     0x00
    #define STATUS_ENQUEUED 0x01
    #define STATUS_RESERVED 0x02
    #define STATUS_ERROR    0x04
    #define DMA_MAX_BTT     0x7FFFFF

    void dma_enqueue(	
        ap_uint<32>          use_tcp,
        ap_uint<32>          nbufs,
        stream< ap_uint<32> > &cmd_dma_tcp,
        stream< ap_uint<32> > &cmd_dma_udp,
        stream< ap_uint<32> > &inflight_queue,
        ap_uint<32>*         rx_buffers
    );

    #define STATUS_OFFSET           0
    #define ADDRL_OFFSET            1
    #define ADDRH_OFFSET            2
    #define MAX_LEN_OFFSET          3
    #define DMA_TAG_OFFSET          4
    #define RX_TAG_OFFSET           5
    #define RX_LEN_OFFSET           6
    #define RX_SRC_OFFSET           7
    #define SEQUENCE_NUMBER_OFFSET  8   
    #define SPARE_BUFFER_SIZE       36
    #define SPARE_BUFFER_FIELDS     9       
#endif
