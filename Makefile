# Makefile for all the examples in the STM32 Bare Library.

# Override this if you want to store temporary files outside of the source folder.
GENDIR := ./gen/

# Sub-directories holding generated files.
OBJDIR := $(GENDIR)/obj/
ELFDIR := $(GENDIR)/elf/
BINDIR := $(GENDIR)/bin/
DEPDIR := $(GENDIR)/dep/

# The cross-compilation toolchain prefix to use for gcc binaries.
CROSS_PREFIX := arm-none-eabi
AS := $(CROSS_PREFIX)-as
CC := $(CROSS_PREFIX)-gcc
CPP := $(CROSS_PREFIX)-g++
LD := $(CROSS_PREFIX)-gcc
OBJCOPY := $(CROSS_PREFIX)-objcopy

OPTFLAGS := -O3
# Debug symbols are enabled with -g, but since we compile ELFs down to bin files, these don't
# affect the code size on-device.
BASE_COMPILER_FLAGS := -mcpu=cortex-m3 -mthumb -std=gnu99 -g -gdwarf-2 $(OPTFLAGS)
CCFLAGS:= $(BASE_COMPILER_FLAGS)
CPPFLAGS:= $(BASE_COMPILER_FLAGS)

# Used to rebuild when headers used by a source file change.
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td

# We rely on headers from Arm's CMSIS library for things like device register layouts. To
# download the library, use `git clone https://github.com/ARM-software/CMSIS_5` in the parent
# folder of the one this Makefile is in (not this folder, but the one above).
CMSIS_DIR :=../CMSIS_5/
ifeq ($(shell test -d $(CMSIS_DIR) ; echo $$?), 1)
  $(error "CMSIS not found at '$(CMSIS_DIR)' - try 'git clone https://github.com/ARM-software/CMSIS_5 $(CMSIS_DIR)'")
endif

# Allow CMSIS core headers, and ones from this library.
INCLUDES := \
-isystem$(CMSIS_DIR)/CMSIS/Core/Include/ \
-I./include 

ASFLAGS :=

# Defines the offsets used when linking binaries for the STM32.

LDFLAGS := -T stm32_linker_layout.lds  -Wl,-Map=gen/$(TARGET).map,--cref -Wl,--gc-sections

# Use this to do libc
# LDFLAGS += -specs rdimon.specs -lc -lm -lrdimon -mfloat-abi=soft



# Library source files.
# The order of boot.s is important, since it needs to be first in linking
# order, since it has to be at the start of flash memory when the chip is reset
LIBRARY_SRCS := \
$(wildcard source/*.c)
LIBRARY_OBJS := $(addprefix $(OBJDIR), \
$(patsubst %.c,%.o,$(patsubst %.s,%.o,$(LIBRARY_SRCS))))

EXAMPLES_FOLDERS := $(wildcard examples/*)
EXAMPLES_NAMES := $(notdir $(EXAMPLES_FOLDERS))
EXAMPLES_BINS := $(patsubst %, $(BINDIR)/examples/%.bin, $(EXAMPLES_NAMES))

# Rule used when no target is specified.
all: $(EXAMPLES_BINS)

clean:
	rm -rf $(GENDIR)

# Generic rules for generating different file types.
$(OBJDIR)%.o: %.c
	@mkdir -p $(dir $@)
	@mkdir -p $(dir $(DEPDIR)$*)
	$(CC) $(CCFLAGS) $(INCLUDES) $(DEPFLAGS) -c $< -o $@
	@mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d

$(OBJDIR)%.o: %.cc
	@mkdir -p $(dir $@)
	@mkdir -p $(dir $(DEPDIR)$*)
	$(CPP) $(CPPFLAGS) $(INCLUDES) $(DEPFLAGS) -c $< -o $@
	@mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d

$(OBJDIR)%.o: %.s
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

$(BINDIR)/%.bin: $(ELFDIR)/%.elf
	@mkdir -p $(dir $@)
	$(OBJCOPY) $< $@ -O binary

# Loop through all of the example folders and create a rule to build each .elf
# file automatically.
define BUILD_EXAMPLE_ELF
$(1): $(2)
	@mkdir -p $(dir $(1))
	$(LD) $(LDFLAGS) -o $(1) $(2)
endef
$(foreach name,$(EXAMPLES_NAMES),\
$(eval $(call BUILD_EXAMPLE_ELF,\
$(ELFDIR)/examples/$(name).elf,\
$(LIBRARY_OBJS) $(patsubst %.c,$(OBJDIR)%.o,$(wildcard examples/$(name)/*.c)))))

# Include dependency tracking rules.
$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d
ALL_SRCS := \
$(wildcard examples/*/*.c) \
$(wildcard source/*.c)
-include $(patsubst %,$(DEPDIR)/%.d,$(basename $(ALL_SRCS)))
