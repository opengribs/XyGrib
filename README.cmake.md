# Cmake build system

## Prerequisities

### macOS

- Install [Homebrew](https://brew.sh)
- Install the requred packages: `brew tap indilib/indi; brew install git cmake libnova openjpeg libpng qt5`

### Linux

- Install the required libraries
  - Ubuntu: `sudo apt-get install build-essential git cmake qt5-default libpng-dev libopenjp2-7-dev libnova-dev libproj-dev zlib1g-dev libbz2-dev`

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
