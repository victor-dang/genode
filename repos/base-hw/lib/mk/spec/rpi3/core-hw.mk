#
# \brief  Build config for Genodes core process
# \author Stefan Kalkowski
# \date   2017-05-13
#

# add include paths
INC_DIR += $(REP_DIR)/src/core/spec/rpi3

# add C++ sources
SRC_CC += platform_services.cc
SRC_CC += spec/arm/platform_support.cc
SRC_CC += spec/arm64/kernel/cpu.cc
SRC_CC += spec/arm64/kernel/pd.cc
SRC_CC += spec/arm64/kernel/thread.cc
SRC_CC += kernel/kernel.cc

SRC_S  += spec/arm64/crt0.s
SRC_S  += spec/arm64/mode_transition.s

# include less specific configuration
include $(REP_DIR)/lib/mk/core-hw.inc

