/**********************************************************************
zUGrib: meteorologic GRIB file data viewer
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

#include "GshhsReader.h"

//==========================================================
// GshhsPolygon  (compatible avec le format .rim de RANGS)
//==========================================================
GshhsPolygon::GshhsPolygon(ZUFILE *file_)
{
 	file  = file_;
    ok = true;
    id    = GshhsPolygon::readInt4();
    n     = GshhsPolygon::readInt4();
    flag  = GshhsPolygon::readInt4();
    west  = GshhsPolygon::readInt4() * 1e-6;
    east  = GshhsPolygon::readInt4() * 1e-6;
    south = GshhsPolygon::readInt4() * 1e-6;
    north = GshhsPolygon::readInt4() * 1e-6;
    area  = GshhsPolygon::readInt4();
    greenwich = (GshhsPolygon::readInt2() != 0);
    GshhsPolygon::readInt2();   // source

    if (ok)
    {
	    antarctic = (west==0 && east==360);
		double x, y=-90;

        if (n < 0 || n > 10*1000*1000) {
            ok = false;
            return;
        }
        for (int i=0; ok && i<n; i++) {
            x = GshhsPolygon::readInt4() * 1e-6;
            if (greenwich && x > 270)
                x -= 360;
            y = GshhsPolygon::readInt4() * 1e-6;
            lsPoints.push_back(new GshhsPoint(x,y));
        }
        
    	// force l'Antarctic à être un "rectangle" qui passe par le pôle
        if (antarctic) {
            lsPoints.insert (lsPoints.begin(), 2, nullptr);
        	lsPoints [1] = new GshhsPoint(360, y);
        	lsPoints [0] = new GshhsPoint(360,-90);
            lsPoints.push_back(new GshhsPoint(0,-90));
        }
    
    }
}

//==========================================================
// GshhsPolygon_WDB     (entete de type GSHHS récent)
//==========================================================
GshhsPolygon_WDB::GshhsPolygon_WDB(ZUFILE *file_)
{
    file  = file_;
    ok = true;
    id    = GshhsPolygon_WDB::readInt4();
    n     = GshhsPolygon_WDB::readInt4();
    flag  = GshhsPolygon_WDB::readInt4();
    west  = GshhsPolygon_WDB::readInt4() * 1e-6;
    east  = GshhsPolygon_WDB::readInt4() * 1e-6;
    south = GshhsPolygon_WDB::readInt4() * 1e-6;
    north = GshhsPolygon_WDB::readInt4() * 1e-6;
    area  = GshhsPolygon_WDB::readInt4();
    
    greenwich = false;
    antarctic = false;
    if (ok) {
        if (n < 0 || n > 10*1000*1000) {
            ok = false;
            return;
        }
        for (int i=0; ok && i<n; i++) {
            double x, y;
            x = GshhsPolygon_WDB::readInt4() * 1e-6;
            if (greenwich && x > 270)
                x -= 360;
            y = GshhsPolygon_WDB::readInt4() * 1e-6;
            lsPoints.push_back(new GshhsPoint(x,y));
        }
    }
}


//--------------------------------------------------------
// Destructeur
GshhsPolygon::~GshhsPolygon() {
    for (auto & lsPoint : lsPoints) {
        delete lsPoint;
    }
    lsPoints.clear();
}


//==========================================================
//==========================================================
//==========================================================
// GshhsReader
//==========================================================
//==========================================================
//==========================================================
GshhsReader::GshhsReader (const QString& fpath, int quality)
{
    this->fpath = fpath;
    gshhsRangsReader = new GshhsRangsReader(fpath);
    isUsingRangsReader = true;
	isListCreator = true;
    for (int qual=0; qual<5; qual++)
    {
        lsPoly_level1[qual] = new std::vector <GshhsPolygon*>;
        lsPoly_level2[qual] = new std::vector <GshhsPolygon*>;
        lsPoly_level3[qual] = new std::vector <GshhsPolygon*>;
        lsPoly_level4[qual] = new std::vector <GshhsPolygon*>;
        lsPoly_boundaries[qual] = new std::vector <GshhsPolygon*>;
        lsPoly_rivers[qual] = new std::vector <GshhsPolygon*>;
    }
    userPreferredQuality = quality;
    setQuality(quality);
}

//-------------------------------------------------------
// Recopie
GshhsReader::GshhsReader (const GshhsReader &model)
{
    fpath = model.fpath;
    gshhsRangsReader = new GshhsRangsReader(fpath);	
    isUsingRangsReader = model.isUsingRangsReader;
    // reuse lists of polygons
	isListCreator = false;
    for (int qual=0; qual<5; qual++)
    {
        lsPoly_level1[qual] = model.lsPoly_level1[qual];
        lsPoly_level2[qual] = model.lsPoly_level2[qual];
        lsPoly_level3[qual] = model.lsPoly_level3[qual];
        lsPoly_level4[qual] = model.lsPoly_level4[qual];
        lsPoly_boundaries[qual] = model.lsPoly_boundaries[qual];
        lsPoly_rivers[qual] = model.lsPoly_rivers[qual];
    }
    userPreferredQuality = model.userPreferredQuality;
    quality = model.quality;
    setQuality(quality);
}

//-------------------------------------------------------
// Destructeur
GshhsReader::~GshhsReader () 
{
    delete gshhsRangsReader;
	if (isListCreator)
		clearLists();
}

//-----------------------------------------------------------------------
void GshhsReader::clearLists () 
{
    std::vector <GshhsPolygon*>::iterator itp;
    for (int qual=0; qual<5; qual++)
    {
        for (itp=lsPoly_level1[qual]->begin(); itp != lsPoly_level1[qual]->end(); ++itp) {
            delete *itp;
            *itp = nullptr;
        }
        for (itp=lsPoly_level2[qual]->begin(); itp != lsPoly_level2[qual]->end(); ++itp) {
            delete *itp;
            *itp = nullptr;
        }
        for (itp=lsPoly_level3[qual]->begin(); itp != lsPoly_level3[qual]->end(); ++itp) {
            delete *itp;
            *itp = nullptr;
        }
        for (itp=lsPoly_level4[qual]->begin(); itp != lsPoly_level4[qual]->end(); ++itp) {
            delete *itp;
            *itp = nullptr;
        }
        for (itp=lsPoly_boundaries[qual]->begin(); itp != lsPoly_boundaries[qual]->end(); ++itp) {
            delete *itp;
            *itp = nullptr;
        }
        for (itp=lsPoly_rivers[qual]->begin(); itp != lsPoly_rivers[qual]->end(); ++itp) {
            delete *itp;
            *itp = nullptr;
        }
        lsPoly_level1[qual]->clear();
        lsPoly_level2[qual]->clear();
        lsPoly_level3[qual]->clear();
        lsPoly_level4[qual]->clear();
        lsPoly_boundaries[qual]->clear();
        lsPoly_rivers[qual]->clear();
    }
}
//-----------------------------------------------------------------------
// extension du nom de fichier gshhs selon la qualité
QString GshhsReader::getNameExtension(int quality)
{
    QString ext;
    switch (quality) {
        case 0: ext = "c"; break;
        case 1: ext = "l"; break;
        case 2: ext = "i"; break;
        case 3: ext = "h"; break;
        case 4: ext = "f"; break;
        default: ext = "l"; break;
    }
    return ext;
}
//-----------------------------------------------------------------------
QString GshhsReader::getFileName_gshhs (int quality)
{
    // Lit le .rim de RANGS à la place du fichier initial
    char txtn[16];
    if (quality < 0)   quality = 0;
    if (quality > 4)   quality = 4;
    snprintf(txtn, 10, "%d", 4-quality);   // précision inversée :(
    QString fname;
    fname = fpath+"/"+"gshhs_" + txtn + ".rim";
	//printf("%s\n", fname.c_str());
    return fname;
}
QString GshhsReader::getFileName_boundaries (int quality) {
    QString fname;
    auto ext = getNameExtension(quality);
    fname = fpath+"/"+"wdb_borders_" + ext + ".b";
    return fname;
}
QString GshhsReader::getFileName_rivers (int quality) {
    QString fname;
    auto  ext = getNameExtension(quality);
    fname = fpath+"/"+"wdb_rivers_" + ext + ".b";
    return fname;
}
//-----------------------------------------------------------------------
bool GshhsReader::gshhsFilesExists(int quality)
{
    if (zu_can_read_file (qPrintable(getFileName_gshhs(quality))) == 0)
        return false;
    if (zu_can_read_file (qPrintable(getFileName_boundaries(quality))) == 0)
        return false;
    if (zu_can_read_file (qPrintable(getFileName_rivers(quality))) == 0)
        return false;
    return true;
}
//-----------------------------------------------------------------------
void GshhsReader::readGshhsFiles()
{
    QString fname;
    ZUFILE *file;
    bool   ok;

	// Bordures des continents (4 niveaux) (gshhs_[clihf].b)
	if (lsPoly_level1[quality]->empty()) { // on ne lit qu'une fois le fichier
		fname = getFileName_gshhs(quality);
		file = zu_open(qPrintable(fname), "rb");
        if (file != nullptr) {
			
			ok = true;
			while (ok) {
				GshhsPolygon *poly = new GshhsPolygon(file);
				ok = poly->isOk();
				if (ok) {
					switch (poly->getLevel()) { /* 0..255 */
						case 1: lsPoly_level1[quality]->push_back(poly); break;
						case 2: lsPoly_level2[quality]->push_back(poly); break;
						case 3: lsPoly_level3[quality]->push_back(poly); break;
						case 4: lsPoly_level4[quality]->push_back(poly); break;
						default: delete poly; break;
					}
				}
				else
				    delete poly;
			}
			zu_close(file);
		}
		//printf("GshhsReader::readGshhsFiles(%d)\n", quality);
	}
}

