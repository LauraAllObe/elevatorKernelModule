MODULE_NAME := my_timer

SRC_DIR := src
SRC_FILES := $(SRC_DIR)/$(MODULE_NAME).c

OUTPUT_DIR := .

obj-m := $(MODULE_NAME).o

KDIR := /lib/modules/$(shell uname -r)/build

all: $(MODULE_NAME).ko

$(MODULE_NAME).ko: $(SRC_DIR)/$(MODULE_NAME).c
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean


.PHONY: all clean
