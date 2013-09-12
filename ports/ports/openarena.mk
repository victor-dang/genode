OPENARENA      = openarena-0.8.8
OPENARENA_URL  = http://download.tuxfamily.org/openarena/rel/088/openarena-0.8.8.zip
PORTS += $(OPENARENA)

prepare:: $(CONTRIB_DIR)/$(OPENARENA)

$(DOWNLOAD_DIR)/$(OPENARENA).zip:
	$(VERBOSE)wget -c -P $(DOWNLOAD_DIR) $(OPENARENA_URL) && touch $@

$(CONTRIB_DIR)/$(OPENARENA): $(DOWNLOAD_DIR)/$(OPENARENA).zip
	$(VERBOSE)unzip -d $(CONTRIB_DIR) $<
