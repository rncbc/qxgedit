// qxgeditUserEg.h
//
/****************************************************************************
   Copyright (C) 2005-2015, rncbc aka Rui Nuno Capela. All rights reserved.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*****************************************************************************/

#ifndef __qxgeditUserEg_h
#define __qxgeditUserEg_h

#include <QFrame>


//----------------------------------------------------------------------------
// qxgeditUserEg -- Custom widget

class qxgeditUserEg : public QFrame
{
	Q_OBJECT

public:

	// Constructor.
	qxgeditUserEg(QWidget *pParent = 0, Qt::WindowFlags wflags = 0);
	// Destructor.
	~qxgeditUserEg();

	// Parameter getters.
	unsigned short level0() const;
	unsigned short level1() const;
	unsigned short level2() const;
	unsigned short level3() const;
	unsigned short level4() const;

	unsigned short rate1() const;
	unsigned short rate2() const;
	unsigned short rate3() const;
	unsigned short rate4() const;

public slots:

	// Parameter setters.
	void setLevel0(unsigned short iLevel0);
	void setLevel1(unsigned short iLevel1);
	void setLevel2(unsigned short iLevel2);
	void setLevel3(unsigned short iLevel3);
	void setLevel4(unsigned short iLevel4);

	void setRate1(unsigned short iRate1);
	void setRate2(unsigned short iRate2);
	void setRate3(unsigned short iRate3);
	void setRate4(unsigned short iRate4);

signals:

	// Parameter change signals.
	void level0Changed(unsigned short iLevel0);
	void level1Changed(unsigned short iLevel1);
	void level2Changed(unsigned short iLevel2);
	void level3Changed(unsigned short iLevel3);
	void level4Changed(unsigned short iLevel4);

	void rate1Changed(unsigned short iRate1);
	void rate2Changed(unsigned short iRate2);
	void rate3Changed(unsigned short iRate3);
	void rate4Changed(unsigned short iRate4);

protected:

	// Draw canvas.
	void paintEvent(QPaintEvent *);

	// Draw rectangular point.
	QRect nodeRect(int iNode) const;
	int nodeIndex(const QPoint& pos) const;

	void dragNode(const QPoint& pos);

	// Mouse interaction.
	void mousePressEvent(QMouseEvent *pMouseEvent);
	void mouseMoveEvent(QMouseEvent *pMouseEvent);
	void mouseReleaseEvent(QMouseEvent *pMouseEvent);

private:

	// Instance state.
	unsigned short m_iLevel0;
	unsigned short m_iLevel1;
	unsigned short m_iLevel2;
	unsigned short m_iLevel3;
	unsigned short m_iLevel4;

	unsigned short m_iRate1;
	unsigned short m_iRate2;
	unsigned short m_iRate3;
	unsigned short m_iRate4;

	// Draw state.
	QPolygon m_poly;

	// Drag state.
	int    m_iDragNode;
	QPoint m_posDrag;
};

#endif	// __qxgeditUserEg_h

// end of qxgeditUserEg.h
