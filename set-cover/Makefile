INPUTS := $(wildcard data/*)
OUTPUTS := $(patsubst data/%, results/%, $(INPUTS))

SCORED_INPUTS := $(wildcard data/_scored*)
SCORED_OUTPUTS := $(patsubst data/%, results/%, $(SCORED_INPUTS))

SMALL_INPUTS := $(wildcard data/sc_???_*)
SMALL_OUTPUTS := $(patsubst data/%, results/%, $(SMALL_INPUTS))

scored: $(SCORED_OUTPUTS)

small: $(SMALL_OUTPUTS)

all: $(OUTPUTS)

results:
	mkdir -p $@

results/%: data/% | results
	@cmake --build build --target set_cover
	time build/set_cover < $< > $@

clean:
	rm -r results

.PHONY: scored small all clean