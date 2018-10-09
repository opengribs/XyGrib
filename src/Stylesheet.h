/**********************************************************************
XyGrib: meteorological GRIB file viewer
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
        "QStatusBar,QScrollBar,QDialog,QMainWindow, QFrame {color: #ffffff; background: #666666}"
        "QPushButton {background-color: #666666; border-style: outset; border-width: 2px;"
                    "border-color: #777777; padding: 6px;min-width: 6em; color: #ffffff;}"
        "QPushButton:pressed {border-width: 2px; border-style: inset;}"
        "QPushButton:disabled {color: #aaaaaa;}"
        "QProgressBar{border: 2px solid grey; border-radius: 5px; text-align: center;}"
        "QProgressBar::chunk{background-color: #ff6600; width: 10px; margin: 0.5px;}"
        "QCheckBox, QRadioButton{color: #ffffff; background-color: #666666}"
        "QCheckBox:disabled{color: #aaaaaa;}"
        "QToolBar {background: #666666;}"
        "QToolBar::separator{width: 2px; background: #aaaaaa;}"
        "QTabBar::tab {background: #666666; color: #ffffff;}"
        "QTabBar::tab:selected {background: #ffffff; color: #666666; }"
        "QTabBar::tab:hover {background: #aaaaaa; color: #666666; }"
        "QDockWidget {color: #ffffff;}"
        ;

QString menuStyleSheetDef =
        "QMenu,QMenuBar {color: #ffffff; background: #666666;}"
        "QMenuBar::item:selected, QMenu::item::selected {color: #666666; background-color: #dddddd;}"
        "QMenu::item:disabled {color: #aaaaaa;}"
        ;



#endif // STYLESHEET_H
