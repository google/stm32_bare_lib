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
LD := $(CROSS_PREFIX)-ld.bfd
OBJCOPY := $(CROSS_PREFIX)-objcopy

# Debug symbols are enabled with -g, but since we compile ELFs down to bin files, these don't
# affect the code size on-device.
CCFLAGS := -mcpu=cortex-m3 -mthumb -g

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
LDFLAGS := -T stm32_linker_layout.lds

# Rule used when no target is specified.
all: \
$(BINDIR)/examples/blink.bin \
$(BINDIR)/examples/hello_world.bin \
$(BINDIR)/examples/benchmark_arithmetic.bin \
$(BINDIR)/examples/adc_interrupt.bin \
$(BINDIR)/examples/adc_dma.bin \
$(BINDIR)/examples/led_from_mic.bin

clean:
	rm -rf $(GENDIR)

# Generic rules for generating different file types.
$(OBJDIR)%.o: %.c
	@mkdir -p $(dir $@)
	@mkdir -p $(dir $(DEPDIR)$*)
	$(CC) $(CCFLAGS) $(INCLUDES) $(DEPFLAGS) -c $< -o $@
	@mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d

$(OBJDIR)%.o: %.s
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

$(BINDIR)/%.bin: $(ELFDIR)/%.elf
	@mkdir -p $(dir $@)
	$(OBJCOPY) $< $@ -O binary

# Include dependency tracking rules.
$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d
ALL_SRCS := \
$(wildcard examples/*/*.c) \
$(wildcard source/*.c)
-include $(patsubst %,$(DEPDIR)/%.d,$(basename $(ALL_SRCS)))

# Library source files.
# The order of boot.s is important, since it needs to be first in linking
# order, since it has to be at the start of flash memory when the chip is reset
LIBRARY_SRCS := \
$(wildcard source/boot.s) \
$(wildcard source/*.c)
LIBRARY_OBJS := $(addprefix $(OBJDIR), \
$(patsubst %.c,%.o,$(patsubst %.s,%.o,$(LIBRARY_SRCS))))

# Blink example rules.
# The library objects need to be at the start, since the order of boot.s is
# important in the final binary.
BLINK_SRCS := $(wildcard examples/blink/*.c)
BLINK_OBJS := $(LIBRARY_OBJS) \
$(addprefix $(OBJDIR), $(patsubst %.c,%.o,$(patsubst %.s,%.o,$(BLINK_SRCS))))

# Link the blink example.
$(ELFDIR)/examples/blink.elf: $(BLINK_OBJS)
	@mkdir -p $(dir $@)
	$(LD) $(LDFLAGS) -o $@ $(BLINK_OBJS)

# Hello world example rules.
HELLO_WORLD_SRCS := $(wildcard examples/hello_world/*.c)
HELLO_WORLD_OBJS := $(LIBRARY_OBJS) \
$(addprefix $(OBJDIR), $(patsubst %.c,%.o,$(patsubst %.s,%.o,$(HELLO_WORLD_SRCS))))

$(ELFDIR)/examples/hello_world.elf: $(HELLO_WORLD_OBJS)
	@mkdir -p $(dir $@)
	$(LD) $(LDFLAGS) -o $@ $(HELLO_WORLD_OBJS)

# Benchmark arithmetic example rules.
BENCHMARK_ARITHMETIC_SRCS := $(wildcard examples/benchmark_arithmetic/*.c)
BENCHMARK_ARITHMETIC_OBJS := $(LIBRARY_OBJS) \
$(addprefix $(OBJDIR), $(patsubst %.c,%.o,$(patsubst %.s,%.o,$(BENCHMARK_ARITHMETIC_SRCS))))

$(ELFDIR)/examples/benchmark_arithmetic.elf: $(BENCHMARK_ARITHMETIC_OBJS)
	@mkdir -p $(dir $@)
	$(LD) $(LDFLAGS) -o $@ $(BENCHMARK_ARITHMETIC_OBJS)

# Interrupt-driven ADC example.
ADC_INTERRUPT_SRCS := $(wildcard examples/adc_interrupt/*.c)
ADC_INTERRUPT_OBJS := $(LIBRARY_OBJS) \
$(addprefix $(OBJDIR), $(patsubst %.c,%.o,$(patsubst %.s,%.o,$(ADC_INTERRUPT_SRCS))))

$(ELFDIR)/examples/adc_interrupt.elf: $(ADC_INTERRUPT_OBJS)
	@mkdir -p $(dir $@)
	$(LD) $(LDFLAGS) -o $@ $(ADC_INTERRUPT_OBJS)

# ADC example using DMA.
ADC_DMA_SRCS := $(wildcard examples/adc_dma/*.c)
ADC_DMA_OBJS := $(LIBRARY_OBJS) \
$(addprefix $(OBJDIR), $(patsubst %.c,%.o,$(patsubst %.s,%.o,$(ADC_DMA_SRCS))))

$(ELFDIR)/examples/adc_dma.elf: $(ADC_DMA_OBJS)
	@mkdir -p $(dir $@)
	$(LD) $(LDFLAGS) -o $@ $(ADC_DMA_OBJS)

# Light the LED based on audio.
LED_FROM_MIC_SRCS := $(wildcard examples/led_from_mic/*.c)
LED_FROM_MIC_OBJS := $(LIBRARY_OBJS) \
$(addprefix $(OBJDIR), $(patsubst %.c,%.o,$(patsubst %.s,%.o,$(LED_FROM_MIC_SRCS))))

$(ELFDIR)/examples/led_from_mic.elf: $(LED_FROM_MIC_OBJS)
	@mkdir -p $(dir $@)
	$(LD) $(LDFLAGS) -o $@ $(LED_FROM_MIC_OBJS)
