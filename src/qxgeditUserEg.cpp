// qxgeditUserEg.cpp
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

#include "qxgeditAbout.h"
#include "qxgeditUserEg.h"

#include <QPainter>
#include <QMouseEvent>


//----------------------------------------------------------------------------
// qxgeditUserEg -- Custom widget

// Constructor.
qxgeditUserEg::qxgeditUserEg ( QWidget *pParent )
	: QFrame(pParent), m_iLevel0(0),
		m_iLevel1(0), m_iLevel2(0), m_iLevel3(0), m_iLevel4(0),
		m_iRate1(0), m_iRate2(0), m_iRate3(0), m_iRate4(0),
		m_poly(6), m_iDragNode(-1)
{
	setMouseTracking(true);
	setMinimumSize(QSize(160, 80));

	QFrame::setFrameShape(QFrame::Panel);
	QFrame::setFrameShadow(QFrame::Sunken);
}


// Destructor.
qxgeditUserEg::~qxgeditUserEg (void)
{
}


// Parameter accessors.
void qxgeditUserEg::setLevel0 ( unsigned short iLevel0 )
{
	if (m_iLevel0 != iLevel0) {
		m_iLevel0  = iLevel0;
		update();
		emit level0Changed(level0());
	}
}

unsigned short qxgeditUserEg::level0 (void) const
{
	return m_iLevel0;
}


void qxgeditUserEg::setLevel1 ( unsigned short iLevel1 )
{
	if (m_iLevel1 != iLevel1) {
		m_iLevel1  = iLevel1;
		update();
		emit level1Changed(level1());
	}
}

unsigned short qxgeditUserEg::level1 (void) const
{
	return m_iLevel1;
}


void qxgeditUserEg::setLevel2 ( unsigned short iLevel2 )
{
	if (m_iLevel2 != iLevel2) {
		m_iLevel2  = iLevel2;
		update();
		emit level2Changed(level2());
	}
}

unsigned short qxgeditUserEg::level2 (void) const
{
	return m_iLevel2;
}


void qxgeditUserEg::setLevel3 ( unsigned short iLevel3 )
{
	if (m_iLevel3 != iLevel3) {
		m_iLevel3  = iLevel3;
		update();
		emit level3Changed(level3());
	}
}

unsigned short qxgeditUserEg::level3 (void) const
{
	return m_iLevel3;
}


void qxgeditUserEg::setLevel4 ( unsigned short iLevel4 )
{
	if (m_iLevel4 != iLevel4) {
		m_iLevel4  = iLevel4;
		update();
		emit level4Changed(level4());
	}
}

unsigned short qxgeditUserEg::level4 (void) const
{
	return m_iLevel4;
}


void qxgeditUserEg::setRate1 ( unsigned short iRate1 )
{
	if (m_iRate1 != iRate1) {
		m_iRate1  = iRate1;
		update();
		emit rate1Changed(rate1());
	}
}

unsigned short qxgeditUserEg::rate1 (void) const
{
	return m_iRate1;
}


void qxgeditUserEg::setRate2 ( unsigned short iRate2 )
{
	if (m_iRate2 != iRate2) {
		m_iRate2  = iRate2;
		update();
		emit rate2Changed(rate2());
	}
}

unsigned short qxgeditUserEg::rate2 (void) const
{
	return m_iRate2;
}


void qxgeditUserEg::setRate3 ( unsigned short iRate3 )
{
	if (m_iRate3 != iRate3) {
		m_iRate3  = iRate3;
		update();
		emit rate3Changed(rate3());
	}
}

unsigned short qxgeditUserEg::rate3 (void) const
{
	return m_iRate3;
}


void qxgeditUserEg::setRate4 ( unsigned short iRate4 )
{
	if (m_iRate4 != iRate4) {
		m_iRate4  = iRate4;
		update();
		emit rate4Changed(rate4());
	}
}

unsigned short qxgeditUserEg::rate4 (void) const
{
	return m_iRate4;
}


