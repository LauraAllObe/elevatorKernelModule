KDIR := /lib/modules/$(shell uname -r)/build
BUILD_DIR := $(PWD)/build

obj-m := src/elevator.o

all:
	$(MAKE) -C $(KDIR) M=$(BUILD_DIR) src=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(BUILD_DIR) src=$(PWD) clean
