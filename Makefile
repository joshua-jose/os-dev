ARCHTUPLE=

AS=$(ARCHTUPLE)as
LD=$(ARCHTUPLE)ld
CC=$(ARCHTUPLE)g++

ROOT=.
SRCDIR=$(ROOT)/src
BINDIR=$(ROOT)/bin

IMAGE=os.flp

# filename extensions
CEXTS:=c
ASMEXTS:=s S
CXXEXTS:=cpp c++ cc

LDFLAGS=--oformat binary -Ttext 0x7c00 -e _start
ASFLAGS=-I $(SRCDIR)/boot
CCFLAGS=-m64 -ffreestanding -mno-red-zone -O2 -fno-exceptions -fno-rtti -mno-mmx -mno-sse -mno-sse2 -fno-stack-protector -nostdlib -static

.DEFAULT_GOAL=run
.PHONY: build clean all

rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

#KASMSRC=$(foreach asmext,$(ASMEXTS),$(call rwildcard, $(SRCDIR)/kernel,*.$(asmext), $1))
#KASMOBJ=$(addprefix $(BINDIR)/,$(patsubst $(SRCDIR)/kernel/%,%.o,$(call ASMSRC,$1)))

CXXSRC=$(foreach cxxext,$(CXXEXTS),$(call rwildcard, $(SRCDIR),*.$(cxxext), $1))
CXXOBJ=$(addprefix $(BINDIR)/,$(patsubst $(SRCDIR)/%,%.o,$(call CXXSRC,$1)))

BOOTSRC=$(foreach asmext,$(ASMEXTS),$(call rwildcard, $(SRCDIR)/boot,*.$(asmext), $1))
BOOTOBJ=$(addprefix $(BINDIR)/,$(patsubst $(SRCDIR)/%,%.o,$(call BOOTSRC,$1)))

vpath $(SRCDIR):$(SRCDIR)/**

$(BINDIR)/%.s.o: $(SRCDIR)/%.s
	@echo Compiling assembly file $< $@
	@mkdir -p $(dir $@)
	@$(AS) $(ASFLAGS) -I $(dir $<) -o $@ $<

$(BINDIR)/%.cpp.o: $(SRCDIR)/%.cpp
	@echo Compiling C++ file $< $@
	@mkdir -p $(dir $@)
	@$(CC) $(CCFLAGS) -o $@ $< 

$(IMAGE): $(BOOTOBJ) $(CXXOBJ)

	@$(LD) -o bin/boot.bin $(LDFLAGS) $(BOOTOBJ)
	@$(CC) $(CCFLAGS) -T klink.ld $(CXXOBJ) -o bin/kernel.bin

	@cat bin/boot.bin bin/kernel.bin > bin/os.bin

	@dd if=/dev/zero of=$(IMAGE) bs=512 count=2880 status=none
	@dd if=bin/os.bin of=$(IMAGE) conv=notrunc status=none

	@rm bin/boot.bin bin/kernel.bin bin/os.bin

build: $(IMAGE)

# Debug: @qemu-system-x86_64 -drive file=$(IMAGE),format=raw -d cpu,exec,in_asm -nographic
run: $(IMAGE)
	@qemu-system-x86_64 -drive file=$(IMAGE),format=raw -curses

clean:
	@rm -rf $(BINDIR)
	@rm -f $(IMAGE)

all: clean build