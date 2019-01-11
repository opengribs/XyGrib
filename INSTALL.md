# XyGrib ver 1.1.1 - Cmake build system

This includes instructions to download sources and build XyGrib yourself. 

There is an option to use pre-build binaries
for all desktops by downloading the appropriate installer for your system. These are found both in
https://gihub.com/opengribs/XyGrib/releases or on https://opengribs.org in the download section.

## Prerequisities

### macOS

- Install [Homebrew](https://brew.sh)
- Install the requred packages: `brew tap indilib/indi; brew install git cmake libnova openjpeg libpng qt5 proj`

### Linux

- Install the required libraries
  - Ubuntu (recent editions): `sudo apt-get install build-essential git cmake qt5-default libpng-dev libopenjp2-7-dev libnova-dev libproj-dev zlib1g-dev libbz2-dev`
  - Ubuntu Trusty: `sudo apt-get install build-essential git cmake qt5-default libpng-dev libopenjpeg-dev libnova-dev libproj-dev zlib1g-dev libbz2-dev`
  - Fedora 28: `sudo yum install gcc git cmake qt5-devel libpng-devel openjpeg2-devel libnova-devel libproj-devel zlib-devel bzip2-devel
  - openSUSE Tumbleweed: `sudo zypper install gcc git cmake libqt5-qtbase-devel libpng-devel openjpeg2-devel libnova-devel libproj-devel zlib-devel libbz2-devel

## Build and package

### macOS

- Get the source
```
git clone https://github.com/opengribs/XyGrib.git
```
- Build

To build, you'll need to tell CMake where QT is located. This will be something like ```/usr/local/Cellar/qt/5.8.1``` The exact version number may differ, check the version you have installed.
```
mkdir build
cd build
QT5_DIR="/usr/local/Cellar/qt/5.11.1"
cmake .. -DCMAKE_PREFIX_PATH=$QT5_DIR/
make
```
- Package (To be integrated to the cmake build process)
```
cd ..
mv build/src/XyGrib.app .
./mac-package.sh
```

### Linux

- Get the source
```
git clone https://github.com/opengribs/XyGrib.git
```
- Build
```
git clone https://github.com/opengribs/XyGrib.git
mkdir build
cd build
cmake ..
make
```
- Install
```
cmake -DCMAKE_INSTALL_PREFIX=/wherever ..
sudo make install
```
- Package
 - Debian / Ubuntu (This create 1 deb for the app and 1 deb for the supplementary maps)
```
dpkg-buildpackage -b
```

### Windows

- Get the source
```
git clone https://github.com/opengribs/XyGrib.git
```
- Build
```
git clone https://github.com/opengribs/XyGrib.git
mkdir build
cd build
cmake ..
cmake --build .
```
- Package
 - TBD