//-----------------------------------------------------------------------
void GshhsReader::setUserPreferredQuality(int quality_) // 5 levels: 0=low ... 4=full
{
	userPreferredQuality = quality_;
}

//-----------------------------------------------------------------------
void GshhsReader::setQuality(int quality_) // 5 levels: 0=low ... 4=full
{
    QString fname;
    ZUFILE *file;
    bool   ok;

    quality = quality_;
    if (quality < 0) quality = 0;
    else if (quality > 4) quality = 4;
    
    gshhsRangsReader->setQuality(quality);

    if (!isUsingRangsReader) {
	    readGshhsFiles();
    }
        
    // Frontières politiques
    if (lsPoly_boundaries[quality]->empty()) { // on ne lit qu'une fois le fichier
        fname = getFileName_boundaries(quality);
        file = zu_open(qPrintable(fname), "rb");
        if (file != nullptr) {
            ok = true;
            while (ok) {
                GshhsPolygon *poly = new GshhsPolygon_WDB(file);
                ok = poly->isOk();
                if (ok && poly->getLevel() < 2) {
                    lsPoly_boundaries[quality]->push_back(poly);
                }
                else
                    delete poly;

            }
            zu_close(file);
        }
    }
    // Rivières
    if (lsPoly_rivers[quality]->empty()) { // on ne lit qu'une fois le fichier
        fname = getFileName_rivers(quality);
        file = zu_open(qPrintable(fname), "rb");
        if (file != nullptr) {
            ok = true;
            while (ok) {
                GshhsPolygon *poly = new GshhsPolygon_WDB(file);
                ok = poly->isOk();
                if (ok) {
                    lsPoly_rivers[quality]->push_back(poly);
                }
                else
                    delete poly;

            }
            zu_close(file);
        }
    }
}

