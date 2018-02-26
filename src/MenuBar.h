/**********************************************************************
zyGrib: meteorological GRIB file viewer
Copyright (C) 2008-2012 - Jacques Zaninetti - http://www.zygrib.org

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

#ifndef MENU_H
#define MENU_H

#include <QMenuBar>
#include <QComboBox>
#include <set>

//================================================================
// ActionGroup which allows 0 or 1 checked action
class ZeroOneActionGroup : public QObject
{ Q_OBJECT
	public:
		ZeroOneActionGroup(QWidget *parent) : QObject(parent) {}
		
		void  addAction (QAction *);
		void  setCheckedAction (QAction *act, bool val, bool emitSignal=false);

	public slots:
		void slot_actionTrigerred (bool);
	signals:
		// Send the checked action, or NULL if there is no cheched action
		void triggered (QAction *);

	private:
		QList <QAction *> lsactions;
};


//================================================================
class MenuBar : public QMenuBar
{
    Q_OBJECT
public:
    MenuBar (QWidget *parent, bool withmblue=false);
	
	void updateFonts ();

    void setQuality (int q);
    void setProjection  (int idproj);
	
    void setIsobarsStep (int step);
	void setIsotherms0Step  (int step);
	void setIsotherms_Step  (int step);
	void setLinesThetaE_Step (int step);
	
    void setCitiesNamesLevel (int level);
	void setWaveArrowsType (int type);

    void updateListeDates  (std::set<time_t> *setDates, time_t currentDate);
    void updateCurrentDate (time_t currentDate);
    time_t  getDateGribById (int id);


	QMenu * createPopupBtRight(QWidget *parent);

    //---------------------------------------------------------
    // Actions des menus
    // Elements de l'interface (public c'est plus pratique)
    //---------------------------------------------------------
    QAction *ac_OpenMeteotable;
	QAction *ac_OpenCurveDrawer;	// added by Tim Holtschneider, 05.2010
    QAction *ac_CreatePOI;
    QAction *ac_CreateAnimation;
    QAction *ac_ExportImage;
	QAction *ac_showSkewtDiagram;

    QAction *acFile_Open;
    QAction *acFile_Close;
	QAction *acFile_NewInstance;
    QAction *acFile_Load_GRIB;
    QAction *acFile_Load_IAC;
    
	QAction *acMBlue_fastInterpolation;
	QAction *acMBlueSwiss_Load;
	QAction *acMBlueSwiss_ShowArea;
	
    QAction *acFile_GribServerStatus;
    QAction *acFile_Info_GRIB;
    QAction *acFile_Quit;

    //------------------------------------------
    ZeroOneActionGroup *acView_GroupColorMap;
		QAction *acView_WindColors;
		QAction *acView_CurrentColors;
		QAction *acView_RainColors;
		QAction *acView_CloudColors;
		QAction *acView_HumidColors;
		QAction *acView_TempColors;
		QAction *acView_DeltaDewpointColors;
		QAction *acView_SnowCateg;
		QAction *acView_SnowDepth;
		QAction *acView_FrzRainCateg;
		QAction *acView_CAPEsfc;
		QAction *acView_CINsfc;
		QAction *acView_ThetaEColors;
		
    QMenu   *menuSeaState;
    QMenu   *menuWavesArrows;
		QAction *acView_SigWaveHeight;
		QAction *acView_MaxWaveHeight;
		QAction *acView_WhiteCapProb;
    QActionGroup *acView_GroupWavesArrows;
        QAction *acView_WavesArrows_none;
        QAction *acView_WavesArrows_max;
        QAction *acView_WavesArrows_swell;
        QAction *acView_WavesArrows_wind;
        QAction *acView_WavesArrows_prim;
        QAction *acView_WavesArrows_scdy;

    QMenu   *menuViewOptions;	
    QAction *acView_ColorMapSmooth;

	QAction *acView_DuplicateFirstCumulativeRecord;
	QAction *acView_DuplicateMissingWaveRecords;
	QAction *acView_InterpolateValues;
	QAction *acView_WindArrowsOnGribGrid;
	QAction *acView_useJetSTreamColorMap;
	QAction *acView_CurrentArrowsOnGribGrid;

    QAction *acView_WindArrow;
    QAction *acView_Barbules;
    QAction *acView_ThinWindArrows;

    QAction *acView_CurrentArrow;

    QAction *acView_Isobars;
    QMenu   *menuIsobarsStep;
    QActionGroup *acView_GroupIsobarsStep;
        QAction *acView_IsobarsStep1;
        QAction *acView_IsobarsStep2;
        QAction *acView_IsobarsStep3;
        QAction *acView_IsobarsStep4;
        QAction *acView_IsobarsStep5;
        QAction *acView_IsobarsStep6;
        QAction *acView_IsobarsStep8;
        QAction *acView_IsobarsStep10;
    QAction *acView_IsobarsLabels;

    QAction *acView_Isotherms0;
    QMenu   *menuIsotherms0Step;
    QActionGroup *acView_GroupIsotherms0Step;
        QAction *acView_Isotherms0Step10;
        QAction *acView_Isotherms0Step20;
        QAction *acView_Isotherms0Step50;
        QAction *acView_Isotherms0Step100;
        QAction *acView_Isotherms0Step200;
        QAction *acView_Isotherms0Step500;
        QAction *acView_Isotherms0Step1000;
    QAction *acView_Isotherms0Labels;

	ZeroOneActionGroup *groupIsotherms;
		QAction *acView_Isotherms_2m;
		QAction *acView_Isotherms_925hpa;
		QAction *acView_Isotherms_850hpa;
		QAction *acView_Isotherms_700hpa;
		QAction *acView_Isotherms_600hpa;
		QAction *acView_Isotherms_500hpa;
		QAction *acView_Isotherms_400hpa;
		QAction *acView_Isotherms_300hpa;
		QAction *acView_Isotherms_200hpa;
	
    QMenu   *menuIsotherms;
    QMenu   *menuIsotherms_Step;
    QAction *acView_Isotherms_Labels;
    QActionGroup *groupIsotherms_Step;
        QAction *acView_Isotherms_Step1;
        QAction *acView_Isotherms_Step2;
        QAction *acView_Isotherms_Step5;
        QAction *acView_Isotherms_Step10;

	QMenu   *menuLinesThetaE;
    QMenu   *menuLinesThetaE_Step;
    QAction *acView_LinesThetaE_Labels;
	ZeroOneActionGroup *groupLinesThetaE;
		QAction *acView_LinesThetaE_925hpa;
		QAction *acView_LinesThetaE_850hpa;
		QAction *acView_LinesThetaE_700hpa;
		QAction *acView_LinesThetaE_600hpa;
		QAction *acView_LinesThetaE_500hpa;
		QAction *acView_LinesThetaE_400hpa;
		QAction *acView_LinesThetaE_300hpa;
		QAction *acView_LinesThetaE_200hpa;
    QActionGroup *groupLinesThetaE_Step;
        QAction *acView_LinesThetaE_Step1;
        QAction *acView_LinesThetaE_Step2;
        QAction *acView_LinesThetaE_Step5;
        QAction *acView_LinesThetaE_Step10;

    QAction *acView_PressureMinMax;
    QAction *acView_TemperatureLabels;
    QAction *acView_GribGrid;
    QAction *acView_ShowColorScale;
    QAction *acView_ShowBoardPanel;
	
    //------------------------------------------
    QActionGroup *acAlt_GroupAltitude;
		QAction *acAlt_MSL;
		QAction *acAlt_sigma995;
		QAction *acAlt_GND;
		QAction *acAlt_GND_1m;
		QAction *acAlt_GND_2m;
		QAction *acAlt_GND_3m;
		QAction *acAlt_GND_10m;
		QAction *acAlt_925hpa;
		QAction *acAlt_850hpa;
		QAction *acAlt_700hpa;
		QAction *acAlt_600hpa;
		QAction *acAlt_500hpa;
		QAction *acAlt_400hpa;
		QAction *acAlt_300hpa;
		QAction *acAlt_200hpa;
		QAction *acAlt_Atmosphere;
		
    ZeroOneActionGroup *acAlt_GroupGeopotLine;
		QAction *acAlt_GeopotLine_925hpa;
		QAction *acAlt_GeopotLine_850hpa;
		QAction *acAlt_GeopotLine_700hpa;
		QAction *acAlt_GeopotLine_600hpa;
		QAction *acAlt_GeopotLine_500hpa;
		QAction *acAlt_GeopotLine_400hpa;
		QAction *acAlt_GeopotLine_300hpa;
		QAction *acAlt_GeopotLine_200hpa;

	QMenu *menuGeopotStep;
    QActionGroup *acAlt_GroupGeopotStep;
        QAction *acAlt_GeopotStep_1;
        QAction *acAlt_GeopotStep_2;
        QAction *acAlt_GeopotStep_5;
        QAction *acAlt_GeopotStep_10;
        QAction *acAlt_GeopotStep_20;
        QAction *acAlt_GeopotStep_50;
        QAction *acAlt_GeopotStep_100;
    QAction *acAlt_GeopotLabels;

    //------------------------------------------
    QAction *acMap_Orthodromie;
    QAction *acMap_Rivers;
    QAction *acMap_CountriesBorders;
    QAction *acMap_CountriesNames;
    QAction *acMap_LonLatGrid;
    QAction *acMap_FindCity;
    QAction *acMap_AutoZoomOnGribArea;

    QActionGroup *acMap_GroupCitiesNames;
        QAction *acMap_CitiesNames0;
        QAction *acMap_CitiesNames1;
        QAction *acMap_CitiesNames2;
        QAction *acMap_CitiesNames3;
        QAction *acMap_CitiesNames4;
        QAction *acMap_CitiesNames5;
    QAction *acMap_ShowPOIs;
    QAction *acMap_ShowMETARs;
    QAction *acMap_SelectMETARs;

    QActionGroup *acMap_GroupQuality;
        QAction *acMap_Quality1;
        QAction *acMap_Quality2;
        QAction *acMap_Quality3;
        QAction *acMap_Quality4;
        QAction *acMap_Quality5;

    QActionGroup *acMap_GroupProjection;
        QAction *acMap_PROJ_ZYGRIB;
        QAction *acMap_PROJ_MERCATOR;
        QAction *acMap_PROJ_MILLER;
        QAction *acMap_PROJ_CENTRAL_CYL;
        QAction *acMap_PROJ_EQU_CYL;

    QAction *acMap_Zoom_In;
    QAction *acMap_Zoom_Out;
    QAction *acMap_Zoom_Sel;
    QAction *acMap_Zoom_All;
    QAction *acMap_Go_Left;
    QAction *acMap_Go_Right;
    QAction *acMap_Go_Up;
    QAction *acMap_Go_Down;

    QAction *acOptions_Proxy;
    QAction *acOptions_AngleConverter;
    QAction *acOptions_Units;
    QAction *acOptions_DateChooser;
    QAction *acOptions_Fonts;
    QAction *acOptions_GraphicsParams;
    QAction *acOptions_Language;

    QAction *acHelp_Help;
    QAction *acHelp_APropos;
    QAction *acHelp_AProposQT;

    //-------------------------------------
    // Autres objets de l'interface
    //-------------------------------------
    QComboBox *cbDatesGrib;      // Choix de la date à afficher
    QAction *acDatesGrib_prev;
    QAction *acDatesGrib_next;

    QMenu *menuFile;
    QMenu *menuColorMap;
    QMenu *menuIsolines;
    QMenu *menuMap;
    QMenu *menuAltitude;
    QMenu *menuOptions;
    QMenu *menuHelp;
	

//------------------------------------------------------------------------
private:
    std::vector<time_t> listGribDates;

    QAction* addAction (QMenu *menu,
                    QString title, QString shortcut, QString statustip,
                    QString iconFileName = "");

    QAction* addGroup (ZeroOneActionGroup *group, QMenu *menu,
                    QString title, QString shortcut, QString statustip,
                    QString iconFileName = "");

    QAction* addGroup (QActionGroup *group, QMenu *menu,
                    QString title, QString shortcut, QString statustip,
                    QString iconFileName = "");

    QAction* addActionCheck (QMenu *menu,
                    QString title, QString shortcut, QString statustip,
                    QString iconFileName = "");


};

#endif
