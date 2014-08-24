sources := $(wildcard *.wav)
targets := $(patsubst %.wav,%.opus,$(sources))
all: $(targets)
%.opus: %.wav
	opusenc $< $@
$(targets): Makefile
