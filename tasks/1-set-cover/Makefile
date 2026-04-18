ALL_INPUTS := $(wildcard all_data/*)
ALL_OUTPUTS := $(patsubst all_data/%, results/%, $(ALL_INPUTS))

INPUTS := $(wildcard data/*)
OUTPUTS := $(patsubst data/%, results/%, $(INPUTS))

TIME := /usr/bin/time -f "time: %E"
CHECKER := ./venv/bin/python3 ./src/checker.py

BUILD_DIR := build-release

CMAKE := @cmake --build $(BUILD_DIR) --target

main: $(OUTPUTS)

results:
	mkdir -p $@

results/%: data/% | results venv $(BUILD_DIR)
	$(CMAKE) main
	$(TIME) $(BUILD_DIR)/main $< < $< > $@
	$(CHECKER) results
	cp ./results/verdict.txt .



GREEDY_OUTPUTS := $(patsubst data/%, results_greedy/%, $(INPUTS))

greedy: $(GREEDY_OUTPUTS)

results_greedy:
	mkdir -p $@

results_greedy/%: data/% | results_greedy venv $(BUILD_DIR)
	$(CMAKE) greedy
	$(TIME) $(BUILD_DIR)/greedy $< < $< > $@
	$(CHECKER) results_greedy




BEAM_OUTPUTS := $(patsubst data/%, results_beam/%, $(INPUTS))

beam: $(BEAM_OUTPUTS)

results_beam:
	mkdir -p $@

results_beam/%: data/% | results_beam venv $(BUILD_DIR)
	$(CMAKE) beam
	$(TIME) $(BUILD_DIR)/beam $< < $< > $@
	$(CHECKER) results_beam




BASIC_LINPROG_OUTPUTS := $(patsubst data/%, results_basic_linprog/%, $(INPUTS))

basic_linprog: $(BASIC_LINPROG_OUTPUTS)

results_basic_linprog:
	mkdir -p $@

results_basic_linprog/%: data/% | results_basic_linprog venv $(BUILD_DIR)
	$(CMAKE) basic_linprog
	$(TIME) $(BUILD_DIR)/basic_linprog $< < $< > $@
	$(CHECKER) results_basic_linprog




GREEDY_LINPROG_OUTPUTS := $(patsubst data/%, results_greedy_linprog/%, $(INPUTS))

greedy_linprog: $(GREEDY_LINPROG_OUTPUTS)

results_greedy_linprog:
	mkdir -p $@

results_greedy_linprog/%: data/% | results_greedy_linprog venv $(BUILD_DIR)
	$(CMAKE) greedy_linprog
	$(TIME) $(BUILD_DIR)/greedy_linprog $< < $< > $@
	$(CHECKER) results_greedy_linprog




all_scored: main greedy beam basic_linprog greedy_linprog

full_eval: $(ALL_OUTPUTS)

venv:
	python3 -m venv venv
	venv/bin/pip install -r requirements.txt

build-release:
	cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release

build-debug:
	cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Debug

clean:
	rm -r results*

.PHONY: main greedy beam basic_linprog greedy_linprog all_scored full_eval clean
