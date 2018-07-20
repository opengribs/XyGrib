set(G2CLIB_HDRS
drstemplates.h
grib2.h
gridtemplates.h
pdstemplates.h
)

set(G2CLIB_SRCS
#cmplxpack.c
#compack.c
comunpack.c
dec_jpeg2000.c
dec_png.c
drstemplates.c
#enc_jpeg2000.c
#enc_png.c
#g2_addfield.c
#g2_addgrid.c
#g2_addlocal.c
#g2_create.c
g2_free.c
g2_getfld.c
g2_gribend.c
g2_info.c
g2_miss.c
g2_unpack1.c
g2_unpack2.c
g2_unpack3.c
g2_unpack4.c
g2_unpack5.c
g2_unpack6.c
g2_unpack7.c
gbits.c
getdim.c
getpoly.c
gridtemplates.c
int_power.c
#jpcpack.c
jpcunpack.c
jpeg2000_openjpeg.c
#main.c
#mainhome.c
#misspack.c
#mkieee.c
#pack_gp.c
pdstemplates.c
#pngpack.c
pngunpack.c
rdieee.c
#reduce.c
seekgb.c
#simpack.c
simunpack.c
#specpack.c
specunpack.c
)

add_library(g2clib STATIC ${G2CLIB_SRCS})

target_include_directories (g2clib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

ADD_DEFINITIONS(-DUSE_PNG -DUSE_OPENJPEG -DUSE_JPEG2000 )