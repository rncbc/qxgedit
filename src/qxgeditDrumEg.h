// qxgeditDrumEg.h
//
/****************************************************************************
   Copyright (C) 2005-2009, rncbc aka Rui Nuno Capela. All rights reserved.

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

#ifndef __qxgeditDrumEg_h
#define __qxgeditDrumEg_h

#include <QFrame>


//----------------------------------------------------------------------------
// qxgeditDrumEg -- Custom widget

class qxgeditDrumEg : public QFrame
{
	Q_OBJECT

public:

	// Constructor.
	qxgeditDrumEg(QWidget *pParent = 0, Qt::WindowFlags wflags = 0);
	// Destructor.
	~qxgeditDrumEg();

	// Parameter getters.
	unsigned short attack() const;
	unsigned short decay1() const;
	unsigned short decay2() const;

public slots:

	// Parameter setters.
	void setAttack(unsigned short iAttack);
	void setDecay1(unsigned short iDecay1);
	void setDecay2(unsigned short iDecay2);

signals:

	// Parameter change signals.
	void attackChanged(unsigned short iAttack);
	void decay1Changed(unsigned short iDecay1);
	void decay2Changed(unsigned short iDecay2);

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
	unsigned short m_iDecay1;
	unsigned short m_iDecay2;

	// Draw state.
	QPolygon m_poly;

	// Drag state.
	int    m_iDragNode;
	QPoint m_posDrag;
};

#endif	// __qxgeditDrumEg_h

// end of qxgeditDrumEg.h
