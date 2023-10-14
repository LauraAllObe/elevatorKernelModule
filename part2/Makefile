SRC_DIR := src
SRC_FILES := $(SRC_DIR)/my_timer.c

obj-m += mytimer.o

KDIR := /lib/modules/$(shell uname -r)/build

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
