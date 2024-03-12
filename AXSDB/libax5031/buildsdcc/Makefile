TARGET_ASM	:= sdcc-asx8051
TARGET_LD	:= sdcc-sdcc
TARGET_CC	:= sdcc-sdcc
TARGET_AR	:= sdcc-sdcclib
TARGET_MODEL	:= --model-small
TARGET_ASMFLAGS	:= -xlosg
TARGET_LNKFLAGS	:= -mmcs51 --debug --code-loc 0x0000 --code-size 0x2000 --xram-loc 0x0000 --xram-size 0x100 $(TARGET_MODEL) --data-loc 0x08
TARGET_CFLAGS	:= --debug -I../source -I../../libmf/source $(TARGET_LNKFLAGS)
LIBSUFFIX	:=

LIBAX5031OBJ	:= pllrange.rel

BINARIES	:=

all:				libax5031.lib libax5031large.lib $(BINARIES) $(patsubst %.omf,%.cdb,$(BINARIES)) $(patsubst %.omf,%.ihx,$(BINARIES))

clean:
				rm -rf *.lnk libax5031.lib libax5031 libax5031large.lib libax5031large libax5031binsdcc.tar.gz libax5031src.tar.gz

tar:				libax5031binsdcc.tar.gz libax5031src.tar.gz

libax5031binsdcc.tar.gz:	libax5031.lib libax5031large.lib
				tar -c -v -z -f $@ libax5031.lib libax5031large.lib

libax5031src.tar.gz:
				cd .. ; \
				tar -c -v -z -f buildsdcc/$@ build*/Makefile source/*.c source/*.h doc/LibAX5031.pdf

ifeq ($(LIBSUFFIX),)
libax5031large.lib:
				make TARGET_MODEL=--model-large LIBSUFFIX=large
endif

libax5031$(LIBSUFFIX):
				[ ! -d $@ ] && mkdir -p $@

.PRECIOUS:			%.rel %.omf %.ihx %.cdb

%.rel:				../%.c
				($(TARGET_CC) -c $(TARGET_CFLAGS) -o $@ $<; x=$$?; dn=`dirname $<`/; fn=`basename $< .c`; rm -f $${dn}$${fn}.asm ; exit $${x})

%.omf %.ihx %.cdb:		%.rel libax5031$(LIBSUFFIX).lib
				(libdir=`pwd`; dn=`dirname $<`/; fn=`basename $< .rel`; cd $${dn}; $(TARGET_LD) -V $(TARGET_LNKFLAGS) -L$${libdir} -llibax5031$(LIBSUFFIX) $${fn}.rel; x=$$?; mv $${fn} $${fn}.omf; exit $${x})

libax5031$(LIBSUFFIX)/%.rel:		../source/%.c | libax5031$(LIBSUFFIX)
					($(TARGET_CC) -c $(TARGET_CFLAGS) -o $@ $<; x=$$?; dn=`dirname $<`/; fn=`basename $< .c`; rm -f $${dn}$${fn}.asm ; exit $${x})

libax5031$(LIBSUFFIX).lib:		$(patsubst %.rel,libax5031$(LIBSUFFIX)/%.rel,$(LIBAX5031OBJ)) | libax5031$(LIBSUFFIX)
					$(TARGET_AR) -a $@ $(patsubst %.rel,libax5031$(LIBSUFFIX)/%.rel,$(LIBAX5031OBJ))

libax5031$(LIBSUFFIX)/pllrange.rel:	../source/pllrange.c ../source/libax5031.h | libax5031$(LIBSUFFIX)
