KLIN ?= /workspace/bin/klin.dart

.PHONY: all clean pp run

all: run

run:
	dart run $(KLIN) run app.kl

pp:
	mkdir -p out
	dart run $(KLIN) --emit-pp app.kl
	@echo "ok: out/app.pp.kl"

clean:
	rm -rf out
