# shared-memory-sysv

        $ make
        make -I /lib/modules/4.15.0-48-generic/build -C /home/amirsorouri00/Desktop/linux-source-4.15.0/linux-source-4.15.0 M=/home/amirsorouri00/Desktop/Papers/IPC/Linux-Kernel/implementations/kernel_module/shared-memory-sysv modules
        make[1]: Entering directory '/home/amirsorouri00/Desktop/linux-source-4.15.0/linux-source-4.15.0'

        WARNING: Symbol version dump ./Module.symvers
                is missing; modules will have no dependencies and modversions.

        Building modules, stage 2.
        MODPOST 1 modules
        CC      /home/amirsorouri00/Desktop/Papers/IPC/Linux-Kernel/implementations/kernel_module/shared-memory-sysv/server.mod.o
        LD [M]  /home/amirsorouri00/Desktop/Papers/IPC/Linux-Kernel/implementations/kernel_module/shared-memory-sysv/server.ko
        make[1]: Leaving directory '/home/amirsorouri00/Desktop/linux-source-4.15.0/linux-source-4.15.0'
        $ sudo insmod server.ko
        insmod: ERROR: could not insert module server.ko: Invalid module format
