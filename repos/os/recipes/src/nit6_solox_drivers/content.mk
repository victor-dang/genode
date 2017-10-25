include $(GENODE_DIR)/repos/base/recipes/src/content.inc

content: src/drivers/sd_card LICENSE

src/drivers/sd_card:
	mkdir -p $@/spec
	cp    $(REP_DIR)/src/drivers/sd_card/*.h  $@/
	cp    $(REP_DIR)/src/drivers/sd_card/*.cc $@/
	cp -r $(REP_DIR)/src/drivers/sd_card/spec/imx $@/spec/
	cp -r $(REP_DIR)/src/drivers/sd_card/spec/imx6 $@/spec/
	cp -r $(REP_DIR)/src/drivers/sd_card/spec/nit6_solox $@/spec/
