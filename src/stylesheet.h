/**********************************************************************
xyGrib: meteorological GRIB file viewer
Created by David Gal

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
***********************************************************************/
#ifndef STYLESHEET_H
#define STYLESHEET_H

QString styleSheetDef =
        "QStatusBar,"
        "QDialog,"
        "QMainWindow,"
        "QMenu,"
        "QFrame,"
        "QMenuBar"
/*        "QDockWidget "*/
        "{color: #cccccc; background: #555555}"
        "QMenuBar::item:selected, QMenu::item::selected {color: #555555; background-color: #cccccc;}"
        "QMenu::separator {height: 2px; background: #777777; margin-left: 10px; margin-right: 5px;}"
        "QPushButton {background-color: #555555; border-style: outset; border-width: 2px;"
                    "border-color: #777777; padding: 6px;min-width: 6em; color: #cccccc;}"
        "QPushButton:pressed {border-width: 2px; border-style: inset;}"
        "QProgressBar{border: 2px solid grey; border-radius: 5px; text-align: center;}"
        "QProgressBar::chunk{background-color: #ff6600; width: 10px; margin: 0.5px;}"
        "QCheckBox{color: #cccccc;}"
        "QToolBar::separator{width: 2px; background: #aaaaaa;}"
        ;

QString menuStyleSheetDef =
        "QMenu,QMenuBar{color: #cccccc; background: #555555}"
        "QMenuBar::item:selected, QMenu::item::selected {color: #555555; background-color: #cccccc;}"
        ;



#endif // STYLESHEET_H
