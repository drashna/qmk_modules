# Set to `no` to disable automatic generation of autocorrect_data.h.
AUTOCORRECT_AUTO_GENERATE ?= yes

# Dictionary filenames to search for in VPATH (first match wins).
AUTOCORRECT_DICT_LIST ?= autocorrection_dict.txt autocorrect_dict.txt

# Resolve once at parse time so recipe execution uses stable values.
AUTOCORRECT_DICT_SEARCH_PATHS := $(strip $(sort $(VPATH) $(MODULE_PATH_AUTOCORRECT) $(CURDIR)))
AUTOCORRECT_DICT_FILE := $(firstword $(foreach p,$(AUTOCORRECT_DICT_SEARCH_PATHS),$(foreach n,$(AUTOCORRECT_DICT_LIST),$(wildcard $(p)/$(n)))))
AUTOCORRECT_EXISTING_DATA_H := $(firstword $(foreach p,$(AUTOCORRECT_DICT_SEARCH_PATHS),$(wildcard $(p)/autocorrect_data.h)))

ifneq ($(filter yes true 1,$(strip $(AUTOCORRECT_AUTO_GENERATE))),)
ifeq ($(strip $(AUTOCORRECT_EXISTING_DATA_H)),)
ifneq ($(strip $(AUTOCORRECT_DICT_FILE)),)
AUTOCORRECT_DATA_H := $(INTERMEDIATE_OUTPUT)/src/autocorrect_data.h

generated-files: $(AUTOCORRECT_DATA_H)

$(AUTOCORRECT_DATA_H): $(AUTOCORRECT_DICT_FILE)
	@mkdir -p $(dir $@)
	@$(SILENT) || printf "$(MSG_GENERATING) $@" | $(AWK_CMD)
	$(eval CMD=python3 $(MODULE_PATH_AUTOCORRECT)/make_autocorrect_data.py $< $@)
	@$(BUILD_CMD)
endif
endif
endif
