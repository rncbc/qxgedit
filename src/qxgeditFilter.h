// qxgeditFilter.h
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

#ifndef __qxgeditFilter_h
#define __qxgeditFilter_h

#include <QFrame>


//----------------------------------------------------------------------------
// qxgeditFilter -- Custom widget

class qxgeditFilter : public QFrame
{
	Q_OBJECT

public:

	// Constructor.
	qxgeditFilter(QWidget *pParent = 0, Qt::WindowFlags wflags = 0);
	// Destructor.
	~qxgeditFilter();

	// Parameter getters.
	unsigned short freq() const;
	unsigned short reso() const;

public slots:

	// Parameter setters.
	void setFreq(unsigned short ifreq);
	void setReso(unsigned short ireso);

signals:

	// Parameter change signals.
	void freqChanged(unsigned short iFreq);
	void resoChanged(unsigned short iReso);

protected:

	// Draw canvas.
	void paintEvent(QPaintEvent *);

	// Drag/move curve.
	void dragCurve(const QPoint& pos);

	// Mouse interaction.
	void mousePressEvent(QMouseEvent *pMouseEvent);
	void mouseMoveEvent(QMouseEvent *pMouseEvent);
	void mouseReleaseEvent(QMouseEvent *pMouseEvent);

private:

	// Instance state.
	unsigned short m_iFreq;
	unsigned short m_iReso;

	// Drag state.
	bool   m_bDragging;
	QPoint m_posDrag;
};

#endif	// __qxgeditFilter_h

// end of qxgeditFilter.h
