
CC = $(CROSSTOOL_PREFIX)gcc
LD = $(CROSSTOOL_PREFIX)gcc
AS = $(CROSSTOOL_PREFIX)as
RM = rm -rf

OUTPUT_DIR = build

CPPFLAGS += -I"/home/pascal/Dokumente/YourOS/src/Bootloader/kernel" -I"/home/pascal/Dokumente/YourOS/src/Bootloader/kernel/boott"
CFLAGS += -nostdinc -g3 -gdwarf-4 -Wall -Wextra -fmessage-length=0 -m32 -ffreestanding -std=gnu99 -mno-sse
LDFLAGS += -nostartfiles -nodefaultlibs -nostdlib -static -T./bootloader.ld -z max-page-size=0x1000

C_SRCS = $(shell find -name '*.c')
S_SRCS = ./kernel/boott/boot.S

C_OBJS = $(patsubst ./%,$(OUTPUT_DIR)/%,$(C_SRCS:.c=.o))
S_OBJS = $(patsubst ./%,$(OUTPUT_DIR)/%,$(S_SRCS:.S=.o))
OBJS := $(C_OBJS) $(S_OBJS)
DEPS := $(OBJS:.o=.d)

ifeq ($(BUILD_CONFIG), release)
	CFLAGS += -O3
else
	CFLAGS += -Og
endif


.PHONY: all
all: bootloader

.PHONY: release
release:
	$(MAKE) BUILD_CONFIG=$@

.PHONY: bootloader
bootloader: $(OUTPUT_DIR)/bootloader

#Pull in dependency info for *existing* .o files
-include $(DEPS)

$(OUTPUT_DIR)/bootloader: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

$(OUTPUT_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(CPPFLAGS) -MMD -MP -MT $@ -c $< -o $@

$(OUTPUT_DIR)/%.o: %.S
	@mkdir -p $(@D)
	$(AS) -32 -o $@ $<

.PHONY: clean
clean:
	-$(RM) $(OUTPUT_DIR)