//-----------------------------------------------------------------------
std::vector <GshhsPolygon*> & GshhsReader::getList_level(int level) {
    switch (level) {
        case 1: return * lsPoly_level1[quality];
        case 2: return * lsPoly_level2[quality];
        case 3: return * lsPoly_level3[quality];
        case 4: return * lsPoly_level4[quality];
        default: return * lsPoly_level1[quality];
    }
}
//-----------------------------------------------------------------------
std::vector <GshhsPolygon*> & GshhsReader::getList_boundaries() {
    return * lsPoly_boundaries[quality];
}
//-----------------------------------------------------------------------
std::vector <GshhsPolygon*> & GshhsReader::getList_rivers() {
    return * lsPoly_rivers[quality];
}
        
//=====================================================================
// Dessin de la carte
//=====================================================================
int GshhsReader::GSHHS_scaledPoints(
            GshhsPolygon *pol, QPoint *pts, double decx,
            Projection *proj
        )
{
    // Elimine les polygones en dehors de la zone visible
    if (! proj->intersect(pol->west+decx, pol->east+decx, pol->south, pol->north)) {
        //printf("%f %f   -  %f %f \n", pol->west+decx, pol->east+decx, pol->south, pol->north);
        return 0;
    }

    // Elimine les polygones trop petits
    int a1, b1;
    int a2, b2;
    proj->map2screen(pol->west+decx, pol->north, &a1, &b1);
    proj->map2screen(pol->east+decx, pol->south, &a2, &b2);
    if (a1==a2 && b1==b2) {
        return 0;
    }
    
    double x, y;
    int xx, yy, oxx=0, oyy=0;
    int j = 0;
    
    for  (auto & lsPoint : pol->lsPoints)
    {
        x = lsPoint->lon+decx;
        y = lsPoint->lat;
        // Ajustement d'échelle
        proj->map2screen(x, y, &xx, &yy);
        if (j==0 || (oxx!=xx || oyy!=yy))  // élimine les ponts trop proches
        {
            oxx = xx;
            oyy = yy;
            pts[j].setX(xx);
            pts[j].setY(yy);
            j ++;
        }
    }
	//if (j>1000)printf("%d\n", j);    
    return j;
}

