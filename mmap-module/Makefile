obj-m+=mmap.o
KDIR := /lib/modules/$(shell uname -r)/build
CARGS := -I /lib/modules/$(shell uname -r)/build
all:
	make $(CARGS) -C $(KDIR) M=$(PWD) modules
clean:
	make $(CARGS) -C $(KDIR) M=$(PWD) clean