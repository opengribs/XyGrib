SHELL=/bin/sh

#  If you want to enable support for PNG or JPEG2000 encoding/decoding,
#  you must specify -DUSE_PNG and/or -DUSE_JPEG2000 in the DEFS variable
#  for the C pre-processor
#  -DUSE_PNG requires libpng.a and libz.a
#                     ( and png.h pngconf.h zconf.h zlib.h include files).
#  -DUSE_JPEG2000 requires libjasper.a
#                     ( and all the jasper/*.h include files).
#
#  In addition, INC must include all directories where the above
#  mentioned include files can be found.
DEFS=-DUSE_OPENJPEG -DUSE_PNG

#
# Please make sure to include all idirectories where include files  
#     can be found (jasper/*.h and png.h pngconf.h zconf.h zlib.h)
#

# INC=-I/nwprod/lib/include/
INC=-I/usr/local/include/openjpeg-2.3

#
#   This "C" source code contains many uses of the C++
#   comment style "//".  Please make sure you include the
#   appropriate compiler flag to allow the use of "//" comment indicators.
#

CFLAGS:= -O3 -g $(INC) $(DEFS)  $(CFLAGS)

PIC:= -fPIC

prefix=/usr
LIBDIR=/usr/lib
SHLIB=libgrib2c.so.0d
SONAME=libgrib2c.so.0d

CC=gcc

LIB=libg2c_v1.6.0.a
ARFLAGS=


all: $(LIB)

$(LIB)(gridtemplates.o): gridtemplates.h

$(LIB)(pdstemplates.o): pdstemplates.h

$(LIB)(drstemplates.o): drstemplates.h

$(LIB):	$(LIB)(gridtemplates.o) \
	$(LIB)(drstemplates.o) \
	$(LIB)(pdstemplates.o) \
	$(LIB)(gbits.o) \
	$(LIB)(g2_unpack1.o) \
	$(LIB)(g2_unpack2.o) \
	$(LIB)(g2_unpack3.o) \
	$(LIB)(g2_unpack4.o) \
	$(LIB)(g2_unpack5.o) \
	$(LIB)(g2_unpack6.o) \
	$(LIB)(g2_unpack7.o) \
	$(LIB)(g2_free.o) \
	$(LIB)(g2_info.o) \
	$(LIB)(g2_getfld.o) \
	$(LIB)(simunpack.o) \
	$(LIB)(comunpack.o) \
        $(LIB)(pack_gp.o) \
        $(LIB)(reduce.o) \
	$(LIB)(specpack.o) \
	$(LIB)(specunpack.o) \
	$(LIB)(rdieee.o) \
	$(LIB)(mkieee.o) \
	$(LIB)(int_power.o) \
	$(LIB)(simpack.o) \
	$(LIB)(compack.o) \
        $(LIB)(cmplxpack.o) \
	$(LIB)(misspack.o) \
	$(LIB)(jpcpack.o) \
	$(LIB)(jpcunpack.o) \
	$(LIB)(pngpack.o) \
	$(LIB)(pngunpack.o) \
	$(LIB)(dec_jpeg2000.o) \
	$(LIB)(enc_jpeg2000.o) \
	$(LIB)(dec_png.o) \
	$(LIB)(enc_png.o) \
	$(LIB)(g2_create.o) \
	$(LIB)(g2_addlocal.o) \
	$(LIB)(g2_addgrid.o) \
	$(LIB)(g2_addfield.o) \
	$(LIB)(g2_gribend.o) \
	$(LIB)(getdim.o) \
	$(LIB)(g2_miss.o) \
	$(LIB)(getpoly.o) \
	$(LIB)(seekgb.o)

.c.a:
	$(CC) -c $(CFLAGS) $(PIC) $<
	ar $(ARFLAGS) -ruv $@ $*.o
	rm -f $*.o

.c.o:
	$(CC)  -c $(CFLAGS) $(PIC) $<

clean:
	rm -f *.o *.a *.so grib2c.pc lib*

LIBS= `pkg-config --libs libpng` -lopenjp2 -lm
OBJS:=  gridtemplates.o \
	 drstemplates.o \
	 pdstemplates.o \
	 gbits.o \
	 g2_unpack1.o \
	 g2_unpack2.o \
	 g2_unpack3.o \
	 g2_unpack4.o \
	 g2_unpack5.o \
	 g2_unpack6.o \
	 g2_unpack7.o \
	 g2_free.o \
	 g2_info.o \
	 g2_getfld.o \
	 simunpack.o \
	 comunpack.o \
	 pack_gp.o \
	 reduce.o \
	 specpack.o \
	 specunpack.o \
         rdieee.o \
	 mkieee.o \
	 int_power.o \
	 simpack.o \
	 compack.o \
	 cmplxpack.o \
	 misspack.o \
	 jpcpack.o \
	 jpcunpack.o \
	 pngpack.o \
	 pngunpack.o \
	 dec_jpeg2000.o \
	 enc_jpeg2000.o \
	 jpeg2000_openjpeg.o \
	 dec_png.o \
	 enc_png.o \
	 g2_create.o \
	 g2_addlocal.o \
	 g2_addgrid.o \
	 g2_addfield.o \
	 g2_gribend.o \
	 getdim.o \
	 g2_miss.o \
	 getpoly.o \
	 seekgb.o


all: $(LIB) $(SHLIB)

$(SHLIB): $(OBJS)
	gcc $(LDFLAGS) -Wl,-as-needed -shared  -o $(SHLIB) -Wl,-soname,$(SONAME) $(OBJS) $(LIBS)

grib2c.pc: grib2c.pc.in
	cat grib2c.pc.in | sed -e 's!@prefix@!${PREFIX}!' | sed -e 's!@libdir@!${LIBDIR}!' > grib2c.pc

install: $(LIB) $(SHLIB) grib2c.pc
	mkdir -p $(LIBDIR)
	cp $(LIB) $(SHLIB) $(LIBDIR)
	mkdir -p $(LIBDIR)/pkgconfig
	cp grib2c.pc $(LIBDIR)/pkgconfig
	mkdir -p $(prefix)/include
	cp grib2.h $(prefix)/include

.PHONY: clean install