//-----------------------------------------------------------------------
void GshhsReader::GsshDrawPolygons(QPainter &pnt, std::vector <GshhsPolygon*> &lst,
                                Projection *proj
        )
{
    int nbp;
    
    int nbmax = 10000;
    QPoint *pts = new QPoint[nbmax];
    assert(pts);
    
    for  (auto pol : lst) {
        assert(pol->isOk());
        
        if (nbmax < pol->n+2) {
            nbmax = pol->n+2;
            delete [] pts;
            pts = new QPoint[nbmax];
            assert(pts);
        }
        
        nbp = GSHHS_scaledPoints(pol, pts, 0, proj);
        if (nbp > 3)
            pnt.drawPolygon(pts, nbp);
            
        nbp = GSHHS_scaledPoints(pol, pts, -360, proj);
        if (nbp > 3)
            pnt.drawPolygon(pts, nbp);
    }

    delete [] pts;
}

//-----------------------------------------------------------------------
void GshhsReader::GsshDrawLines(QPainter &pnt, std::vector <GshhsPolygon*> &lst,
                                Projection *proj, bool isClosed
        )
{
    int nbp;
    
    int nbmax = 10000;
    QPoint *pts = new QPoint[nbmax];
    assert(pts);
    
    for  (auto pol : lst) {
        assert(pol->isOk());
        
        if (nbmax < pol->n+2) {
            nbmax = pol->n+2;
            delete [] pts;
            pts = new QPoint[nbmax];
            assert(pts);
        }
        
		
		//--------------------------------------------------------------
        nbp = GSHHS_scaledPoints(pol, pts, 0, proj);
		if (nbp > 1) {
			if (pol->isAntarctic()) {
				// Ne pas tracer les bords artificiels qui rejoignent le pôle
				// ajoutés lors de la création des polygones (2 au début, 1 à la fin).
				pts ++;
				nbp -= 2;
				pnt.drawPolyline(pts, nbp);
				pts --;
			}
			else {
				pnt.drawPolyline(pts, nbp);
				if (isClosed)
					pnt.drawLine(pts[0], pts[nbp-1]);
			}
		}
        
		//--------------------------------------------------------------
        nbp = GSHHS_scaledPoints(pol, pts, -360, proj);
		if (nbp > 1) {
			if (pol->isAntarctic()) {
				// Ne pas tracer les bords artificiels qui rejoignent le pôle
				// ajoutés lors de la création des polygones (2 au début, 1 à la fin).
				pts ++;
				nbp -= 2;
				pnt.drawPolyline(pts, nbp);
				pts --;
			}
			else {
				pnt.drawPolyline(pts, nbp);
				if (isClosed)
					pnt.drawLine(pts[0], pts[nbp-1]);
			}
		}
    }
    delete [] pts;
}

