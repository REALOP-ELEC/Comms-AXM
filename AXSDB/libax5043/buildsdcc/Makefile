TARGET_ASM	:= sdcc-asx8051
TARGET_LD	:= sdcc-sdcc
TARGET_CC	:= sdcc-sdcc
TARGET_AR	:= sdcc-sdcclib
TARGET_MODEL	:= --model-small
TARGET_ASMFLAGS	:= -xlosg
TARGET_LNKFLAGS	:= -mmcs51 --debug --code-loc 0x0000 --code-size 0x2000 --xram-loc 0x0000 --xram-size 0x100 $(TARGET_MODEL) --data-loc 0x08
TARGET_CFLAGS	:= --debug -I../source -I../../libmf/source $(TARGET_LNKFLAGS)
LIBSUFFIX	:=

LIBAX5043OBJ	:= pllrangea.rel pllrangeb.rel

BINARIES	:=

all:				libax5043.lib libax5043large.lib $(BINARIES) $(patsubst %.omf,%.cdb,$(BINARIES)) $(patsubst %.omf,%.ihx,$(BINARIES))

clean:
				rm -rf *.lnk libax5043.lib libax5043 libax5043large.lib libax5043large libax5043binsdcc.tar.gz libax5043src.tar.gz

tar:				libax5043binsdcc.tar.gz libax5043src.tar.gz

libax5043binsdcc.tar.gz:	libax5043.lib libax5043large.lib
				tar -c -v -z -f $@ libax5043.lib libax5043large.lib

libax5043src.tar.gz:
				cd .. ; \
				tar -c -v -z -f buildsdcc/$@ build*/Makefile source/*.c source/*.h doc/LibAX5043.pdf

ifeq ($(LIBSUFFIX),)
libax5043large.lib:
				make TARGET_MODEL=--model-large LIBSUFFIX=large
endif

libax5043$(LIBSUFFIX):
				[ ! -d $@ ] && mkdir -p $@

.PRECIOUS:			%.rel %.omf %.ihx %.cdb

%.rel:				../%.c
				($(TARGET_CC) -c $(TARGET_CFLAGS) -o $@ $<; x=$$?; dn=`dirname $<`/; fn=`basename $< .c`; rm -f $${dn}$${fn}.asm ; exit $${x})

%.omf %.ihx %.cdb:		%.rel libax5043$(LIBSUFFIX).lib
				(libdir=`pwd`; dn=`dirname $<`/; fn=`basename $< .rel`; cd $${dn}; $(TARGET_LD) -V $(TARGET_LNKFLAGS) -L$${libdir} -llibax5043$(LIBSUFFIX) $${fn}.rel; x=$$?; mv $${fn} $${fn}.omf; exit $${x})

libax5043$(LIBSUFFIX)/%.rel:		../source/%.c | libax5043$(LIBSUFFIX)
					($(TARGET_CC) -c $(TARGET_CFLAGS) -o $@ $<; x=$$?; dn=`dirname $<`/; fn=`basename $< .c`; rm -f $${dn}$${fn}.asm ; exit $${x})

libax5043$(LIBSUFFIX).lib:		$(patsubst %.rel,libax5043$(LIBSUFFIX)/%.rel,$(LIBAX5043OBJ)) | libax5043$(LIBSUFFIX)
					$(TARGET_AR) -a $@ $(patsubst %.rel,libax5043$(LIBSUFFIX)/%.rel,$(LIBAX5043OBJ))

libax5043$(LIBSUFFIX)/pllrangea.rel:	../source/pllrangea.c ../source/libax5043.h | libax5043$(LIBSUFFIX)
libax5043$(LIBSUFFIX)/pllrangeb.rel:	../source/pllrangeb.c ../source/libax5043.h | libax5043$(LIBSUFFIX)
