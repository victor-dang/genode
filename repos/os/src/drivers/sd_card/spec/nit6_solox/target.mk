TARGET   = sd_card_drv
REQUIRES = arm_v7a
SRC_CC   = main.cc
SRC_CC  += adma2.cc spec/imx/driver.cc spec/imx6/driver.cc
LIBS    += base
INC_DIR += $(REP_DIR)/src/drivers/sd_card \
           $(REP_DIR)/src/drivers/sd_card/spec/imx \
           $(PRG_DIR)

vpath %.cc $(REP_DIR)/src/drivers/sd_card
