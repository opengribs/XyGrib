# XyGrib - Weather Forecast Visualization
Fork of zyGrib-8.0.1 under a new name and a new home https://opengribs.org

Changes in version 1.2.4
- New feature - Arome 0.025 deg regional model has been added covering all of France and neigbouring areas
- Grib grid only shown if there is weather display selected and if zoom factor allows it (no more black out)
- Many code improvements thanks to Didier
- Bug fixes including crashes on closing POI Editor or opening MeteoTable


Changes in version 1.2.2 :
- New feature - Gust color map that can show absolute gust velocity values or relative (delta) values above the wind speed. Absolute gust velocity is the default. This can be changed in the weather options sub-menu.
- Transparency added to lowest color values for Precipitation, CAPE, Reflectivity, Gust (delta) and Snow. This improves the color overlay displays.
- Functionality for viewing non-global model coverage is improved and now zooms on the shaded area.
- Cloud display problems with ICON gribs is fixed.
- Enforcement of minimum area for grib requests is fixed. "Bad Request" errors are no longer returned on very small area requests.
- Several other bugs were fixed and several code stability improvements were included.


Changes in version 1.2.0 :
- Major enhancement to Grib Server and to XyGrib with the addition of large area regional models. These include NAM (CONUS, Central America & Carribean, Pacific & Hawai), ICON-EU and Arpege-EU.
- ECMWF added to global models albeit a limited free public release of the model including some surface and altitude data at 0.5 deg resolution and 24hr temporal resolution.
- Bug fixes
- Many maintenance improvements on stability and speed (Thanks to Didier)


Changes in version 1.1.2 :
- Mainly bug fixes regarding the saving of settings and a few others
- New installer system applied now to Mac OS
- Native menu system now used on Mac OS (Thanks to Theeko)


Changes in version 1.1.1 :
- Build system changed from qmake to cmake (Thanks to Pavel Kalian)
- Major clean up and improvements to code and stability (Thanks to Didier)
- Jpeg2000 compression in Grib files is now working properly. Wave data on the grib server will included this compression soon. (Thanks to Didier)
- Application and data have been separated. The app goes to where application should be installed and static data goes to where applicaiton data needs should be located.
- New installer system for binaries. The installer downloads app and static data from an online repository. Installation includes optional install of high resolution maps. It also installs a maintenance tool that can be used to install updates, add/remove components or uninstall all.


Changes in version 1.1.0 :
- Interactive GRIB downloads is now from the OpenGribs' on-the-fly grib server with
  multiple models to choose from. These include: GFS, ICON and Arpege Global model
  and WW3, GWAM and EWAM wave models.
- Opened the filter logic of accepting only gribs from recognized sources. 
  Now any valid grib file can be read.
- Improvements in the use of alternate parameters for the same result such as
  Total Precipitation and Precipitation Rate.
- A startup check for new versions is now build-in. This can also be performed
  from the help menu group.
- Significant cleaning up of the code
- Hebrew language was added
- Select/Pan toggle (Thanks to Andrew Tseng)


Changes in version 1.0.1 :
- Addition of new meteorological parameter 'Composite Simulated Reflectivity'. This supports visualisation of severe convection
- Inclusion of DWD ICON gribs
- Addition of Grib2 handling for wave data
- Enabling of Gusts at both surface or 10 meters
- Name change of the application to XyGrib
- Inclusion of Norwegian Meteorological Institute and NRK Gribs
- Inclusion of German Hydrographic Office current gribs
- Switchable dark/light skin (in Options menu)
- Help points to Wiki (WIP)