//-----------------------------------------------------------------------
void GshhsReader::drawBackground( QPainter &pnt, Projection *proj,
            const QColor& seaColor, const QColor& backgroundColor
        )
{
    // fond de carte
    pnt.setBrush(backgroundColor);
    pnt.setPen(backgroundColor);
    pnt.drawRect(0,0,proj->getW(), proj->getH());
    
    // océans bleus (peint toute la zone entre les 2 pôles)
    pnt.setBrush(seaColor);
    pnt.setPen(seaColor);
    int x0,y0,x1,y1;
    proj->map2screen(0,90, &x0,&y0);
    proj->map2screen(0,-90, &x1,&y1);

	//printf("drawBackground y0=%d y1=%d\n", y0,y1);
    
    pnt.drawRect(0, y0, proj->getW(), y1-y0);

}

//-----------------------------------------------------------------------
void GshhsReader::drawContinents( QPainter &pnt, Projection *proj,
            const QColor& seaColor, const QColor& landColor
        )
{
	selectBestQuality(proj);
	
    pnt.setPen(Qt::transparent);

    if (isUsingRangsReader) {
        gshhsRangsReader->drawGshhsRangsMapPlain(pnt, proj, seaColor, landColor);
        return;
    }
    
    readGshhsFiles();
    
    // Continents (level 1)
    pnt.setBrush(landColor);
    GsshDrawPolygons(pnt, getList_level(1), proj);
    // Grands lacs (level 2)
    pnt.setBrush(seaColor);
    GsshDrawPolygons(pnt, getList_level(2), proj);
    // Terres dans les grands lacs (level 3)
    pnt.setBrush(landColor);
    GsshDrawPolygons(pnt, getList_level(3), proj);
    // Lacs dans les terres dans les grands lacs (level 4)
    pnt.setBrush(seaColor);
    GsshDrawPolygons(pnt, getList_level(4), proj);
}

//-----------------------------------------------------------------------
void GshhsReader::drawSeaBorders( QPainter &pnt, Projection *proj)
{
	selectBestQuality(proj);

    pnt.setBrush(Qt::transparent);
    
    if (isUsingRangsReader) {
       gshhsRangsReader->drawGshhsRangsMapSeaBorders(pnt, proj);
       return;
    }
    
    readGshhsFiles();
    
    // Continents (level 1)
    GsshDrawLines(pnt, getList_level(1), proj, true);
    // Grands lacs (level 2)
    GsshDrawLines(pnt, getList_level(2), proj, true);
    // Terres dans les grands lacs (level 3)
    GsshDrawLines(pnt, getList_level(3), proj, true);
    // Lacs dans les terres dans les grands lacs (level 4)
    GsshDrawLines(pnt, getList_level(4), proj, true);
}

//-----------------------------------------------------------------------
void GshhsReader::drawBoundaries( QPainter &pnt, Projection *proj)
{
    // Frontières
    GsshDrawLines(pnt, getList_boundaries(), proj, false);
}

//-----------------------------------------------------------------------
void GshhsReader::drawRivers( QPainter &pnt, Projection *proj)
{
    // Rivières
    GsshDrawLines(pnt, getList_rivers(), proj, false);
}

//-----------------------------------------------------------------------
void GshhsReader::selectBestQuality(Projection *proj)
{
	double gshhsRangsThreshold = 200;	// FIXME
	isUsingRangsReader = proj->getCoefremp()<gshhsRangsThreshold;
	
	int bestQuality = 0;
	if (proj->getCoefremp() > 50)
		bestQuality = 0;
	else if (proj->getCoefremp() > 5)
		bestQuality = 1;
	else if (proj->getCoefremp() > 0.2)
		bestQuality = 2;
	else if (proj->getCoefremp() > 0.005)
		bestQuality = 3;
	else
		bestQuality = 4;
	
	if (bestQuality > userPreferredQuality)
		setQuality(userPreferredQuality);
	else
		setQuality(bestQuality);

	//printf("coefremp=%.2f usingRangs=%d qual=%d\n", proj->getCoefremp(),(int)isUsingRangsReader,getQuality());
}

