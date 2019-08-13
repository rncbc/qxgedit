// qxgeditAmpEg.h
//
/****************************************************************************
   Copyright (C) 2005-2019, rncbc aka Rui Nuno Capela. All rights reserved.

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

#ifndef __qxgeditAmpEg_h
#define __qxgeditAmpEg_h

#include <QFrame>


//----------------------------------------------------------------------------
// qxgeditAmpEg -- Custom widget

class qxgeditAmpEg : public QFrame
{
	Q_OBJECT

public:

	// Constructor.
	qxgeditAmpEg(QWidget *pParent = 0, Qt::WindowFlags wflags = 0);
	// Destructor.
	~qxgeditAmpEg();

	// Parameter getters.
	unsigned short attack() const;
	unsigned short release() const;
	unsigned short decay1() const;
	unsigned short decay2() const;
	unsigned short level1() const;
	unsigned short level2() const;

public slots:

	// Parameter setters.
	void setAttack(unsigned short iAttack);
	void setRelease(unsigned short iRelease);
	void setDecay1(unsigned short iDecay1);
	void setDecay2(unsigned short iDecay2);
	void setLevel1(unsigned short iLevel1);
	void setLevel2(unsigned short iLevel2);

signals:

	// Parameter change signals.
	void attackChanged(unsigned short iAttack);
	void releaseChanged(unsigned short iRelease);
	void decay1Changed(unsigned short iDecay1);
	void decay2Changed(unsigned short iDecay2);
	void level1Changed(unsigned short iLevel1);
	void level2Changed(unsigned short iLevel2);

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
	unsigned short m_iAttack;
	unsigned short m_iRelease;
	unsigned short m_iDecay1;
	unsigned short m_iDecay2;
	unsigned short m_iLevel1;
	unsigned short m_iLevel2;

	// Draw state.
	QPolygon m_poly;

	// Drag state.
	int    m_iDragNode;
	QPoint m_posDrag;
};

#endif	// __qxgeditAmpEg_h

// end of qxgeditAmpEg.h
