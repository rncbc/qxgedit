// qxgeditVibra.cpp
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

#include "qxgeditAbout.h"
#include "qxgeditVibra.h"

#include <QPainter>
#include <QMouseEvent>


//----------------------------------------------------------------------------
// qxgeditVibra -- Custom widget

// Constructor.
qxgeditVibra::qxgeditVibra (
	QWidget *pParent, Qt::WindowFlags wflags )
	: QFrame(pParent, wflags),
		m_iDelay(0), m_iRate(0), m_iDepth(0),
		m_poly(3), m_iDragNode(-1)
{
	setMouseTracking(true);
	setMinimumSize(QSize(120, 80));
}


// Destructor.
qxgeditVibra::~qxgeditVibra (void)
{
}


// Parameter accessors.
void qxgeditVibra::setDelay ( unsigned short iDelay )
{
	if (m_iDelay != iDelay) {
		m_iDelay  = iDelay;
		update();
		emit delayChanged(delay());
	}
}

unsigned short qxgeditVibra::delay (void) const
{
	return m_iDelay;
}


void qxgeditVibra::setRate ( unsigned short iRate )
{
	if (m_iRate != iRate) {
		m_iRate  = iRate;
		update();
		emit rateChanged(rate());
	}
}

unsigned short qxgeditVibra::rate (void) const
{
	return m_iRate;
}

void qxgeditVibra::setDepth ( unsigned short iDepth )
{
	if (m_iDepth != iDepth) {
		m_iDepth  = iDepth;
		update();
		emit depthChanged(depth());
	}
}

unsigned short qxgeditVibra::depth (void) const
{
	return m_iDepth;
}


// Draw curve.
void qxgeditVibra::paintEvent ( QPaintEvent *pPaintEvent )
{
	QPainter painter(this);

	int h  = height();
	int w  = width();

	int h2 = h >> 1;
	int w2 = w >> 1;
	int w4 = w >> 2;

	int x1 = int((m_iDelay * w2) >> 7) + 6;
	int y1 = h2 + (m_iDepth > 64 ? 0 : int((64 - m_iDepth) * (h - 9)) >> 7);
	int x2 = int(((127 - m_iRate)  * w4) >> 7) + x1;
	int y2 = (m_iDepth < 64 ? 0 : int((m_iDepth - 64) * (h - 9)) >> 7);

	m_poly.putPoints(0, 3,
		0,  y1,
		x1, y1,
		x2, y1 - y2);

//	painter.drawPolyline(m_poly);

	QPainterPath path;
	path.addPolygon(m_poly);
	int dx = ((x2 - x1) << 1) + 1;
	while (x2 < w) {
		x2 += dx;
		y2 = -y2;
		path.lineTo(x2, y1 - y2);
	}
	path.lineTo(x2, h);
	path.lineTo(0, h);

	const QPalette& pal = palette();
	const QColor& rgbLite = (pal.window().color().value() < 0x7f
		? Qt::darkYellow : Qt::yellow);

	QLinearGradient grad(0, 0, w << 1, h << 1);
	grad.setColorAt(0.0f, rgbLite);
	grad.setColorAt(1.0f, Qt::black);

	painter.setBrush(grad);
	painter.drawPath(path);

	painter.setBrush(rgbLite); // pal.midlight().color()
	painter.drawRect(nodeRect(1));
	painter.drawRect(nodeRect(2));

#ifdef CONFIG_DEBUG
	painter.drawText(QFrame::rect(),
		Qt::AlignTop|Qt::AlignHCenter,
		tr("Delay (%1) Rate (%2) Depth (%3)")
		.arg(int(delay()) - 64)
		.arg(int(rate())  - 64)
		.arg(int(depth()) - 64));
#endif

	painter.end();

	QFrame::paintEvent(pPaintEvent);
}


// Draw rectangular point.
QRect qxgeditVibra::nodeRect ( int iNode ) const
{
	const QPoint& pos = m_poly.at(iNode);
	return QRect(pos.x() - 4, pos.y() - 4, 8, 8); 
}


int qxgeditVibra::nodeIndex ( const QPoint& pos ) const
{
	if (nodeRect(2).contains(pos))
		return 2; // Rate/Depth

	if (nodeRect(1).contains(pos))
		return 1; // Delay

	return -1;
}


void qxgeditVibra::dragNode ( const QPoint& pos )
{
	if (m_iDragNode == 2) {
		// Rate/Depth
		int iRate = int(m_iRate)
			- ((pos.x() - m_posDrag.x()) << 7) / (width() >> 1);
		int iDepth = int(m_iDepth)
			- ((pos.y() - m_posDrag.y()) << 7) / (height() >> 1);
		if (iRate < 0) iRate = 0;
		else
		if (iRate > 127) iRate = 127;
		if (iDepth < 0) iDepth = 0;
		else
		if (iDepth > 127) iDepth = 127;
		if (m_iRate  != (unsigned short) iRate ||
			m_iDepth != (unsigned short) iDepth) {
			m_iRate   = (unsigned short) iRate;
			m_iDepth  = (unsigned short) iDepth;
			m_posDrag = pos;
			update();
			emit rateChanged(rate());
			emit depthChanged(depth());
		}
	}
	else
	if (m_iDragNode == 1) {
		// Delay...
		int iDelay = int(m_iDelay)
			+ ((pos.x() - m_posDrag.x()) << 7) / (width() >> 1);
		int iDepth = int(m_iDepth)
			- ((pos.y() - m_posDrag.y()) << 7) / (height() >> 1);
		if (iDelay < 0) iDelay = 0;
		else
		if (iDelay > 127) iDelay = 127;
		if (iDepth < 0) iDepth = 0;
		else
		if (iDepth > 127) iDepth = 127;
		if (m_iDelay != (unsigned short) iDelay ||
			m_iDepth != (unsigned short) iDepth) {
			m_iDelay  = (unsigned short) iDelay;
			m_iDepth  = (unsigned short) iDepth;
			m_posDrag = pos;
			update();
			emit delayChanged(delay());
			emit depthChanged(depth());
		}
	}
	else
	if (nodeIndex(pos) >= 0) {
		setCursor(Qt::PointingHandCursor);
	} else {
		unsetCursor();
	}
}


// Mouse interaction.
void qxgeditVibra::mousePressEvent ( QMouseEvent *pMouseEvent )
{
	if (pMouseEvent->button() == Qt::LeftButton) {
		const QPoint& pos = pMouseEvent->pos();
		int iDragNode = nodeIndex(pos);
		if (iDragNode >= 0) {
			setCursor(iDragNode > 1 ? Qt::SizeAllCursor : Qt::SizeHorCursor);
			m_iDragNode = iDragNode;
			m_posDrag = pos;
		}
	}

	QFrame::mousePressEvent(pMouseEvent);
}


void qxgeditVibra::mouseMoveEvent ( QMouseEvent *pMouseEvent )
{
	dragNode(pMouseEvent->pos());
}


void qxgeditVibra::mouseReleaseEvent ( QMouseEvent *pMouseEvent )
{
	QFrame::mouseReleaseEvent(pMouseEvent);

	dragNode(pMouseEvent->pos());

	if (m_iDragNode >= 0) {
		m_iDragNode = -1;
		unsetCursor();
	}
}


// end of qxgeditVibra.cpp
