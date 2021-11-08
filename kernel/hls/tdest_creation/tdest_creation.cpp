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

#include "tdest_creation.h"

using namespace hls;
using namespace std;

void tdest_creation(	
        stream< ap_uint<32> > &scenario,
        unsigned int *  MAIN_SWITCH_S_DMA0_RX,
        unsigned int *  MAIN_SWITCH_S_DMA1_RX,
        unsigned int *  MAIN_SWITCH_S_DMA2_RX,
        unsigned int *  MAIN_SWITCH_S_ARITH_RES,
        unsigned int *  MAIN_SWITCH_S_EXT_KRNL

){  
    #pragma HLS INTERFACE axis 		port=scenario
    #pragma HLS INTERFACE ap_none   port=MAIN_SWITCH_S_DMA0_RX
    #pragma HLS INTERFACE ap_none   port=MAIN_SWITCH_S_DMA1_RX
    #pragma HLS INTERFACE ap_none   port=MAIN_SWITCH_S_DMA2_RX
    #pragma HLS INTERFACE ap_none   port=MAIN_SWITCH_S_ARITH_RES
    #pragma HLS INTERFACE ap_none   port=MAIN_SWITCH_S_EXT_KRNL
    switch(scenario.read()){
        case DATAPATH_DMA_LOOPBACK:
            *MAIN_SWITCH_S_DMA0_RX   = MAIN_SWITCH_M_DISABLE;
			*MAIN_SWITCH_S_DMA1_RX   = MAIN_SWITCH_M_DMA1_TX;
            *MAIN_SWITCH_S_DMA2_RX   = MAIN_SWITCH_M_DISABLE;
            *MAIN_SWITCH_S_ARITH_RES = MAIN_SWITCH_M_DISABLE;
            *MAIN_SWITCH_S_EXT_KRNL  = MAIN_SWITCH_M_DISABLE;	
			break;
		case DATAPATH_DMA_REDUCTION:
            *MAIN_SWITCH_S_DMA0_RX   = MAIN_SWITCH_M_ARITH_OP0;
            *MAIN_SWITCH_S_DMA1_RX   = MAIN_SWITCH_M_ARITH_OP1;
            *MAIN_SWITCH_S_DMA2_RX   = MAIN_SWITCH_M_DISABLE;
            *MAIN_SWITCH_S_ARITH_RES = MAIN_SWITCH_M_DMA1_TX;
            *MAIN_SWITCH_S_EXT_KRNL  = MAIN_SWITCH_M_DISABLE;		
			break;
		case DATAPATH_OFFCHIP_TX_UDP:
            *MAIN_SWITCH_S_DMA0_RX   = MAIN_SWITCH_M_DISABLE;
            *MAIN_SWITCH_S_DMA1_RX   = MAIN_SWITCH_M_UDP_TX;
            *MAIN_SWITCH_S_DMA2_RX   = MAIN_SWITCH_M_DISABLE;
            *MAIN_SWITCH_S_ARITH_RES = MAIN_SWITCH_M_DISABLE;
            *MAIN_SWITCH_S_EXT_KRNL  = MAIN_SWITCH_M_DISABLE;		
			break;
		case DATAPATH_OFFCHIP_TX_TCP:
            *MAIN_SWITCH_S_DMA0_RX   = MAIN_SWITCH_M_DISABLE;
            *MAIN_SWITCH_S_DMA1_RX   = MAIN_SWITCH_M_TCP_TX;
            *MAIN_SWITCH_S_DMA2_RX   = MAIN_SWITCH_M_DISABLE;
            *MAIN_SWITCH_S_ARITH_RES = MAIN_SWITCH_M_DISABLE;
            *MAIN_SWITCH_S_EXT_KRNL  = MAIN_SWITCH_M_DISABLE;
			break;
		case DATAPATH_OFFCHIP_UDP_REDUCTION:
            *MAIN_SWITCH_S_DMA0_RX   = MAIN_SWITCH_M_ARITH_OP0;
            *MAIN_SWITCH_S_DMA1_RX   = MAIN_SWITCH_M_ARITH_OP1;
            *MAIN_SWITCH_S_DMA2_RX   = MAIN_SWITCH_M_DISABLE;
            *MAIN_SWITCH_S_ARITH_RES = MAIN_SWITCH_M_UDP_TX;
            *MAIN_SWITCH_S_EXT_KRNL  = MAIN_SWITCH_M_DISABLE;
			break;
		case DATAPATH_OFFCHIP_TCP_REDUCTION:
            *MAIN_SWITCH_S_DMA0_RX   = MAIN_SWITCH_M_ARITH_OP0;
            *MAIN_SWITCH_S_DMA1_RX   = MAIN_SWITCH_M_ARITH_OP1;
            *MAIN_SWITCH_S_DMA2_RX   = MAIN_SWITCH_M_DISABLE;
            *MAIN_SWITCH_S_ARITH_RES = MAIN_SWITCH_M_TCP_TX;
            *MAIN_SWITCH_S_EXT_KRNL  = MAIN_SWITCH_M_DISABLE;
			break;
		case DATAPATH_DMA_EXT_LOOPBACK:
            *MAIN_SWITCH_S_DMA0_RX   = MAIN_SWITCH_M_DISABLE;
            *MAIN_SWITCH_S_DMA1_RX   = MAIN_SWITCH_M_EXT_KRNL;
            *MAIN_SWITCH_S_DMA2_RX   = MAIN_SWITCH_M_DISABLE;
            *MAIN_SWITCH_S_ARITH_RES = MAIN_SWITCH_M_DISABLE;
            *MAIN_SWITCH_S_EXT_KRNL  = MAIN_SWITCH_M_DMA1_TX;
			break;//TODO: add DATAPATH_OFFCHIP_RX
		default:
            *MAIN_SWITCH_S_DMA0_RX   = MAIN_SWITCH_M_DISABLE;
            *MAIN_SWITCH_S_DMA1_RX   = MAIN_SWITCH_M_DISABLE;
            *MAIN_SWITCH_S_DMA2_RX   = MAIN_SWITCH_M_DISABLE;
            *MAIN_SWITCH_S_ARITH_RES = MAIN_SWITCH_M_DISABLE;
            *MAIN_SWITCH_S_EXT_KRNL  = MAIN_SWITCH_M_DISABLE;
	}

}
