KDIR := /lib/modules/$(shell uname -r)/build

obj-m += src/my_timer.o

all:
	$(MAKE) -C $(KDIR) M=$(PWD) src=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) src=$(PWD) clean
