KLIN ?= /workspace/bin/klin.dart

.PHONY: all clean run emit

all: run

run:
	dart run $(KLIN) run flag_wait.kl

emit:
	mkdir -p out
	dart run $(KLIN) --emit-c flag_wait.kl
	@echo "ok: out/flag_wait.c"

clean:
	rm -rf out
