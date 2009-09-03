// qxgeditPitch.h
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

#ifndef __qxgeditPitch_h
#define __qxgeditPitch_h

#include <QFrame>


//----------------------------------------------------------------------------
// qxgeditPitch -- Custom widget

class qxgeditPitch : public QFrame
{
	Q_OBJECT

public:

	// Constructor.
	qxgeditPitch(QWidget *pParent = 0, Qt::WindowFlags wflags = 0);
	// Destructor.
	~qxgeditPitch();

	// Parameter getters.
	unsigned short attackTime() const;
	unsigned short attackLevel() const;
	unsigned short releaseTime() const;
	unsigned short releaseLevel() const;

public slots:

	// Parameter setters.
	void setAttackTime(unsigned short iAttackTime);
	void setAttackLevel(unsigned short iAttackLevel);
	void setReleaseTime(unsigned short iReleaseTime);
	void setReleaseLevel(unsigned short iReleaseLevel);

signals:

	// Parameter change signals.
	void attackTimeChanged(unsigned short iAttackTime);
	void attackLevelChanged(unsigned short iAttackLevel);
	void releaseTimeChanged(unsigned short iReleaseTime);
	void releaseLevelChanged(unsigned short iReleaseLevel);

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
	unsigned short m_iAttackTime;
	unsigned short m_iAttackLevel;
	unsigned short m_iReleaseTime;
	unsigned short m_iReleaseLevel;

	// Draw state.
	QPolygon m_poly;

	// Drag state.
	int    m_iDragNode;
	QPoint m_posDrag;
};

#endif	// __qxgeditPitch_h

// end of qxgeditPitch.h
