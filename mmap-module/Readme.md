# mmap-module

## Server
        $ make
        make -I /lib/modules/4.15.0-48-generic/build -C /lib/modules/4.15.0-48-generic/build M=/home/amirsorouri00/Desktop/Papers/IPC/Linux-Kernel/implementations/kernel_module/mmap-module modules
        make[1]: Entering directory '/usr/src/linux-headers-4.15.0-48-generic'
        CC [M]  /home/amirsorouri00/Desktop/Papers/IPC/Linux-Kernel/implementations/kernel_module/mmap-module/mmap.o
        Building modules, stage 2.
        MODPOST 1 modules
        LD [M]  /home/amirsorouri00/Desktop/Papers/IPC/Linux-Kernel/implementations/kernel_module/mmap-module/mmap.ko
        make[1]: Leaving directory '/usr/src/linux-headers-4.15.0-48-generic'
        $ sudo cp mmap.ko /lib/modules/4.15.0-48-generic/
        $ sudo depmod -a
        $ modprobe mmap

## Client
        $ cc user-mmap.c -o user-mmap.out
        $ ./user-mmap.out mmapfile.txt
        
done:))