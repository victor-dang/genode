REQUIRES = arm_v7

TARGET   = nic_drv
LIBS     = base lx_kit_setjmp fec_nic_include
SRC_CC   = main.cc platform.cc
INC_DIR += $(PRG_DIR)

# lx_kit
SRC_CC  += env.cc irq.cc malloc.cc scheduler.cc timer.cc
INC_DIR += $(REP_DIR)/src/include
INC_DIR += $(REP_DIR)/src/include/spec/arm
INC_DIR += $(REP_DIR)/src/lib/usb/include/spec/arm

# contrib code
LX_CONTRIB_DIR := $(call select_from_ports,dde_linux)/src/drivers/nic/fec
SRC_C          += $(notdir $(wildcard $(LX_CONTRIB_DIR)/drivers/net/ethernet/freescale/*.c))
INC_DIR        += $(LX_CONTRIB_DIR)/include

#
# Linux sources are C89 with GNU extensions
#
CC_C_OPT += -std=gnu89

#
# Reduce build noise of compiling contrib code
#
CC_OPT_fec_ptp  = -Wno-unused-but-set-variable -Wno-unused-variable \
                  -Wno-maybe-uninitialized -Wno-uninitialized
CC_OPT_fec_main = -Wno-unused-but-set-variable -Wno-unused-variable

vpath %.c  $(LX_CONTRIB_DIR)/drivers/net/ethernet/freescale
vpath %.cc $(PRG_DIR)
vpath %.cc $(REP_DIR)/src/lx_kit
