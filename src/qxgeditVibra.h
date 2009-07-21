// qxgeditVibra.h
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

#ifndef __qxgeditVibra_h
#define __qxgeditVibra_h

#include <QFrame>


//----------------------------------------------------------------------------
// qxgeditVibra -- Custom widget

class qxgeditVibra : public QFrame
{
	Q_OBJECT

public:

	// Constructor.
	qxgeditVibra(QWidget *pParent = 0, Qt::WindowFlags wflags = 0);
	// Destructor.
	~qxgeditVibra();

	// Parameter getters.
	unsigned short delay() const;
	unsigned short rate() const;
	unsigned short depth() const;

public slots:

	// Parameter setters.
	void setDelay(unsigned short iDelay);
	void setRate(unsigned short iRate);
	void setDepth(unsigned short iDepth);

signals:

	// Parameter change signals.
	void delayChanged(unsigned short iDelay);
	void rateChanged(unsigned short iRate);
	void depthChanged(unsigned short iDepth);

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
	unsigned short m_iDelay;
	unsigned short m_iRate;
	unsigned short m_iDepth;

	// Draw state.
	QPolygon m_poly;

	// Drag state.
	int    m_iDragNode;
	QPoint m_posDrag;
};

#endif	// __qxgeditVibra_h

// end of qxgeditVibra.h
