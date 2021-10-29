
# /*******************************************************************************
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

`timescale 1 ns / 1 ns

module RDMA_pkt_filter_one_port
(
input          clk,
input          rst,
//input streaming signals from CMAC
input [511:0]   s_axis_tdata,
input [63:0]    s_axis_tkeep,
input           s_axis_tlast,
input [0:0]     s_axis_tuser,
input           s_axis_tvalid,

// roce streaming interface
output reg [511:0]  roce_m_axism_axis_tdata,
output reg [63:0]   roce_m_axism_axis_tkeep,
output reg          roce_m_axism_axis_tlast,
output reg [0:0]    roce_m_axism_axis_tuser,
output reg          roce_m_axism_axis_tvalid
);

localparam [15:0] ETH_TYPE_IPv4 = 16'h0008; //frame type IPv4 = 0x0800 in Big endian  https://www.iana.org/assignments/ieee-802-numbers/ieee-802-numbers.xhtml
localparam [15:0] ETH_TYPE_IPv6 = 16'hdd86; //frame type IPv6 = 0x86dd in Big endian
localparam [15:0] UDP_PORT      = 16'hB712; //UDP port for ROCE traffic = d4791 = h12B7 which in Big endian is B712 
localparam [ 7:0] UDP_PROTO_ID  =  8'h11;   //UDP proto identifier = 17
localparam BITS_STATES = 2;
typedef enum logic [BITS_STATES-1 : 0] {  S_IDLE  , 
                            S_ROCE ,
                            S_NO_ROCE 
                    } State;
State  curState  , nextState;

// roce streaming interface
reg [511:0]  roce_m_axism_axis_tdata_next;
reg [63:0]   roce_m_axism_axis_tkeep_next;
reg          roce_m_axism_axis_tlast_next;
reg [0:0]    roce_m_axism_axis_tuser_next;
reg          roce_m_axism_axis_tvalid_next;
reg          is_roce;

always_ff @(posedge clk) begin
    if(rst == 1'b0) 
        begin
            curState <= S_IDLE;
        end 
    else 
        begin
            curState <= nextState;
        end
    
    roce_m_axism_axis_tdata  <= roce_m_axism_axis_tdata_next;
    roce_m_axism_axis_tkeep  <= roce_m_axism_axis_tkeep_next;
    roce_m_axism_axis_tlast  <= roce_m_axism_axis_tlast_next;
    roce_m_axism_axis_tuser  <= roce_m_axism_axis_tuser_next;
    roce_m_axism_axis_tvalid <= roce_m_axism_axis_tvalid_next;       
end

always_comb begin
    nextState   = curState;
    is_roce     = 1'b0;


    case (curState)
        S_IDLE: 
            begin
                if(s_axis_tvalid) begin
                    if(         (s_axis_tdata[12*8+:16] == ETH_TYPE_IPv4) && (s_axis_tdata[23*8+:8] == UDP_PROTO_ID) && (s_axis_tdata[36*8+:16] == UDP_PORT)) begin
                        is_roce = 1'b1;
                        if(!s_axis_tlast)
                            current_state <= S_ROCE;

                    end else if((s_axis_tdata[12*8+:16] == ETH_TYPE_IPv6) && (s_axis_tdata[20*8+:8] == UDP_PROTO_ID) && (s_axis_tdata[56*8+:16] == UDP_PORT)) begin 
                        is_roce = 1'b1;
                        if(!s_axis_tlast)
                            current_state <= S_ROCE;
                    end
                    else
                    begin
                        is_roce = 1'b0;
                        if(!s_axis_tlast)
                            current_state <= S_NO_ROCE;
                    end
                end
            end
        S_NO_ROCE: 
            begin
                is_roce = 1'b0;
                if(s_axis_tlast)
                    current_state <= S_IDLE; 

            end
        S_ROCE:
            begin
                is_roce = 1'b1;
                if(s_axis_tlast)
                    current_state <= S_IDLE;
            end
        default:
            begin
                is_roce = 1'b0;
            end
    endcase
end

always_comb begin
    
    non_roce_m_axis_tdata_next       = s_axis_tdata;


    if (is_roce)
        begin
            non_roce_m_axistkeep_next        = s_axis_tkeep;
            non_roce_m_axistlast_next        = s_axis_tlast;
            non_roce_m_axistuser_next        = s_axis_tuser;
            non_roce_m_axistvalid_next       = s_axis_tvalid;
        end
    else
        begin
            roce_m_axism_axis_tkeep_next     = 0;
            roce_m_axism_axis_tlast_next     = 0;
            roce_m_axism_axis_tuser_next     = 0;
            roce_m_axism_axis_tvalid_next    = 0;

        end
end


endmodule


