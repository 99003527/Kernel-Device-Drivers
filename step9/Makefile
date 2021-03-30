obj-m += threads.o

KSRC=/home/ltts/ewsbuild/packages/linux-4.14.224

all:
	make -C ${KSRC} M=${PWD} modules  ARCH=arm CROSS_COMPILE=arm-linux-gnueabi-
clean:
	make -C ${KSRC} M=${PWD} modules  ARCH=arm CROSS_COMPILE=arm-linux-gnueabi-
