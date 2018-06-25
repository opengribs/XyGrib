# XyGrib - Weather Forecast Visualization
Fork of zyGrib-8.0.1 under a new name and a new home https://opengribs.org

Changes in version 1.1.1 :
- Build system changed from qmake to cmake (Thanks to Pavel Kalian)


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



