// qxgeditScale.cpp
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

#include "qxgeditAbout.h"
#include "qxgeditScale.h"

#include <QPainter>
#include <QMouseEvent>


//----------------------------------------------------------------------------
// qxgeditScale -- Custom widget

// Constructor.
qxgeditScale::qxgeditScale (
	QWidget *pParent, Qt::WindowFlags wflags )
	: QFrame(pParent, wflags),
		m_iBreak1(0), m_iBreak2(0), m_iBreak3(0), m_iBreak4(0),
		m_iOffset1(0), m_iOffset2(0), m_iOffset3(0), m_iOffset4(0),
		m_poly(6), m_iDragNode(-1)
{
	setMouseTracking(true);
	setMinimumSize(QSize(160, 80));

	QFrame::setFrameShape(QFrame::Panel);
	QFrame::setFrameShadow(QFrame::Sunken);
}


// Destructor.
qxgeditScale::~qxgeditScale (void)
{
}


// Parameter accessors.
void qxgeditScale::setBreak1 ( unsigned short iBreak1 )
{
	if (iBreak1 > 127 - 3)
		iBreak1 = 127 - 3;

	if (m_iBreak1 != iBreak1) {
		m_iBreak1  = iBreak1;
		if (iBreak1 >= m_iBreak2)
			setBreak2(iBreak1 + 1);
		else
			update();
		emit break1Changed(break1());
	}
}

unsigned short qxgeditScale::break1 (void) const
{
	return m_iBreak1;
}


void qxgeditScale::setBreak2 ( unsigned short iBreak2 )
{
	if (iBreak2 > 127 - 2)
		iBreak2 = 127 - 2;
	else
	if (iBreak2 < 1)
		iBreak2 = 1;

	if (m_iBreak2 != iBreak2) {
		m_iBreak2  = iBreak2;
		if (m_iBreak1 >= iBreak2)
			setBreak1(iBreak2 - 1);
		else
		if (iBreak2 >= m_iBreak3)
			setBreak3(iBreak2 + 1);
		else
			update();
		emit break2Changed(break2());
	}
}

unsigned short qxgeditScale::break2 (void) const
{
	return m_iBreak2;
}


void qxgeditScale::setBreak3 ( unsigned short iBreak3 )
{
	if (iBreak3 > 127 - 1)
		iBreak3 = 127 - 1;
	else
	if (iBreak3 < 2)
		iBreak3 = 2;

	if (m_iBreak3 != iBreak3) {
		m_iBreak3  = iBreak3;
		if (m_iBreak2 >= iBreak3)
			setBreak2(iBreak3 - 1);
		else
		if (iBreak3 >= m_iBreak4)
			setBreak4(iBreak3 + 1);
		else
			update();
		emit break3Changed(break3());
	}
}

unsigned short qxgeditScale::break3 (void) const
{
	return m_iBreak3;
}


void qxgeditScale::setBreak4 ( unsigned short iBreak4 )
{
	if (iBreak4 > 127)
		iBreak4 = 127;
	else
	if (iBreak4 < 3)
		iBreak4 = 3;

	if (m_iBreak4 != iBreak4) {
		m_iBreak4  = iBreak4;
		if (m_iBreak3 >= iBreak4)
			setBreak3(iBreak4 - 1);
		else
			update();
		emit break4Changed(break4());
	}
}

unsigned short qxgeditScale::break4 (void) const
{
	return m_iBreak4;
}


void qxgeditScale::setOffset1 ( unsigned short iOffset1 )
{
	if (m_iOffset1 != iOffset1) {
		m_iOffset1  = iOffset1;
		update();
		emit offset1Changed(offset1());
	}
}

unsigned short qxgeditScale::offset1 (void) const
{
	return m_iOffset1;
}


void qxgeditScale::setOffset2 ( unsigned short iOffset2 )
{
	if (m_iOffset2 != iOffset2) {
		m_iOffset2  = iOffset2;
		update();
		emit offset2Changed(offset2());
	}
}

unsigned short qxgeditScale::offset2 (void) const
{
	return m_iOffset2;
}


void qxgeditScale::setOffset3 ( unsigned short iOffset3 )
{
	if (m_iOffset3 != iOffset3) {
		m_iOffset3  = iOffset3;
		update();
		emit offset3Changed(offset3());
	}
}

unsigned short qxgeditScale::offset3 (void) const
{
	return m_iOffset3;
}


void qxgeditScale::setOffset4 ( unsigned short iOffset4 )
{
	if (m_iOffset4 != iOffset4) {
		m_iOffset4  = iOffset4;
		update();
		emit offset4Changed(offset4());
	}
}

unsigned short qxgeditScale::offset4 (void) const
{
	return m_iOffset4;
}


