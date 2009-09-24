// qxgeditAmpEg.cpp
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
#include "qxgeditAmpEg.h"

#include <QPainter>
#include <QMouseEvent>


//----------------------------------------------------------------------------
// qxgeditAmpEg -- Custom widget

// Constructor.
qxgeditAmpEg::qxgeditAmpEg (
	QWidget *pParent, Qt::WindowFlags wflags )
	: QFrame(pParent, wflags),
		m_iAttack(0), m_iRelease(0),
		m_iDecay1(0), m_iDecay2(0),
		m_iLevel1(0), m_iLevel2(0),
		m_poly(8), m_iDragNode(-1)
{
	setMouseTracking(true);
	setMinimumSize(QSize(160, 80));

	QFrame::setFrameShape(QFrame::Panel);
	QFrame::setFrameShadow(QFrame::Sunken);
}


// Destructor.
qxgeditAmpEg::~qxgeditAmpEg (void)
{
}


// Parameter accessors.
void qxgeditAmpEg::setAttack ( unsigned short iAttack )
{
	if (m_iAttack != iAttack) {
		m_iAttack  = iAttack;
		update();
		emit attackChanged(attack());
	}
}

unsigned short qxgeditAmpEg::attack (void) const
{
	return m_iAttack;
}


void qxgeditAmpEg::setRelease ( unsigned short iRelease )
{
	if (m_iRelease != iRelease) {
		m_iRelease  = iRelease;
		update();
		emit releaseChanged(release());
	}
}

unsigned short qxgeditAmpEg::release (void) const
{
	return m_iRelease;
}


void qxgeditAmpEg::setDecay1 ( unsigned short iDecay1 )
{
	if (m_iDecay1 != iDecay1) {
		m_iDecay1  = iDecay1;
		update();
		emit decay1Changed(decay1());
	}
}

unsigned short qxgeditAmpEg::decay1 (void) const
{
	return m_iDecay1;
}


void qxgeditAmpEg::setDecay2 ( unsigned short iDecay2 )
{
	if (m_iDecay2 != iDecay2) {
		m_iDecay2  = iDecay2;
		update();
		emit decay2Changed(decay2());
	}
}

unsigned short qxgeditAmpEg::decay2 (void) const
{
	return m_iDecay2;
}


void qxgeditAmpEg::setLevel1 ( unsigned short iLevel1 )
{
	if (m_iLevel1 != iLevel1) {
		m_iLevel1  = iLevel1;
		if (m_iLevel2 > iLevel1)
			setLevel2(iLevel1);
		else
			update();
		emit level1Changed(level1());
	}
}

unsigned short qxgeditAmpEg::level1 (void) const
{
	return m_iLevel1;
}


void qxgeditAmpEg::setLevel2 ( unsigned short iLevel2 )
{
	if (m_iLevel2 != iLevel2) {
		m_iLevel2  = iLevel2;
		if (m_iLevel1 < iLevel2)
			setLevel1(iLevel2);
		else
			update();
		emit level2Changed(level2());
	}
}

unsigned short qxgeditAmpEg::level2 (void) const
{
	return m_iLevel2;
}


