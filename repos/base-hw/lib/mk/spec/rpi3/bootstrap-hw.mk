INC_DIR += $(BASE_DIR)/../base-hw/src/bootstrap/spec/rpi3

SRC_CC  += bootstrap/spec/rpi3/platform.cc
SRC_S   += bootstrap/spec/arm64/crt0.s

include $(BASE_DIR)/../base-hw/lib/mk/bootstrap-hw.inc

