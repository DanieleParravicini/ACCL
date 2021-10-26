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

PLATFORM ?= xilinx_u280_xdma_201920_3
ERNIC_XO  =/ernic.xo

ifeq (u250,$(findstring u250, $(PLATFORM)))
	FPGAPART =xcu250-figd2104-2L-e
	BOARDPART=xilinx.com:au250:part0:1.3
else ifeq (u280,$(findstring u280, $(PLATFORM)))
	FPGAPART =xcu280-fsvh2892-2L-e
	BOARDPART=xilinx.com:au280:part0:1.1
else
	$(error Unsupported PLATFORM)
endif


.PHONY: xo
xo: $(ERNIC_XO)

$(ERNIC_XO): tcl/generate_bd.tcl ip
	rm -rf project_ernic
	rm -rf vivado*.jou
	rm -rf vivado*.log
	vivado -mode batch -source $< -tclargs $(FPGAPART) $(BOARDPART)

ip: tcl/generate_pkt_filter.tcl hdl/src/*.sv
	rm -rf project_pk_filter
	rm -rf vivado*.jou
	rm -rf vivado*.log
	vivado -mode batch -source $< -tclargs $(FPGAPART) $(BOARDPART)

distclean:
	git clean -xfd
