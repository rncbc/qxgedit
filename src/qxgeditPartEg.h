// qxgeditPartEg.h
//
/****************************************************************************
   Copyright (C) 2005-2020, rncbc aka Rui Nuno Capela. All rights reserved.

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

#ifndef __qxgeditPartEg_h
#define __qxgeditPartEg_h

#include <QFrame>


//----------------------------------------------------------------------------
// qxgeditPartEg -- Custom widget

class qxgeditPartEg : public QFrame
{
	Q_OBJECT

public:

	// Constructor.
	qxgeditPartEg(QWidget *pParent = 0, Qt::WindowFlags wflags = 0);
	// Destructor.
	~qxgeditPartEg();

	// Parameter getters.
	unsigned short attack() const;
	unsigned short decay() const;
	unsigned short release() const;

public slots:

	// Parameter setters.
	void setAttack(unsigned short iAttack);
	void setDecay(unsigned short iDecay);
	void setRelease(unsigned short iRelease);

signals:

	// Parameter change signals.
	void attackChanged(unsigned short iAttack);
	void decayChanged(unsigned short iDecay);
	void releaseChanged(unsigned short iRelease);

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
	unsigned short m_iDecay;
	unsigned short m_iRelease;

	// Draw state.
	QPolygon m_poly;

	// Drag state.
	int    m_iDragNode;
	QPoint m_posDrag;
};

#endif	// __qxgeditPartEg_h

// end of qxgeditPartEg.h
