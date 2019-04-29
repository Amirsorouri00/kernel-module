obj-m+=server.o
CARGS = /home/amirsorouri00/Desktop/linux-source-4.15.0/linux-source-4.15.0
KDIR := -I /lib/modules/$(shell uname -r)/build
all:
	make $(KDIR) -C $(CARGS) M=$(PWD) modules
clean:
	make $(CARGS) -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean