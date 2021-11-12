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

int main(){
    stream< ap_uint<32> > scenario;
    ap_uint<NUM_BITS_MASTER> DMA0_RX; 
    ap_uint<NUM_BITS_MASTER> DMA1_RX; 
    ap_uint<NUM_BITS_MASTER> DMA2_RX; 
    ap_uint<NUM_BITS_MASTER> ARITH_RES; 
    ap_uint<NUM_BITS_MASTER> EXT_KRNL;

    for(int i = 0; i < 10; i++){
        scenario.write(DATAPATH_DMA_LOOPBACK);
        tdest_creation(	
            scenario,
            &DMA0_RX,
            &DMA1_RX,
            &DMA2_RX,
            &ARITH_RES,
            &EXT_KRNL
        );

        if( DMA0_RX   != MAIN_SWITCH_M_DISABLE) { printf("LP DMA0_RX tdest failure\n");return 1;}
        if( DMA1_RX   != MAIN_SWITCH_M_DMA1_TX) { printf("LP DMA1_RX tdest failure\n");return 1;}
        if( DMA2_RX   != MAIN_SWITCH_M_DISABLE) { printf("LP DMA2_RX tdest failure\n");return 1;}
        if( ARITH_RES != MAIN_SWITCH_M_DISABLE) { printf("LP ARITH_RES tdest failure\n");return 1;}
        if( EXT_KRNL  != MAIN_SWITCH_M_DISABLE) { printf("LP EXT_KRNL tdest failure\n");return 1;}

        scenario.write(DATAPATH_DMA_REDUCTION);
        tdest_creation(	
            scenario,
            &DMA0_RX,
            &DMA1_RX,
            &DMA2_RX,
            &ARITH_RES,
            &EXT_KRNL
        );

        if( DMA0_RX   != MAIN_SWITCH_M_ARITH_OP0) { printf("DMA_R DMA0_RX tdest failure\n");return 1;}
        if( DMA1_RX   != MAIN_SWITCH_M_ARITH_OP1) { printf("DMA_R DMA1_RX tdest failure\n");return 1;}
        if( DMA2_RX   != MAIN_SWITCH_M_DISABLE) { printf("DMA_R DMA2_RX tdest failure\n");return 1;}
        if( ARITH_RES != MAIN_SWITCH_M_DMA1_TX) { printf("DMA_R ARITH_RES tdest failure\n");return 1;}
        if( EXT_KRNL  != MAIN_SWITCH_M_DISABLE) { printf("DMA_R EXT_KRNL tdest failure\n");return 1;}
    
        scenario.write(DATAPATH_OFFCHIP_TX_UDP);
        tdest_creation(	
            scenario,
            &DMA0_RX,
            &DMA1_RX,
            &DMA2_RX,
            &ARITH_RES,
            &EXT_KRNL
        );

        if( DMA0_RX   != MAIN_SWITCH_M_DISABLE) { printf("TX UDP OFFCHIP DMA0_RX tdest failure\n");return 1;}
        if( DMA1_RX   != MAIN_SWITCH_M_UDP_TX) { printf("TX UDP OFFCHIP DMA1_RX tdest failure\n");return 1;}
        if( DMA2_RX   != MAIN_SWITCH_M_DISABLE) { printf("TX UDP OFFCHIP DMA2_RX tdest failure\n");return 1;}
        if( ARITH_RES != MAIN_SWITCH_M_DISABLE) { printf("TX UDP OFFCHIP ARITH_RES tdest failure\n");return 1;}
        if( EXT_KRNL  != MAIN_SWITCH_M_DISABLE) { printf("TX UDP OFFCHIP EXT_KRNL tdest failure\n");return 1;}
    
        scenario.write(DATAPATH_OFFCHIP_TX_TCP);
        tdest_creation(	
            scenario,
            &DMA0_RX,
            &DMA1_RX,
            &DMA2_RX,
            &ARITH_RES,
            &EXT_KRNL
        );

        if( DMA0_RX   != MAIN_SWITCH_M_DISABLE) { printf("TX TCP DMA0_RX tdest failure\n");return 1;}
        if( DMA1_RX   != MAIN_SWITCH_M_TCP_TX) { printf("TX TCP DMA1_RX tdest failure\n");return 1;}
        if( DMA2_RX   != MAIN_SWITCH_M_DISABLE) { printf("TX TCP DMA2_RX tdest failure\n");return 1;}
        if( ARITH_RES != MAIN_SWITCH_M_DISABLE) { printf("TX TCP ARITH_RES tdest failure\n");return 1;}
        if( EXT_KRNL  != MAIN_SWITCH_M_DISABLE) { printf("TX TCP EXT_KRNL tdest failure\n");return 1;}
    
        scenario.write(DATAPATH_OFFCHIP_UDP_REDUCTION);
        tdest_creation(	
            scenario,
            &DMA0_RX,
            &DMA1_RX,
            &DMA2_RX,
            &ARITH_RES,
            &EXT_KRNL
        );

        if( DMA0_RX   != MAIN_SWITCH_M_ARITH_OP0) { printf("UDP RED DMA0_RX tdest failure\n");return 1;}
        if( DMA1_RX   != MAIN_SWITCH_M_ARITH_OP1) { printf("UDP RED DMA1_RX tdest failure\n");return 1;}
        if( DMA2_RX   != MAIN_SWITCH_M_DISABLE) { printf("UDP RED DMA2_RX tdest failure\n");return 1;}
        if( ARITH_RES != MAIN_SWITCH_M_UDP_TX) { printf("UDP RED ARITH_RES tdest failure\n");return 1;}
        if( EXT_KRNL  != MAIN_SWITCH_M_DISABLE) { printf("UDP RED EXT_KRNL tdest failure\n");return 1;}
    
        scenario.write(DATAPATH_OFFCHIP_TCP_REDUCTION);
        tdest_creation(	
            scenario,
            &DMA0_RX,
            &DMA1_RX,
            &DMA2_RX,
            &ARITH_RES,
            &EXT_KRNL
        );

        if( DMA0_RX   != MAIN_SWITCH_M_ARITH_OP0) { printf("TCP RED DMA0_RX tdest failure\n");return 1;}
        if( DMA1_RX   != MAIN_SWITCH_M_ARITH_OP1) { printf("TCP RED DMA1_RX tdest failure\n");return 1;}
        if( DMA2_RX   != MAIN_SWITCH_M_DISABLE) { printf("TCP RED DMA2_RX tdest failure\n");return 1;}
        if( ARITH_RES != MAIN_SWITCH_M_TCP_TX) { printf("TCP RED ARITH_RES tdest failure\n");return 1;}
        if( EXT_KRNL  != MAIN_SWITCH_M_DISABLE) { printf("TCP RED EXT_KRNL tdest failure\n");return 1;}

        scenario.write(DATAPATH_DMA_EXT_LOOPBACK);
        tdest_creation(	
            scenario,
            &DMA0_RX,
            &DMA1_RX,
            &DMA2_RX,
            &ARITH_RES,
            &EXT_KRNL
        );

        if( DMA0_RX   != MAIN_SWITCH_M_DISABLE) { printf("DMA EXT DMA0_RX tdest failure\n");return 1;}
        if( DMA1_RX   != MAIN_SWITCH_M_EXT_KRNL) { printf("DMA EXT DMA1_RX tdest failure\n");return 1;}
        if( DMA2_RX   != MAIN_SWITCH_M_DISABLE) { printf("DMA EXT DMA2_RX tdest failure\n");return 1;}
        if( ARITH_RES != MAIN_SWITCH_M_DISABLE) { printf("DMA EXT ARITH_RES tdest failure\n");return 1;}
        if( EXT_KRNL  != MAIN_SWITCH_M_DMA1_TX) { printf("DMA EXT EXT_KRNL tdest failure\n");return 1;}
        
        for(int j = DATAPATH_DMA_EXT_LOOPBACK + 1; j < 10; j++ ){

            scenario.write(j);
            tdest_creation(	
                scenario,
                &DMA0_RX,
                &DMA1_RX,
                &DMA2_RX,
                &ARITH_RES,
                &EXT_KRNL
            );

            if( DMA0_RX   != MAIN_SWITCH_M_DISABLE) { printf("non mapped DMA0_RX tdest failure\n");return 1;}
            if( DMA1_RX   != MAIN_SWITCH_M_DISABLE) { printf("non mapped DMA1_RX tdest failure\n");return 1;}
            if( DMA2_RX   != MAIN_SWITCH_M_DISABLE) { printf("non mapped DMA2_RX tdest failure\n");return 1;}
            if( ARITH_RES != MAIN_SWITCH_M_DISABLE) { printf("non mapped ARITH_RES tdest failure\n");return 1;}
            if( EXT_KRNL  != MAIN_SWITCH_M_DISABLE) { printf("non mapped EXT_KRNL tdest failure\n");return 1;}
        }
    
    }
    printf("TB passed\n");
    return 0;
}
