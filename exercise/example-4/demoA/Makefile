arch ?= x86
modname ?= demo

ifeq ($(arch),arm)
	KERNELDIR := /home/ming/linux-5.10.61/
else
	KERNELDIR := /lib/modules/$(shell uname -r)/build/
endif

PWD := $(shell pwd)

all:
	make -C $(KERNELDIR) M=$(PWD) modules
# 进入内核源代码目录下，读取内核的Makefile(顶层)
# 执行make M=$(PWD) modules
# make modules 表示编译模块，编译M变量指定路径下的模块
clean:
	make -C $(KERNELDIR) M=$(PWD) clean

# 指定编译的模块的名字
obj-m:=$(modname).o 