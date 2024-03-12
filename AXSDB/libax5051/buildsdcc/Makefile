TARGET_ASM	:= sdcc-asx8051
TARGET_LD	:= sdcc-sdcc
TARGET_CC	:= sdcc-sdcc
TARGET_AR	:= sdcc-sdcclib
TARGET_MODEL	:= --model-small
TARGET_ASMFLAGS	:= -xlosg
TARGET_LNKFLAGS	:= -mmcs51 --debug --code-loc 0x0000 --code-size 0x2000 --xram-loc 0x0000 --xram-size 0x100 $(TARGET_MODEL) --data-loc 0x08
TARGET_CFLAGS	:= --debug -I../source -I../../libmf/source $(TARGET_LNKFLAGS)
LIBSUFFIX	:=

LIBAX5051OBJ	:= pllrange.rel

BINARIES	:=

all:				libax5051.lib libax5051large.lib $(BINARIES) $(patsubst %.omf,%.cdb,$(BINARIES)) $(patsubst %.omf,%.ihx,$(BINARIES))

clean:
				rm -rf *.lnk libax5051.lib libax5051 libax5051large.lib libax5051large libax5051binsdcc.tar.gz libax5051src.tar.gz

tar:				libax5051binsdcc.tar.gz libax5051src.tar.gz

libax5051binsdcc.tar.gz:	libax5051.lib libax5051large.lib
				tar -c -v -z -f $@ libax5051.lib libax5051large.lib

libax5051src.tar.gz:
				cd .. ; \
				tar -c -v -z -f buildsdcc/$@ build*/Makefile source/*.c source/*.h doc/LibAX5051.pdf

ifeq ($(LIBSUFFIX),)
libax5051large.lib:
				make TARGET_MODEL=--model-large LIBSUFFIX=large
endif

libax5051$(LIBSUFFIX):
				[ ! -d $@ ] && mkdir -p $@

.PRECIOUS:			%.rel %.omf %.ihx %.cdb

%.rel:				../%.c
				($(TARGET_CC) -c $(TARGET_CFLAGS) -o $@ $<; x=$$?; dn=`dirname $<`/; fn=`basename $< .c`; rm -f $${dn}$${fn}.asm ; exit $${x})

%.omf %.ihx %.cdb:		%.rel libax5051$(LIBSUFFIX).lib
				(libdir=`pwd`; dn=`dirname $<`/; fn=`basename $< .rel`; cd $${dn}; $(TARGET_LD) -V $(TARGET_LNKFLAGS) -L$${libdir} -llibax5051$(LIBSUFFIX) $${fn}.rel; x=$$?; mv $${fn} $${fn}.omf; exit $${x})

libax5051$(LIBSUFFIX)/%.rel:			../source/%.c | libax5051$(LIBSUFFIX)
						($(TARGET_CC) -c $(TARGET_CFLAGS) -o $@ $<; x=$$?; dn=`dirname $<`/; fn=`basename $< .c`; rm -f $${dn}$${fn}.asm ; exit $${x})

libax5051$(LIBSUFFIX).lib:			$(patsubst %.rel,libax5051$(LIBSUFFIX)/%.rel,$(LIBAX5051OBJ)) | libax5051$(LIBSUFFIX)
						$(TARGET_AR) -a $@ $(patsubst %.rel,libax5051$(LIBSUFFIX)/%.rel,$(LIBAX5051OBJ))

libax5051$(LIBSUFFIX)/pllrange.rel:		../source/pllrange.c ../source/libax5051.h | libax5051$(LIBSUFFIX)