// Draw curve.
void qxgeditScale::paintEvent ( QPaintEvent *pPaintEvent )
{
	QPainter painter(this);

	const int h  = height();
	const int w  = width();

	const int h2 = h >> 1;

	const int x1 = 6 + int((m_iBreak1 * (w - 12)) >> 7);
	const int x2 = 6 + int((m_iBreak2 * (w - 12)) >> 7);
	const int x3 = 6 + int((m_iBreak3 * (w - 12)) >> 7);
	const int x4 = 6 + int((m_iBreak4 * (w - 12)) >> 7);

	const int y1 = h2 - ((int(m_iOffset1) - 64) * (h - 12) >> 7);
	const int y2 = h2 - ((int(m_iOffset2) - 64) * (h - 12) >> 7);
	const int y3 = h2 - ((int(m_iOffset3) - 64) * (h - 12) >> 7);
	const int y4 = h2 - ((int(m_iOffset4) - 64) * (h - 12) >> 7);

	m_poly.putPoints(0, 6,
		0,  y1,
		x1, y1,
		x2, y2,
		x3, y3,
		x4, y4,
		w,  y4);

	const QPalette& pal = palette();
	const bool bDark = (pal.window().color().value() < 0x7f);
	const QColor& rgbLite = (bDark ? Qt::darkYellow : Qt::yellow);
	if (bDark)
		painter.fillRect(0, 0, w, h, pal.dark().color());

	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setPen(bDark ? Qt::gray : Qt::darkGray);

	const QPen oldpen(painter.pen());
	QPen dotpen(oldpen);
	dotpen.setStyle(Qt::DotLine);
	painter.setPen(dotpen);
	painter.drawLine(0, h2, w, h2);
	painter.setPen(oldpen);

	painter.drawPolyline(m_poly);

	painter.setBrush(rgbLite); // pal.midlight().color()
	painter.drawRect(nodeRect(1));
	painter.drawRect(nodeRect(2));
	painter.drawRect(nodeRect(3));
	painter.drawRect(nodeRect(4));

#ifdef CONFIG_DEBUG_0
	painter.drawText(QFrame::rect(),
		Qt::AlignTop|Qt::AlignHCenter,
		tr("Break (%1,%2,%3,%4) Offset(%5,%6,%7,%8)")
		.arg(int(break1()))
		.arg(int(break2()))
		.arg(int(break3()))
		.arg(int(break4()))
		.arg(int(offset1()) - 64)
		.arg(int(offset2()) - 64)
		.arg(int(offset3()) - 64)
		.arg(int(offset4()) - 64));
#endif

	painter.setRenderHint(QPainter::Antialiasing, false);
	painter.end();

	QFrame::paintEvent(pPaintEvent);
}


// Draw rectangular point.
QRect qxgeditScale::nodeRect ( int iNode ) const
{
	const QPoint& pos = m_poly.at(iNode);
	return QRect(pos.x() - 4, pos.y() - 4, 8, 8); 
}


int qxgeditScale::nodeIndex ( const QPoint& pos ) const
{
	if (nodeRect(4).contains(pos))
		return 4; // Break4/Offset4

	if (nodeRect(3).contains(pos))
		return 3; // Break3/Offset3

	if (nodeRect(2).contains(pos))
		return 2; // Break2/Offset2

	if (nodeRect(1).contains(pos))
		return 1; // Break1/Offset1

	return -1;
}


void qxgeditScale::dragNode ( const QPoint& pos )
{
	unsigned short *piBreak  = NULL;
	unsigned short *piOffset = NULL;
	switch (m_iDragNode) {
	case 1: // Break1/Offset1
		piBreak  = &m_iBreak1;
		piOffset = &m_iOffset1;
		break;
	case 2: // Break2/Offset2
		piBreak  = &m_iBreak2;
		piOffset = &m_iOffset2;
		break;
	case 3: // Break3/Offset3
		piBreak  = &m_iBreak3;
		piOffset = &m_iOffset3;
		break;
	case 4: // Break4/Offset4
		piBreak  = &m_iBreak4;
		piOffset = &m_iOffset4;
		break;
	}

	if (piBreak && piOffset) {
		int iBreak = int(*piBreak)
			+ ((pos.x() - m_posDrag.x()) << 7) / width();
		int iOffset = int(*piOffset)
			+ ((m_posDrag.y() - pos.y()) << 7) / height();
		if (iBreak < 0) iBreak = 0;
		else
		if (iBreak > 127) iBreak = 127;
		if (iOffset < 0) iOffset = 0;
		else
		if (iOffset > 127) iOffset = 127;
		if (*piBreak  != (unsigned short) iBreak ||
			*piOffset != (unsigned short) iOffset) {
			m_posDrag = pos;
			switch (m_iDragNode) {
			case 1: // Break1/Offset1
				setBreak1(iBreak);
				setOffset1(iOffset);
				break;
			case 2: // Break2/Offset2
				setBreak2(iBreak);
				setOffset2(iOffset);
				break;
			case 3: // Break3/Offset3
				setBreak3(iBreak);
				setOffset3(iOffset);
				break;
			case 4: // Break4/Offset4
				setBreak4(iBreak);
				setOffset4(iOffset);
				break;
			}
		}
	} else if (nodeIndex(pos) >= 0) {
		setCursor(Qt::PointingHandCursor);
	} else {
		unsetCursor();
	}
}


// Mouse interaction.
void qxgeditScale::mousePressEvent ( QMouseEvent *pMouseEvent )
{
	if (pMouseEvent->button() == Qt::LeftButton) {
		const QPoint& pos = pMouseEvent->pos();
		const int iDragNode = nodeIndex(pos);
		if (iDragNode >= 0) {
			setCursor(Qt::SizeAllCursor);
			m_iDragNode = iDragNode;
			m_posDrag = pos;
		}
	}

	QFrame::mousePressEvent(pMouseEvent);
}


void qxgeditScale::mouseMoveEvent ( QMouseEvent *pMouseEvent )
{
	dragNode(pMouseEvent->pos());
}


void qxgeditScale::mouseReleaseEvent ( QMouseEvent *pMouseEvent )
{
	QFrame::mouseReleaseEvent(pMouseEvent);

	dragNode(pMouseEvent->pos());

	if (m_iDragNode >= 0) {
		m_iDragNode = -1;
		unsetCursor();
	}
}


// end of qxgeditScale.cpp
