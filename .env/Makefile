# Modify names below
SRC_DIR := ./src
BLD_DIR := ./build
OBJ_DIR := $(BLD_DIR)/objs/src
DEP_DIR	:= $(BLD_DIR)/deps/src
ENV_DIR := ./env

TARGET := $(BLD_DIR)/$(notdir $(CURDIR)).elf

OPENOCD = "C:\ENV\EmbeddedToolChain\OpenOCD\bin\openocd.exe"
OPENOCD_ARGS = -f "./env/stm32f4discovery.cfg"

COMPILER = GNU
# cpu arch
ARCH = armv7e-m
# cpu core
CPU = cortex-m4

ifeq ($(COMPILER), GNU)
# for GNU
CROSS_COMPILE = arm-none-eabi-
AS  = $(CROSS_COMPILE)as
LD  = $(CROSS_COMPILE)ld
CC  = $(CROSS_COMPILE)gcc
CPP = $(CC) -E
AR  = $(CROSS_COMPILE)ar
NM  = $(CROSS_COMPILE)nm
STRIP    = $(CROSS_COMPILE)strip
OBJCOPY  = $(CROSS_COMPILE)objcopy
OBJDUMP  = $(CROSS_COMPILE)objdump

CFLAGS += -mcpu=$(CPU) -mthumb -mthumb-interwork --specs=nosys.specs --specs=nano.specs

ASFLAGS := $(CFLAGS)
LDFLAGS += -Wl,--print-memory-usage
LDFLAGS += --specs=nosys.specs --specs=nano.specs
else
# for LLVM
CROSS_COMPILE = llvm-
AS  = clang
LD  = ld.lld
CC  = clang
CPP = $(CC) -E
AR  = aarch32-ar
NM  = $(CROSS_COMPILE)nm

STRIP    = $(CROSS_COMPILE)strip
OBJCOPY  = $(CROSS_COMPILE)objcopy
OBJDUMP  = $(CROSS_COMPILE)objdump

# flags
CFLAGS :=	-march=$(ARCH) \
			-mcpu=$(CPU) \
			--target=armv7m-none-eabi \
			-mfloat-abi=hard \
			-mfpu=vfpv4-d16 \
			-mthumb


ASFLAGS :=	-march=$(ARCH) \
			-mcpu=$(CPU) \
			--target=armv7m-none-eabi \
			-mfpu=vfpv4-d16

LDFLAGS += --target=armv7m-none-eabi
endif
## common flag
CFLAGS += -g3 -Os  -Wall -Wextra -Wcast-align -std=c99
CFLAGS += -funsigned-char -ffunction-sections -fdata-sections -fno-common -fomit-frame-pointer

### include path
CFLAGS += -I ./src/Core/Inc
CFLAGS += -I ./src/Core/SpiFlash
CFLAGS += -I ./src/Drivers/HAL_Driver/Inc
CFLAGS += -I ./src/MinRTOS/Inc
# CFLAGS += -I ./src/Drivers/FREE_RTOS/portable/GCC_ARM_CM3
CFLAGS += -I ./src/Drivers/CMSIS/Include
CFLAGS += -I ./src/Drivers/CMSIS/Device/ST/STM32F1xx/Include
### definitions
CFLAGS += -DSTM32F401xC

LDFLAGS += -march=$(ARCH) -mcpu=$(CPU)
LDFLAGS += -T $(ENV_DIR)/STM32F401RCTX_FLASH.ld
LDFLAGS += -Wl,--gc-sections,-Map="$(basename $(TARGET)).map"


SRC_C = $(shell find $(SRC_DIR) -name "*.c")
SRC_S = $(shell find $(SRC_DIR) -name "*.s")

# exclude files
# SRC := $(filter-out src/app/QspiFlash/%, $(SRC))

SRC := $(SRC_C) $(SRC_S)
OBJ := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_C))
OBJ += $(patsubst $(SRC_DIR)/%.s, $(OBJ_DIR)/%.o, $(SRC_S))
DEP := $(patsubst $(SRC_DIR)/%.c, $(DEP_DIR)/%.d, $(SRC_C))

.PHONY : all
all : $(TARGET)

include $(DEP)

$(TARGET) : $(OBJ)
	$(CC) $(LDFLAGS) $^ -o $@
	$(OBJCOPY) $(TARGET) -O ihex $(basename $(TARGET)).hex
	$(OBJCOPY) $(TARGET) -O binary $(basename $(TARGET)).bin
	$(OBJDUMP) -d $(TARGET) > $(basename $(TARGET)).dis

$(DEP_DIR)/%.d: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@$(CC) -MM $(CFLAGS) $< | sed 's,\($(notdir $*)\)\.o[ :]*,$(OBJ_DIR)/$*.o $@ : ,g' > $@

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.s
	@mkdir -p $(dir $@)
	$(CC) $(ASFLAGS) -c $< -o $@

.PHONY : clean
clean :
	-rm -rf $(DEP_DIR) $(OBJ_DIR)

.PHONY : download
download :
	$(OPENOCD) $(OPENOCD_ARGS) -c "program $(TARGET)" -c reset -c shutdown

.PHONY : openocd
openocd :
	$(OPENOCD) $(OPENOCD_ARGS) -c init -c halt

.PHONY : gen
gen :
	make clean
	wsl -e bear -- make -j12
