#!/bin/bash


echo "************************ BUILDING LIBOPENJPEG-2.3.0 ************************************"
BASEDIR=`pwd`
echo "starting in $BASERDIR"

cd src/openjpeg-2.3.0
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make

sudo make install
make clean

cd $BASEDIR

echo "************ NOW BUILDING LIBG2C - SUDO PASSWORD MAY BE REQUIRED **************************"

cd src/g2clib-1.6.0
sudo make install
sudo ln -sf /usr/lib/libg2c_v1.6.0.a /usr/lib/libg2c.a
sudo ln -sf /usr/lib/libg2c_v1.6.0.a /usr/lib/x86_64-linux-gnu/libg2c.a

cd $BASEDIR
echo "Back in $BASEDIR"
echo "************ END OF SCRIPT TO BUILD THIRD PARTY LIBS **************************"