// Draw curve.
void qxgeditAmpEg::paintEvent ( QPaintEvent *pPaintEvent )
{
	QPainter painter(this);

	int h  = height();
	int w  = width();

	int w5 = (w - 3) / 5;

	int x1 = int(((63 - m_iAttack)  * w5) >> 6) + 6;
	int x2 = int(((63 - m_iDecay1)  * w5) >> 6) + x1;
	int x3 = int(((63 - m_iDecay2)  * w5) >> 6) + x2;
	int x4 = int(((63 - m_iRelease) * w5) >> 6) + x3 + w5;

	int y2 = h - int(((m_iLevel1 + 1) * (h - 12)) >> 7) - 6;
	int y3 = h - int(((m_iLevel2 + 1) * (h - 12)) >> 7) - 6;

	m_poly.putPoints(0, 8,
		0, h,
		6,  h - 6,
		x1, 6,
		x2, y2,
		x3, y3,
		x3 + w5 - 6, y3,
		x4 - 6, h - 6,
		x4, h);

	QPainterPath path;
	path.addPolygon(m_poly);

	const QPalette& pal = palette();
	const bool bDark = (pal.window().color().value() < 0x7f);
	const QColor& rgbLite = (bDark ? Qt::darkYellow : Qt::yellow);
	if (bDark)
		painter.fillRect(0, 0, w, h, pal.dark().color());

	painter.setPen(bDark ? Qt::gray : Qt::darkGray);
//	painter.drawPolyline(m_poly);

	QLinearGradient grad(0, 0, w << 1, h << 1);
	grad.setColorAt(0.0f, rgbLite);
	grad.setColorAt(1.0f, Qt::black);

	painter.setBrush(grad);
	painter.drawPath(path);

	painter.setBrush(pal.mid().color());
	painter.drawRect(nodeRect(1));
	painter.drawRect(nodeRect(5));
	painter.setBrush(rgbLite); // pal.midlight().color()
	painter.drawRect(nodeRect(2));
	painter.drawRect(nodeRect(3));
	painter.drawRect(nodeRect(4));
	painter.drawRect(nodeRect(6));

#ifdef CONFIG_DEBUG_0
	painter.drawText(QFrame::rect(),
		Qt::AlignTop|Qt::AlignHCenter,
		tr("Attack (%1) Decay(%2,%3) Level(%4,%5) Release (%6)")
		.arg(int(attack())
		.arg(int(decay1())
		.arg(int(decay2())
		.arg(int(level1()))
		.arg(int(level2()))
		.arg(int(release()));
#endif

	painter.end();

	QFrame::paintEvent(pPaintEvent);
}


// Draw rectangular point.
QRect qxgeditAmpEg::nodeRect ( int iNode ) const
{
	const QPoint& pos = m_poly.at(iNode);
	return QRect(pos.x() - 4, pos.y() - 4, 8, 8); 
}


int qxgeditAmpEg::nodeIndex ( const QPoint& pos ) const
{
	if (nodeRect(6).contains(pos))
		return 6; // Release

	if (nodeRect(4).contains(pos))
		return 4; // Decay2/Level1

	if (nodeRect(3).contains(pos))
		return 3; // Decay1/Level1

	if (nodeRect(2).contains(pos))
		return 2; // Attack

	return -1;
}


void qxgeditAmpEg::dragNode ( const QPoint& pos )
{
	static unsigned short m_iLevel0 = 0; // Dummy!
	unsigned short *piRate  = NULL;
	unsigned short *piLevel = NULL;
	switch (m_iDragNode) {
	case 2: // Attack
		piRate  = &m_iAttack;
		piLevel = &m_iLevel0;
		break;
	case 3: // Decay1/Level1
		piRate  = &m_iDecay1;
		piLevel = &m_iLevel1;
		break;
	case 4: // Decay2/Level1
		piRate  = &m_iDecay2;
		piLevel = &m_iLevel2;
		break;
	case 6: // Release
		piRate  = &m_iRelease;
		piLevel = &m_iLevel0;
		break;
	}

	if (piRate && piLevel) {
		int iRate = int(*piRate)
			- ((pos.x() - m_posDrag.x()) << 6) / (width() >> 2);
		int iLevel = int(*piLevel)
			+ ((m_posDrag.y() - pos.y()) << 7) / height();
		if (iLevel < 0) iLevel = 0;
		else
		if (iLevel > 127) iLevel = 127;
		if (iRate < 0) iRate = 0;
		else
		if (iRate > 63) iRate = 63;
		if (*piRate  != (unsigned short) iRate ||
			*piLevel != (unsigned short) iLevel) {
			m_posDrag = pos;
			switch (m_iDragNode) {
			case 2: // Attack
				setAttack(iRate);
				break;
			case 3: // Decay1/Level1
				setDecay1(iRate);
				setLevel1(iLevel);
				break;
			case 4: // Decay2/Level1
				setDecay2(iRate);
				setLevel2(iLevel);
				break;
			case 6: // Release
				setRelease(iRate);
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
void qxgeditAmpEg::mousePressEvent ( QMouseEvent *pMouseEvent )
{
	if (pMouseEvent->button() == Qt::LeftButton) {
		const QPoint& pos = pMouseEvent->pos();
		int iDragNode = nodeIndex(pos);
		if (iDragNode >= 0) {
			setCursor(iDragNode == 3 || iDragNode == 4
				? Qt::SizeAllCursor
				: Qt::SizeHorCursor);
			m_iDragNode = iDragNode;
			m_posDrag = pos;
		}
	}

	QFrame::mousePressEvent(pMouseEvent);
}


void qxgeditAmpEg::mouseMoveEvent ( QMouseEvent *pMouseEvent )
{
	dragNode(pMouseEvent->pos());
}


void qxgeditAmpEg::mouseReleaseEvent ( QMouseEvent *pMouseEvent )
{
	QFrame::mouseReleaseEvent(pMouseEvent);

	dragNode(pMouseEvent->pos());

	if (m_iDragNode >= 0) {
		m_iDragNode = -1;
		unsetCursor();
	}
}


// end of qxgeditAmpEg.cpp
