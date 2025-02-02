


PROJECT = megatron
BUILD_DIR = bin

#LDSCRIPT = bluepill.ld
LDFLAGS += bluepill.ld

SHARED_DIR = ../common

CFILES = main.c

CFILES += common.c north_port.c south_port.c test_ports.c
#CFILES += dwt_stm32_delay.c 
CFILES += stm32_sw_i2c.c 
CFILES += blocknet.c 

#AFILES += api-asm.S


###################################
# TODO - you will need to edit these  
DEVICE=stm32f1
OOCD_INTERFACE=/usr/share/openocd/scripts/interface/stlink.cfg
OOCD_TARGET=/usr/share/openocd/scripts/target/stm32f1x.cfg
# OOCD_FILE - eg my.openocd.cfg
#    This overrides interface/target above, and is used as just -f FILE


###################################


# You shouldn't have to edit anything below here.
VPATH += $(SHARED_DIR)
INCLUDES += $(patsubst %,-I%, . $(SHARED_DIR))
OPENCM3_DIR=../libopencm3

include $(OPENCM3_DIR)/mk/genlink-config.mk
include ../rules.mk
include $(OPENCM3_DIR)/mk/genlink-rules.mk


.PHONY: program
program:
	st-flash write $(PROJECT).bin 0x8000000 


.PHONY: clearchip
clearchip:
	st-flash erase 


