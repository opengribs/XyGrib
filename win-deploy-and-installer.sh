#!/usr/bin/env bash

# Creates installer from an already built application and data
# Assumes this script being copied to the root of 'qtbuild' in the following folder structure:

# -- qtbuild 
#		-- deploy 								('XyGrib.exe' as copied from Qt release build folder. ADD libopenjp2.dll)
#		-- win_online_installer					(structure as copied from the repository clone)
#				-- config
#				-- packages
#						-- org.opengribs.xygrib.core.win	
#								-- data 		(should be empty except for icons folder)
#										-- icons
#												xyGrib.ico			(check that it exists)
#								-- meta
#						-- org.opengribs.xygrib.data	
#								-- data			(latest XyGrib 'data' structure should be copied here under 'data' 2x data !)
#								-- meta
#						-- org.opengribs.xygrib.maps
#								-- data			(hires map 'data' structure should be copied here under 'data' 2x data !)
#								-- meta
#				-- repository					(should be empty)
#
#
# Also assumes that 'XyGrib.exe' has been copied from the Qt release build folders to the 'deploy' folder
# That XyGrib 'data' structure and hires map 'data' structure are copied to respective 'data' folders ('data' appears in two levels in each case)
#
# After running the script the installers should be in the win_online_installer folder and the repository should be ready for upload
#
# UPDATE VERSION AND RELEASE DATES in xml files

XVER="v1.2.2"

## run the Qt windows deployment tool to create the executable package
cd deploy
if which windeployqt >/dev/null; then
  DEPLOY='windeployqt.exe'
else
	echo "Tool windeployqt is not in the path, can't continue"
	exit 1
fi

$DEPLOY --no-translations --release --no-opengl-sw XyGrib.exe

## now copy bundle to installer package for core.win
cp -rf ./ ../win_online_installer/packages/org.opengribs.xygrib.core.win/data
cp /d/qtprojects/xygrib/LICENSE ../win_online_installer/packages/org.opengribs.xygrib.core.win/data
cp /d/qtprojects/xygrib/README.md ../win_online_installer/packages/org.opengribs.xygrib.core.win/data

## go to the installer build folder
cd ../win_online_installer

## build the repository which should be empty (new one each time)
if which repogen.exe >/dev/null; then
  REPOGEN='repogen.exe'
else
  echo "Tool repogen not found in path, can't continue"
  exit 1
fi

$REPOGEN --update-new-components -v -p packages repository

## create the installer apps
if which binarycreator.exe >/dev/null; then
  BINARYCREATOR='binarycreator.exe'
else
  echo "Tool binarycreator not found in path, can't continue"
  exit 1
fi

$BINARYCREATOR --online-only -v -c config/config.xml -p packages XyGrib_Win_Online_Installer_$XVER
$BINARYCREATOR -v -c config/config.xml -p packages -e org.opengribs.xygrib.maps XyGrib_Win_Offline_Installer_$XVER
$BINARYCREATOR -v --offline-only -c config/config.xml -p packages  XyGrib_Win_Testing_Installer_$XVER

echo "++++ All Done ++++"
