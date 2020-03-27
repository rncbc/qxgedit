// qxgeditFilter.h
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
	qxgeditFilter(QWidget *pParent = nullptr);
	// Destructor.
	~qxgeditFilter();

	// Parameter getters.
	unsigned short cutoff() const;
	unsigned short resonance() const;

	// Parameter range modifiers.
	void setMaxCutoff(unsigned short iMaxCutoff);
	unsigned short maxCutoff() const;

	void setMaxResonance(unsigned short iMaxResonance);
	unsigned short maxResonance() const;

public slots:

	// Parameter setters.
	void setCutoff(unsigned short iCutoff);
	void setResonance(unsigned short iResonance);

signals:

	// Parameter change signals.
	void cutoffChanged(unsigned short iCutoff);
	void resonanceChanged(unsigned short iResonance);

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
	unsigned short m_iCutoff;
	unsigned short m_iResonance;

	unsigned short m_iMaxCutoff;
	unsigned short m_iMaxResonance;

	// Drag state.
	bool   m_bDragging;
	QPoint m_posDrag;
};

#endif	// __qxgeditFilter_h

// end of qxgeditFilter.h
