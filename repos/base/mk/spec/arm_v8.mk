#
# \brief  Build-system configurations for ARMv8
# \author Stefan Kalkowski
# \date   2017-05-13
#

# denote wich specs are also fullfilled by this spec
SPECS += arm64

# configure compiler
CC_MARCH += -march=armv8-a

#
# ARM-specific Genode headers
#
REP_INC_DIR += include/spec/arm64

SPECS += 64bit

include $(BASE_DIR)/mk/spec/64bit.mk
