TARGET_MODEL	:= 
TARGET_DEFINES	:= 
ifeq ($(OS),Windows_NT)
TARGET_INSTDIR := "C:/Program Files (x86)/GNU Tools ARM Embedded/5.4 2016q3"
else
TARGET_INSTDIR	:= /usr
endif
TARGET_ASM	:= $(TARGET_INSTDIR)/bin/arm-none-eabi-as
TARGET_LD	:= $(TARGET_INSTDIR)/bin/arm-none-eabi-ld
TARGET_CC	:= $(TARGET_INSTDIR)/bin/arm-none-eabi-gcc
TARGET_AR	:= $(TARGET_INSTDIR)/bin/arm-none-eabi-ar
TARGET_RANLIB	:= $(TARGET_INSTDIR)/bin/arm-none-eabi-ranlib
TARGET_ASMFLAGS	:= -mcpu=cortex-m0plus -mthumb
TARGET_LNKFLAGS	:=
TARGET_ARFLAGS	:= --target=elf32-littlearm 
TARGET_CFLAGS	:= -mcpu=cortex-m0plus -mthumb -fno-common -Os -g -Wall $(LIBDEFINES) -I../source -I../../libmf/source -I../../libmf/source/CMSIS_5/CMSIS/Core/Include -I../../libmf/source/CMSIS_5/Device/ARM/ARMCM0plus/Include

LIBAX5043OBJ	:= pllrangea.o pllrangeb.o

LIBBINARIES	:= libax5043.a libax5043f2.a

ifeq ($(LIBBINARY),)
all:					$(LIBBINARIES)

.PHONY:					libax5043.a
.PHONY:					libax5043f2.a

libax5043.a:
					make LIBBINARY="$@" LIBSUFFIX="" LIBDEFINES="-D__AXM0=1"

libax5043f2.a:
					make LIBBINARY="$@" LIBSUFFIX="f2" LIBDEFINES="-D__AXM0F2=1"
else
all:					$(LIBBINARY)
endif

clean:
					rm -rf libax5043 libax5043f2 $(LIBBINARIES) $(patsubst %.o,%,$(LIBBINARIES)) libax5043binarm.tar.gz

tar:					libax5043binarm.tar.gz

libax5043binarm.tar.gz:			$(LIBBINARIES)
					tar -c -v -z -f $@ $(LIBBINARIES)

libax5043$(LIBSUFFIX):
					[ ! -d $@ ] && mkdir -p $@

.PRECIOUS:				%.o

%.o:					../%.c 
					$(TARGET_CC) $(TARGET_CFLAGS) -c -o $@ $<		

%.S:					../%.c
					$(TARGET_CC) $(TARGET_CFLAGS) -S -o $@ $<

libax5043$(LIBSUFFIX)/%.o:		../source/%.c | libax5043$(LIBSUFFIX)
					$(TARGET_CC) $(TARGET_CFLAGS) -c -o $@ $<

libax5043$(LIBSUFFIX)/%.S:		../source/%.c | libax5043$(LIBSUFFIX)
					$(TARGET_CC) $(TARGET_CFLAGS) -S -o $@ $<

libax5043$(LIBSUFFIX)/%.o:		../source/%.S | libax5043$(LIBSUFFIX)
					$(TARGET_ASM) $(TARGET_ASMFLAGS) -o $@ $<

libax5043$(LIBSUFFIX)/%.o:		libax5043$(LIBSUFFIX)/%.c | libax5043$(LIBSUFFIX)
					$(TARGET_CC) $(TARGET_CFLAGS) -c -o $@ $<

libax5043$(LIBSUFFIX)/%.S:		libax5043$(LIBSUFFIX)/%.c | libax5043$(LIBSUFFIX)
					$(TARGET_CC) $(TARGET_CFLAGS) -S -o $@ $<

libax5043$(LIBSUFFIX)/%.o:		%.S | libax5043$(LIBSUFFIX)
					$(TARGET_ASM) $(TARGET_ASMFLAGS) -o $@ $<

ifneq ($(LIBBINARY),)
$(LIBBINARY):				$(patsubst %.o,libax5043$(LIBSUFFIX)/%.o,$(LIBAX5043OBJ)) | libax5043$(LIBSUFFIX)
					$(TARGET_AR) $(TARGET_ARFLAGS) -r $@ $(patsubst %.o,libax5043$(LIBSUFFIX)/%.o,$(LIBAX5043OBJ))
					$(TARGET_RANLIB) $@
endif

libax5043$(LIBSUFFIX)/pllrangea.o:	../source/pllrangea.c ../source/libax5043.h | libax5043$(LIBSUFFIX)

libax5043$(LIBSUFFIX)/pllrangeb.o:	../source/pllrangeb.c ../source/libax5043.h | libax5043$(LIBSUFFIX)
