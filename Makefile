ARCHTUPLE=

AS=$(ARCHTUPLE)as
LD=$(ARCHTUPLE)ld

ROOT=.
SRCDIR=$(ROOT)/src
BINDIR=$(ROOT)/bin

IMAGE=os.bin

# filename extensions
CEXTS:=c
ASMEXTS:=s S
CXXEXTS:=cpp c++ cc

LDFLAGS=--oformat binary -Ttext 0x7c00 -e _start
ASFLAGS=-I $(SRCDIR)/boot

.DEFAULT_GOAL=run
.PHONY: build clean all

rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

ASMSRC=$(foreach asmext,$(ASMEXTS),$(call rwildcard, $(SRCDIR),*.$(asmext), $1))
ASMOBJ=$(addprefix $(BINDIR)/,$(patsubst $(SRCDIR)/%,%.o,$(call ASMSRC,$1)))

vpath $(SRCDIR):$(SRCDIR)/**

$(BINDIR)/%.s.o: $(SRCDIR)/%.s
	@echo File $< $@
	@mkdir -p $(dir $@)
	@$(AS) $(ASFLAGS) -o $@ $<

$(IMAGE): $(ASMOBJ)
	@$(LD) $(LDFLAGS) -o $(IMAGE) $(ASMOBJ)

build: $(IMAGE)

run: $(IMAGE)
	@qemu-system-x86_64 -drive file=$(IMAGE),format=raw -curses

clean:
	@rm -rf $(BINDIR)
	@rm -f $(IMAGE)

all: clean build