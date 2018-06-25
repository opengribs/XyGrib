# Cmake build system

## Prerequisities

### macOS

- Install [Homebrew](https://brew.sh)
- Install the requred packages: `brew tap indilib/indi; brew install git cmake libnova openjpeg libpng qt5`

### Linux

- Install the required libraries
  - Ubuntu (recent editions): `sudo apt-get install build-essential git cmake qt5-default libpng-dev libopenjp2-7-dev libnova-dev libproj-dev zlib1g-dev libbz2-dev`
  - Ubuntu Trusty: `sudo apt-get install build-essential git cmake qt5-default libpng-dev libopenjpeg-dev libnova-dev libproj-dev zlib1g-dev libbz2-dev`
  - Fedora 28: `sudo yum install gcc git cmake qt5-devel libpng-devel openjpeg2-devel libnova-devel libproj-devel zlib-devel bzip2-devel
  - openSUSE Tumbleweed: `sudo zypper install gcc git cmake libqt5-qtbase-devel libpng-devel openjpeg2-devel libnova-devel libproj-devel zlib-devel libbz2-devel

### Windows

- TBD

## Build and package

### macOS

- Get the source
```
git clone https://github.com/opengribs/XyGrib.git
```
- Build
```
mkdir build
cd build
cmake ..
make
```
- Package (To be integrated to the cmake build process)
```
cd ..
mv src/build/XyGrib.app .
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
sudo build install
```
- Package
 - TBD

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
