# hello-world

        $ make
        make -C /lib/modules/4.15.0-47-generic/build/ M=/home/amirsorouri00/Desktop/Papers/IPC/ modules
            make[1]: Entering directory '/usr/src/linux-headers-4.15.0-47-generic'
            CC [M]  /home/amirsorouri00/Desktop/Papers/IPC//hello.o
            Building modules, stage 2.
            MODPOST 1 modules
            CC      /home/amirsorouri00/Desktop/Papers/IPC//hello.mod.o
            LD [M]  /home/amirsorouri00/Desktop/Papers/IPC//hello.ko
        make[1]: Leaving directory '/usr/src/linux-headers-4.15.0-47-generic'
        $ sudo insmod hello.ko
