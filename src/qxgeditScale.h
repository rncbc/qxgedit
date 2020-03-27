// qxgeditScale.h
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

#ifndef __qxgeditScale_h
#define __qxgeditScale_h

#include <QFrame>


//----------------------------------------------------------------------------
// qxgeditScale -- Custom widget

class qxgeditScale : public QFrame
{
	Q_OBJECT

public:

	// Constructor.
	qxgeditScale(QWidget *pParent = nullptr);
	// Destructor.
	~qxgeditScale();

	// Parameter getters.
	unsigned short break1() const;
	unsigned short break2() const;
	unsigned short break3() const;
	unsigned short break4() const;

	unsigned short offset1() const;
	unsigned short offset2() const;
	unsigned short offset3() const;
	unsigned short offset4() const;

public slots:

	// Parameter setters.
	void setBreak1(unsigned short iBreak1);
	void setBreak2(unsigned short iBreak2);
	void setBreak3(unsigned short iBreak3);
	void setBreak4(unsigned short iBreak4);

	void setOffset1(unsigned short iOffset1);
	void setOffset2(unsigned short iOffset2);
	void setOffset3(unsigned short iOffset3);
	void setOffset4(unsigned short iOffset4);

signals:

	// Parameter change signals.
	void break1Changed(unsigned short iBreak1);
	void break2Changed(unsigned short iBreak2);
	void break3Changed(unsigned short iBreak3);
	void break4Changed(unsigned short iBreak4);

	void offset1Changed(unsigned short iOffset1);
	void offset2Changed(unsigned short iOffset2);
	void offset3Changed(unsigned short iOffset3);
	void offset4Changed(unsigned short iOffset4);

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
	unsigned short m_iBreak1;
	unsigned short m_iBreak2;
	unsigned short m_iBreak3;
	unsigned short m_iBreak4;

	unsigned short m_iOffset1;
	unsigned short m_iOffset2;
	unsigned short m_iOffset3;
	unsigned short m_iOffset4;

	// Draw state.
	QPolygon m_poly;

	// Drag state.
	int    m_iDragNode;
	QPoint m_posDrag;
};


#endif	// __qxgeditScale_h

// end of qxgeditScale.h
