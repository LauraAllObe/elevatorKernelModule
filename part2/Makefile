SRC_DIR := src
SRC_FILES := $(SRC_DIR)/my_timer.c

OUTPUT_DIR := .

obj-m += my_timer.o

KDIR := /lib/modules/$(shell uname -r)/build

all: mytimer.ko

mytimer.ko: $(SRC_FILES)
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
