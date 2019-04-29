obj-m+=server.o
KDIR = /home/amirsorouri00/Desktop/linux-source-4.15.0/linux-source-4.15.0
CARGS := -I /lib/modules/$(shell uname -r)/build
all:
	make $(CARGS) -C $(KDIR) M=$(PWD) modules
clean:
	make $(CARGS) -C $(KDIR) M=$(PWD) clean