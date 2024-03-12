TARGET_ASM	:= sdcc-asx8051
TARGET_LD	:= sdcc-sdcc
TARGET_CC	:= sdcc-sdcc
TARGET_AR	:= sdcc-sdcclib
TARGET_MODEL	:= --model-small
TARGET_ASMFLAGS	:= -xlosg
TARGET_LNKFLAGS	:= -mmcs51 --debug --code-loc 0x0000 --code-size 0x2000 --xram-loc 0x0000 --xram-size 0x100 $(TARGET_MODEL) --data-loc 0x08 --stack-auto
TARGET_CFLAGS	:= --debug -I../source -I../../libmf/source $(TARGET_LNKFLAGS)
LIBSUFFIX	:=

COMMON_FLOAT = \
  _atof.c \
  _schar2fs.c \
  _sint2fs.c \
  _slong2fs.c \
  _uchar2fs.c \
  _uint2fs.c \
  _ulong2fs.c \
  _fs2schar.c \
  _fs2sint.c \
  _fs2slong.c \
  _fs2uchar.c \
  _fs2uint.c \
  _fs2ulong.c \
  _fsadd.c \
  _fsdiv.c \
  _fsmul.c \
  _fssub.c \
  _fseq.c \
  _fsgt.c \
  _fslt.c \
  _fsneq.c \
  fabsf.c \
  frexpf.c \
  ldexpf.c \
  expf.c \
  powf.c \
  sincosf.c \
  sinf.c \
  cosf.c \
  logf.c \
  log10f.c \
  sqrtf.c \
  tancotf.c \
  tanf.c \
  cotf.c \
  asincosf.c \
  asinf.c \
  acosf.c \
  atanf.c \
  atan2f.c \
  sincoshf.c \
  sinhf.c \
  coshf.c \
  tanhf.c \
  floorf.c \
  ceilf.c \
  modff.c \
  errno.c

COMMON_LONG = \
  _divslong.c \
  _modslong.c \
  _modulong.c

SOURCES_FLOAT = $(COMMON_FLOAT) \
  _fscmp.c \
  _fsget1arg.c \
  _fsget2args.c \
  _fsnormalize.c \
  _fsreturnval.c \
  _fsrshift.c \
  _fsswapargs.c \
  _logexpf.c

SOURCES_INT = $(COMMON_INT) \
  _divsint.c \
  _divuint.c \
  _modsint.c \
  _moduint.c \
  _mulint.c

SOURCES_LONG = $(COMMON_LONG) \
  _divulong.c \
  _mullong.c



LIBRINTOBJ	:= $(patsubst %.c,%.rel,$(SOURCES_INT) $(SOURCES_LONG))

LIBRFLOATOBJ	:= $(patsubst %.c,%.rel,$(SOURCES_FLOAT))

BINARIES	:=

all:				librint.lib librfloat.lib $(BINARIES) $(patsubst %.omf,%.cdb,$(BINARIES)) $(patsubst %.omf,%.ihx,$(BINARIES))

clean:
				rm -rf *.lnk librint.lib librint librfloat.lib librfloat libreentbinsdcc.tar.gz libreentsrc.tar.gz

tar:				libreentbinsdcc.tar.gz libreentsrc.tar.gz

libreentbinsdcc.tar.gz:		librint.lib librfloat.lib
				tar -c -v -z -f $@ librint.lib librfloat.lib

libreentsrc.tar.gz:
				cd .. ; \
				tar -c -v -z -f buildsdcc/$@ build*/Makefile

ifeq ($(LIBSUFFIX),)
librintlarge.lib:
				make TARGET_MODEL=--model-large LIBSUFFIX=large
endif

librint$(LIBSUFFIX):
				[ ! -d $@ ] && mkdir -p $@

librfloat$(LIBSUFFIX):
				[ ! -d $@ ] && mkdir -p $@

.PRECIOUS:			%.rel %.omf %.ihx %.cdb

%.rel:				../%.c
				($(TARGET_CC) -c $(TARGET_CFLAGS) -o $@ $<; x=$$?; dn=`dirname $<`/; fn=`basename $< .c`; rm -f $${dn}$${fn}.asm ; exit $${x})

%.omf %.ihx %.cdb:		%.rel librint$(LIBSUFFIX).lib librfloat$(LIBSUFFIX).lib
				(libdir=`pwd`; dn=`dirname $<`/; fn=`basename $< .rel`; cd $${dn}; $(TARGET_LD) -V $(TARGET_LNKFLAGS) -L$${libdir} -llibrint$(LIBSUFFIX) -llibrfloat$(LIBSUFFIX) $${fn}.rel; x=$$?; mv $${fn} $${fn}.omf; exit $${x})

librint$(LIBSUFFIX)/%.rel:	/usr/share/sdcc/lib/src/%.c | librint$(LIBSUFFIX)
				($(TARGET_CC) -c $(TARGET_CFLAGS) -o $@ $<; x=$$?; dn=`dirname $<`/; fn=`basename $< .c`; rm -f $${dn}$${fn}.asm ; exit $${x})

librint$(LIBSUFFIX).lib:	$(patsubst %.rel,librint$(LIBSUFFIX)/%.rel,$(LIBRINTOBJ)) | librint$(LIBSUFFIX)
				$(TARGET_AR) -a $@ $(patsubst %.rel,librint$(LIBSUFFIX)/%.rel,$(LIBRINTOBJ))

librfloat$(LIBSUFFIX)/%.rel:	/usr/share/sdcc/lib/src/%.c | librfloat$(LIBSUFFIX)
				($(TARGET_CC) -c $(TARGET_CFLAGS) -o $@ $<; x=$$?; dn=`dirname $<`/; fn=`basename $< .c`; rm -f $${dn}$${fn}.asm ; exit $${x})

librfloat$(LIBSUFFIX).lib:	$(patsubst %.rel,librfloat$(LIBSUFFIX)/%.rel,$(LIBRFLOATOBJ)) | librfloat$(LIBSUFFIX)
				$(TARGET_AR) -a $@ $(patsubst %.rel,librfloat$(LIBSUFFIX)/%.rel,$(LIBRFLOATOBJ))

