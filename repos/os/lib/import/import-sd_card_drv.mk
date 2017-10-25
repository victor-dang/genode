ifeq ($(filter-out $(SPECS),exynos5),)
SPEC = exynos5
else ifeq ($(filter-out $(SPECS),imx53),)
SPEC = imx
else ifeq ($(filter-out $(SPECS),wand_quad),)
SPEC = imx
INC_DIR += $(REP_DIR)/src/drivers/sd_card/spec/wand_quad
else ifeq ($(filter-out $(SPECS),nit6_solox),)
SPEC = imx
INC_DIR += $(REP_DIR)/src/drivers/sd_card/spec/nit6_solox
else ifeq ($(filter-out $(SPECS),omap4),)
SPEC = omap4
else ifeq ($(filter-out $(SPECS),pbxa9),)
SPEC = pbxa9
else ifeq ($(filter-out $(SPECS),rpi),)
SPEC = rpi
endif

INC_DIR += $(REP_DIR)/src/drivers/sd_card $(REP_DIR)/src/drivers/sd_card/spec/$(SPEC)
