ARCHTUPLE=~/x-tools/x86_64-elf/bin/x86_64-elf-
#~/x-tools/x86_64-elf/bin/x86_64-elf-

AS=$(ARCHTUPLE)as
LD=$(ARCHTUPLE)ld
CC=$(ARCHTUPLE)g++

ROOT=$(shell pwd)
SRCDIR=$(ROOT)/src
BINDIR=$(ROOT)/bin
INCDIR=$(ROOT)/include

IMAGE=$(ROOT)/os.flp

# filename extensions
CEXTS:=c
ASMEXTS:=s S
CXXEXTS:=cpp c++ cc c

LDFLAGS=--oformat binary -Ttext 0x7c00
ASFLAGS=-I $(SRCDIR)/boot
CCFLAGS=-m64 -ffreestanding -std=gnu++14 -mno-red-zone -O2 -fno-exceptions -fno-rtti -mno-mmx -mgeneral-regs-only
CCFLAGS+= -mno-sse -mno-sse2 -fno-stack-protector -static -Wall -Wextra -lgcc -I $(INCDIR)

.DEFAULT_GOAL=run
.PHONY: build clean all
.ONESHELL:

rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

#KASMSRC=$(foreach asmext,$(ASMEXTS),$(call rwildcard, $(SRCDIR)/kernel,*.$(asmext), $1))
#KASMOBJ=$(addprefix $(BINDIR)/,$(patsubst $(SRCDIR)/kernel/%,%.o,$(call ASMSRC,$1)))

CXXSRC=$(foreach cxxext,$(CXXEXTS),$(call rwildcard, $(SRCDIR),*.$(cxxext), $1))
CXXOBJ=$(addprefix $(BINDIR)/,$(patsubst $(SRCDIR)/%,%.o,$(call CXXSRC,$1)))

BOOTSRC=$(foreach asmext,$(ASMEXTS),$(call rwildcard, $(SRCDIR)/boot,*.$(asmext), $1))
BOOTOBJ=$(addprefix $(BINDIR)/,$(patsubst $(SRCDIR)/%,%.o,$(call BOOTSRC,$1)))

vpath $(SRCDIR):$(SRCDIR)/**

$(BINDIR)/%.s.o: $(SRCDIR)/%.s
	@echo Compiling assembly file $(notdir $<) to $(notdir $@)
	@mkdir -p $(dir $@)
	@$(AS) $(ASFLAGS) -c -I $(dir $<) -o $@ $<

$(BINDIR)/%.cpp.o: $(SRCDIR)/%.cpp
	@echo Compiling C++ file $(notdir $<) to $(notdir $@)
	@mkdir -p $(dir $@)
	
	@$(CC) $(CCFLAGS) -c -I $(subst src/,include/, $(dir $<)) -o $@ $< 

$(BINDIR)/%.c.o: $(SRCDIR)/%.c
	@echo Compiling C file $(notdir $<) to $(notdir $@)
	@mkdir -p $(dir $@)
	
	@$(CC) $(CCFLAGS) -c -I $(subst src/,include/, $(dir $<)) -o $@ $< 

$(IMAGE): $(BOOTOBJ) $(CXXOBJ)
	@cd bin
	@echo Linking...
	@$(LD) -o $(BINDIR)/boot.bin $(LDFLAGS) $(BOOTOBJ)
	
	@cp $(BINDIR)/kernel/crt0.cpp.o crt0.o
	@$(CC) $(CCFLAGS) -T $(ROOT)/klink.ld $(CXXOBJ) -o $(BINDIR)/kernel.bin -I $(dir $<)

	@cat $(BINDIR)/boot.bin $(BINDIR)/kernel.bin > $(BINDIR)/os.bin

	@dd if=/dev/zero of=$(IMAGE) bs=512 count=2880 status=none
	@dd if=$(BINDIR)/os.bin of=$(IMAGE) conv=notrunc status=none

build: $(IMAGE)

# Debug: @qemu-system-x86_64 -drive file=$(IMAGE),format=raw -d cpu,exec,in_asm -nographic
run: $(IMAGE)
	@qemu-system-x86_64 -drive file=$(IMAGE),format=raw -curses

clean:
	@rm -rf $(BINDIR)
	@rm -f $(IMAGE)

all: clean build