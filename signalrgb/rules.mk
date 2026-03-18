# Adapted from https://discord.com/channels/440868230475677696/440868230475677698/1095484933549600808

GENERATED_VERSIONS_FILE = $(MODULE_PATH_SIGNALRGB)/qmk_version.h

clean_qmk_version_h:
	rm -f $(GENERATED_VERSIONS_FILE)

$(GENERATED_VERSIONS_FILE): clean_qmk_version_h
	echo "#pragma once" > $(GENERATED_VERSIONS_FILE)
	echo "#define QMK_VERSION_BYTE_1 $$(git -C "$(TOP_DIR)" describe --tags | cut -d. -f1)" >> $(GENERATED_VERSIONS_FILE)
	echo "#define QMK_VERSION_BYTE_2 $$(git -C "$(TOP_DIR)" describe --tags | cut -d. -f2)" >> $(GENERATED_VERSIONS_FILE)
	echo "#define QMK_VERSION_BYTE_3 $$(git -C "$(TOP_DIR)" describe --tags | cut -d. -f3 | cut -d- -f1)" >> $(GENERATED_VERSIONS_FILE)

generated-files: $(GENERATED_VERSIONS_FILE)

POST_CONFIG_H += $(GENERATED_VERSIONS_FILE)
