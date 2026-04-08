ALL_INPUTS := $(wildcard data/*)
ALL_OUTPUTS := $(patsubst data/%, results/%, $(ALL_INPUTS))

INPUTS := $(wildcard data/_scored*)
OUTPUTS := $(patsubst data/%, results/%, $(INPUTS))

TIME := /usr/bin/time -f "time: %E"
CHECKER := ./venv/bin/python3 ./src/checker.py

main: $(OUTPUTS)

results:
	mkdir -p $@

results/%: data/% | results venv
	@cmake --build build --target main
	$(TIME) build/main $< < $< > $@
	$(CHECKER) results
	cp ./results/report.txt .



GREEDY_OUTPUTS := $(patsubst data/%, results_greedy/%, $(INPUTS))

greedy: $(GREEDY_OUTPUTS)

results_greedy:
	mkdir -p $@

results_greedy/%: data/% | results_greedy venv
	@cmake --build build --target greedy
	$(TIME) build/greedy $< < $< > $@
	$(CHECKER) results_greedy




BEAM_OUTPUTS := $(patsubst data/%, results_beam/%, $(INPUTS))

beam: $(BEAM_OUTPUTS)

results_beam:
	mkdir -p $@

results_beam/%: data/% | results_beam venv
	@cmake --build build --target beam
	$(TIME) build/beam $< < $< > $@
	$(CHECKER) results_beam




BASIC_LINPROG_OUTPUTS := $(patsubst data/%, results_basic_linprog/%, $(INPUTS))

basic_linprog: $(BASIC_LINPROG_OUTPUTS)

results_basic_linprog:
	mkdir -p $@

results_basic_linprog/%: data/% | results_basic_linprog venv
	@cmake --build build --target basic_linprog
	$(TIME) build/basic_linprog $< < $< > $@
	$(CHECKER) results_basic_linprog




GREEDY_LINPROG_OUTPUTS := $(patsubst data/%, results_greedy_linprog/%, $(INPUTS))

greedy_linprog: $(GREEDY_LINPROG_OUTPUTS)

results_greedy_linprog:
	mkdir -p $@

results_greedy_linprog/%: data/% | results_greedy_linprog venv
	@cmake --build build --target greedy_linprog
	$(TIME) build/greedy_linprog $< < $< > $@
	$(CHECKER) results_greedy_linprog




all_scored: main greedy beam basic_linprog greedy_linprog

full_eval: $(ALL_OUTPUTS)

venv:
	python3 -m venv venv
	venv/bin/pip install -r requirements.txt

clean:
	rm -r results*

.PHONY: main greedy beam basic_linprog greedy_linprog all_scored full_eval clean
