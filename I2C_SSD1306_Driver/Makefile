PWD := $(shell pwd)
KERNEL_SRC = /home/chiennguyen/working_space/BBB/bb-kernel/KERNEL

CROSS = /home/chiennguyen/working_space/BBB/bb-kernel/dl/gcc-8.5.0-nolibc/arm-linux-gnueabi/bin/arm-linux-gnueabi-

obj-m += ssd1306.o

all:
	make ARCH=arm CROSS_COMPILE=$(CROSS) -C $(KERNEL_SRC) M=$(PWD) modules

clean:
	make -C $(KERNEL_SRC) M=$(shell pwd) clean

copy:
	scp ssd1306.ko debian@192.168.1.106:/home/debian
	
