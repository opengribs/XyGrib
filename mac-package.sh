#!/usr/bin/env bash

# Creates packages from and already built application and data
# Produces a ZIP-ball if zip is available in $PATH and .pkg if Packages tool is available (https://github.com/packagesdev/packages)

if which macdeployqt >/dev/null; then
  DEPLOY='macdeployqt'
elif [ -f /usr/local/opt/qt5/bin/macdeployqt ]; then
  DEPLOY='/usr/local/opt/qt5/bin/macdeployqt'
else
  DEPLOY="$(find ~/Qt -name macdeployqt|head -n1)"
fi
if [ -z "$DEPLOY" ]; then
  echo "Tool macdeployqt not found, can't continue"
  exit 1
fi
$DEPLOY XyGrib.app
rm -rf data/config
if which zip >/dev/null; then
  echo "ZIP found, creating XyGrib-mac.zip"
  zip -r XyGrib-mac.zip XyGrib.app data > /dev/null
fi
if which packagesbuild >/dev/null; then
  echo "Packages found, creating XyGrib-mac.pkg"
  packagesbuild mac-installer/XyGrib.pkgproj
  mv mac-installer/build/XyGrib.pkg XyGrib-mac.pkg
fi
