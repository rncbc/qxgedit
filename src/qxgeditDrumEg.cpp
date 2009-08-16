// qxgeditDrumEg.cpp
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
#include "qxgeditDrumEg.h"

#include <QPainter>
#include <QMouseEvent>


//----------------------------------------------------------------------------
// qxgeditDrumEg -- Custom widget

// Constructor.
qxgeditDrumEg::qxgeditDrumEg (
	QWidget *pParent, Qt::WindowFlags wflags )
	: QFrame(pParent, wflags),
		m_iAttack(0), m_iDecay1(0), m_iDecay2(0),
		m_poly(6), m_iDragNode(-1)
{
	setMouseTracking(true);
	setMinimumSize(QSize(160, 80));

	QFrame::setFrameShape(QFrame::Panel);
	QFrame::setFrameShadow(QFrame::Sunken);
}


// Destructor.
qxgeditDrumEg::~qxgeditDrumEg (void)
{
}


// Parameter accessors.
void qxgeditDrumEg::setAttack ( unsigned short iAttack )
{
	if (m_iAttack != iAttack) {
		m_iAttack  = iAttack;
		update();
		emit attackChanged(attack());
	}
}

unsigned short qxgeditDrumEg::attack (void) const
{
	return m_iAttack;
}


void qxgeditDrumEg::setDecay1 ( unsigned short iDecay1 )
{
	if (m_iDecay1 != iDecay1) {
		m_iDecay1  = iDecay1;
		update();
		emit decay1Changed(decay1());
	}
}

unsigned short qxgeditDrumEg::decay1 (void) const
{
	return m_iDecay1;
}


void qxgeditDrumEg::setDecay2 ( unsigned short iDecay2 )
{
	if (m_iDecay2 != iDecay2) {
		m_iDecay2  = iDecay2;
		update();
		emit decay2Changed(decay2());
	}
}

unsigned short qxgeditDrumEg::decay2 (void) const
{
	return m_iDecay2;
}


// Draw curve.
void qxgeditDrumEg::paintEvent ( QPaintEvent *pPaintEvent )
{
	QPainter painter(this);

	int h  = height();
	int w  = width();

	int h2 = h >> 1;
	int w3 = (w / 3) - 3;

	int x1 = int((m_iAttack * w3) >> 7) + 6;
	int x2 = int((m_iDecay1 * w3) >> 7) + x1;
	int x3 = int((m_iDecay2 * w3) >> 7) + x2;

	m_poly.putPoints(0, 6,
		0, h,
		6, h - 6,
		x1, 6,
		x2, h2,
		x3, h - 6,
		x3 + 6, h);

	QPainterPath path;
	path.addPolygon(m_poly);

	const QPalette& pal = palette();
	const QColor& rgbLite = Qt::darkYellow;

	painter.fillRect(0, 0, w, h, pal.dark().color());
//	painter.drawPolyline(m_poly);

	QLinearGradient grad(0, 0, w << 1, h << 1);
	grad.setColorAt(0.0f, rgbLite);
	grad.setColorAt(1.0f, Qt::black);

	painter.setBrush(grad);
	painter.drawPath(path);

	painter.setBrush(rgbLite); // pal.midlight().color()
	painter.drawRect(nodeRect(2));
	painter.drawRect(nodeRect(3));
	painter.drawRect(nodeRect(4));
	painter.setBrush(pal.mid().color());
	painter.drawRect(nodeRect(1));

#ifdef CONFIG_DEBUG_0
	painter.drawText(QFrame::rect(),
		Qt::AlignTop|Qt::AlignHCenter,
		tr("Attack (%1) Decay1 (%2) Decay2 (%3)")
		.arg(int(attack()) - 64)
		.arg(int(decay1()) - 64)
		.arg(int(decay2()) - 64));
#endif

	painter.end();

	QFrame::paintEvent(pPaintEvent);
}


// Draw rectangular point.
QRect qxgeditDrumEg::nodeRect ( int iNode ) const
{
	const QPoint& pos = m_poly.at(iNode);
	return QRect(pos.x() - 4, pos.y() - 4, 8, 8); 
}


int qxgeditDrumEg::nodeIndex ( const QPoint& pos ) const
{
	if (nodeRect(2).contains(pos))
		return 2; // Attack

	if (nodeRect(3).contains(pos))
		return 3; // Decay1

	if (nodeRect(4).contains(pos))
		return 4; // Decay2

	return -1;
}


void qxgeditDrumEg::dragNode ( const QPoint& pos )
{
	unsigned short *piValue = NULL;
	switch (m_iDragNode) {
	case 2: // Attack
		piValue = &m_iAttack;
		break;
	case 3: // Decay1
		piValue = &m_iDecay1;
		break;
	case 4: // Decay2
		piValue = &m_iDecay2;
		break;
	}

	if (piValue) {
		int iValue = int(*piValue)
			+ ((pos.x() - m_posDrag.x()) << 7) / (width() >> 2);
		if (iValue < 0) iValue = 0;
		else
		if (iValue > 127) iValue = 127;
		if (*piValue != (unsigned short) iValue) {
			*piValue  = (unsigned short) iValue;
			m_posDrag = pos;
			update();
			switch (m_iDragNode) {
			case 2: // Attack
				emit attackChanged(attack());
				break;
			case 3: // Decay1
				emit decay1Changed(decay1());
				break;
			case 4: // Decay2
				emit decay2Changed(decay2());
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
void qxgeditDrumEg::mousePressEvent ( QMouseEvent *pMouseEvent )
{
	if (pMouseEvent->button() == Qt::LeftButton) {
		const QPoint& pos = pMouseEvent->pos();
		int iDragNode = nodeIndex(pos);
		if (iDragNode >= 0) {
			setCursor(Qt::SizeHorCursor);
			m_iDragNode = iDragNode;
			m_posDrag = pos;
		}
	}

	QFrame::mousePressEvent(pMouseEvent);
}


void qxgeditDrumEg::mouseMoveEvent ( QMouseEvent *pMouseEvent )
{
	dragNode(pMouseEvent->pos());
}


void qxgeditDrumEg::mouseReleaseEvent ( QMouseEvent *pMouseEvent )
{
	QFrame::mouseReleaseEvent(pMouseEvent);

	dragNode(pMouseEvent->pos());

	if (m_iDragNode >= 0) {
		m_iDragNode = -1;
		unsetCursor();
	}
}


// end of qxgeditDrumEg.cpp