// Draw curve.
void qxgeditUserEg::paintEvent ( QPaintEvent *pPaintEvent )
{
	QPainter painter(this);

	const int h  = height();
	const int w  = width();

	const int h2 = (h >> 1);
	const int w5 = (w - 9) / 5;

	const int x1 = int(((63 - m_iRate1) * w5) >> 6) + 6;
	const int x2 = int(((63 - m_iRate2) * w5) >> 6) + x1;
	const int x3 = int(((63 - m_iRate3) * w5) >> 6) + x2;
	const int x4 = int(((63 - m_iRate4) * w5) >> 6) + x3 + w5;

	const int y0 = h - int((m_iLevel0 * (h - 12)) >> 7) - 6;
	const int y1 = h - int((m_iLevel1 * (h - 12)) >> 7) - 6;
	const int y2 = h - int((m_iLevel2 * (h - 12)) >> 7) - 6;
	const int y3 = h - int((m_iLevel3 * (h - 12)) >> 7) - 6;
	const int y4 = h - int((m_iLevel4 * (h - 12)) >> 7) - 6;

	m_poly.putPoints(0, 6,
		6,  y0,
		x1, y1,
		x2, y2,
		x3, y3,
		x3 + w5, y3,
		x4, y4);

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

	painter.setBrush(pal.mid().color());
	painter.drawRect(nodeRect(4));
	painter.setBrush(rgbLite); // pal.midlight().color()
	painter.drawRect(nodeRect(0));
	painter.drawRect(nodeRect(1));
	painter.drawRect(nodeRect(2));
	painter.drawRect(nodeRect(3));
	painter.drawRect(nodeRect(5));

#ifdef CONFIG_DEBUG_0
	painter.drawText(QFrame::rect(),
		Qt::AlignTop|Qt::AlignHCenter,
		tr("Level (%1,%2,%3,%4,%5) Rate (0,%6,%7,%8,%9)")
		.arg(int(level0()))
		.arg(int(level1()))
		.arg(int(level2()))
		.arg(int(level3()))
		.arg(int(level4()))
		.arg(int(rate1())
		.arg(int(rate2())
		.arg(int(rate3())
		.arg(int(rate4()));
#endif

	painter.setRenderHint(QPainter::Antialiasing, false);
	painter.end();

	QFrame::paintEvent(pPaintEvent);
}


// Draw rectangular point.
QRect qxgeditUserEg::nodeRect ( int iNode ) const
{
	const QPoint& pos = m_poly.at(iNode);
	return QRect(pos.x() - 4, pos.y() - 4, 8, 8); 
}


int qxgeditUserEg::nodeIndex ( const QPoint& pos ) const
{
	if (nodeRect(5).contains(pos))
		return 5; // Level4/Rate4

	if (nodeRect(3).contains(pos))
		return 3; // Level3/Rate3

	if (nodeRect(2).contains(pos))
		return 2; // Level2/Rate2

	if (nodeRect(1).contains(pos))
		return 1; // Level1/Rate1

	if (nodeRect(0).contains(pos))
		return 0; // Level0

	return -1;
}


void qxgeditUserEg::dragNode ( const QPoint& pos )
{
	static unsigned short m_iRate0 = 0; // Dummy!
	unsigned short *piLevel = nullptr;
	unsigned short *piRate  = nullptr;
	switch (m_iDragNode) {
	case 0: // Level0
		piLevel = &m_iLevel0;
		piRate  = &m_iRate0;
		break;
	case 1: // Level1/Rate1
		piLevel = &m_iLevel1;
		piRate  = &m_iRate1;
		break;
	case 2: // Level2/Rate2
		piLevel = &m_iLevel2;
		piRate  = &m_iRate2;
		break;
	case 3: // Level3/Rate3
		piLevel = &m_iLevel3;
		piRate  = &m_iRate3;
		break;
	case 5: // Level4/Rate4
		piLevel = &m_iLevel4;
		piRate  = &m_iRate4;
		break;
	}

	if (piLevel && piRate) {
		int iLevel = int(*piLevel)
			+ ((m_posDrag.y() - pos.y()) << 7) / height();
		int iRate = int(*piRate)
			- ((pos.x() - m_posDrag.x()) << 6) / (width() >> 2);
		if (iLevel < 0) iLevel = 0;
		else
		if (iLevel > 127) iLevel = 127;
		if (iRate < 0) iRate = 0;
		else
		if (iRate > 63) iRate = 63;
		if (*piLevel != (unsigned short) iLevel ||
			*piRate  != (unsigned short) iRate) {
			m_posDrag = pos;
			switch (m_iDragNode) {
			case 0: // Level0
				setLevel0(iLevel);
				break;
			case 1: // Level1/Rate1
				setLevel1(iLevel);
				setRate1(iRate);
				break;
			case 2: // Level2/Rate2
				setLevel2(iLevel);
				setRate2(iRate);
				break;
			case 3: // Level3/Rate3
				setLevel3(iLevel);
				setRate3(iRate);
				break;
			case 5: // Level4/Rate4
				setLevel4(iLevel);
				setRate4(iRate);
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
void qxgeditUserEg::mousePressEvent ( QMouseEvent *pMouseEvent )
{
	if (pMouseEvent->button() == Qt::LeftButton) {
		const QPoint& pos = pMouseEvent->pos();
		const int iDragNode = nodeIndex(pos);
		if (iDragNode >= 0) {
			setCursor(iDragNode == 0
				? Qt::SizeVerCursor
				: Qt::SizeAllCursor);
			m_iDragNode = iDragNode;
			m_posDrag = pos;
		}
	}

	QFrame::mousePressEvent(pMouseEvent);
}


void qxgeditUserEg::mouseMoveEvent ( QMouseEvent *pMouseEvent )
{
	dragNode(pMouseEvent->pos());
}


void qxgeditUserEg::mouseReleaseEvent ( QMouseEvent *pMouseEvent )
{
	QFrame::mouseReleaseEvent(pMouseEvent);

	dragNode(pMouseEvent->pos());

	if (m_iDragNode >= 0) {
		m_iDragNode = -1;
		unsetCursor();
	}
}


// end of qxgeditUserEg.cpp
