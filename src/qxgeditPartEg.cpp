// qxgeditPartEg.cpp
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
#include "qxgeditPartEg.h"

#include <QPainter>
#include <QMouseEvent>


//----------------------------------------------------------------------------
// qxgeditPartEg -- Custom widget

// Constructor.
qxgeditPartEg::qxgeditPartEg (
	QWidget *pParent, Qt::WindowFlags wflags )
	: QFrame(pParent, wflags),
		m_iAttack(0), m_iDecay(0), m_iRelease(0),
		m_poly(7), m_iDragNode(-1)
{
	setMouseTracking(true);
	setMinimumSize(QSize(160, 80));

	QFrame::setFrameShape(QFrame::Panel);
	QFrame::setFrameShadow(QFrame::Sunken);
}


// Destructor.
qxgeditPartEg::~qxgeditPartEg (void)
{
}


// Parameter accessors.
void qxgeditPartEg::setAttack ( unsigned short iAttack )
{
	if (m_iAttack != iAttack) {
		m_iAttack  = iAttack;
		update();
		emit attackChanged(attack());
	}
}

unsigned short qxgeditPartEg::attack (void) const
{
	return m_iAttack;
}


void qxgeditPartEg::setDecay ( unsigned short iDecay )
{
	if (m_iDecay != iDecay) {
		m_iDecay  = iDecay;
		update();
		emit decayChanged(decay());
	}
}

unsigned short qxgeditPartEg::decay (void) const
{
	return m_iDecay;
}

void qxgeditPartEg::setRelease ( unsigned short iRelease )
{
	if (m_iRelease != iRelease) {
		m_iRelease  = iRelease;
		update();
		emit releaseChanged(release());
	}
}

unsigned short qxgeditPartEg::release (void) const
{
	return m_iRelease;
}


// Draw curve.
void qxgeditPartEg::paintEvent ( QPaintEvent *pPaintEvent )
{
	QPainter painter(this);

	const int h  = height();
	const int w  = width();

	const int h2 = h >> 1;
	const int w4 = w >> 2;

	const int x1 = int((m_iAttack  * w4) >> 7);
	const int x2 = int((m_iDecay   * w4) >> 7) + x1;
	const int x3 = int((m_iRelease * w4) >> 7) + x2 + w4;

	m_poly.putPoints(0, 7,
		0, h,
		6, h - 6,
		x1 + 6, 6,
		x2 + 6, h2,
		x2 + w4 - 6, h2,
		x3 - 6, h - 6,
		x3, h);

	QPainterPath path;
	path.addPolygon(m_poly);

	const QPalette& pal = palette();
	const bool bDark = (pal.window().color().value() < 0x7f);
	const QColor& rgbLite = (bDark ? Qt::darkYellow : Qt::yellow);
	if (bDark)
		painter.fillRect(0, 0, w, h, pal.dark().color());

	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setPen(bDark ? Qt::gray : Qt::darkGray);

	QLinearGradient grad(0, 0, w << 1, h << 1);
	grad.setColorAt(0.0f, rgbLite);
	grad.setColorAt(1.0f, Qt::black);

	painter.setBrush(grad);
	painter.drawPath(path);

	painter.setBrush(pal.mid().color());
	painter.drawRect(nodeRect(1));
	painter.drawRect(nodeRect(4));
	painter.setBrush(rgbLite); // pal.midlight().color()
	painter.drawRect(nodeRect(2));
	painter.drawRect(nodeRect(3));
	painter.drawRect(nodeRect(5));

#ifdef CONFIG_DEBUG_0
	painter.drawText(QFrame::rect(),
		Qt::AlignTop|Qt::AlignHCenter,
		tr("Attack (%1) Decay (%2) Release (%3)")
		.arg(int(attack())  - 64)
		.arg(int(decay())   - 64)
		.arg(int(release()) - 64));
#endif

	painter.setRenderHint(QPainter::Antialiasing, false);
	painter.end();

	QFrame::paintEvent(pPaintEvent);
}


// Draw rectangular point.
QRect qxgeditPartEg::nodeRect ( int iNode ) const
{
	const QPoint& pos = m_poly.at(iNode);
	return QRect(pos.x() - 4, pos.y() - 4, 8, 8); 
}


int qxgeditPartEg::nodeIndex ( const QPoint& pos ) const
{
	if (nodeRect(5).contains(pos))
		return 5; // Release

	if (nodeRect(3).contains(pos))
		return 3; // Decay

	if (nodeRect(2).contains(pos))
		return 2; // Attack

	return -1;
}


void qxgeditPartEg::dragNode ( const QPoint& pos )
{
	unsigned short *piValue = NULL;
	switch (m_iDragNode) {
	case 2: // Attack
		piValue = &m_iAttack;
		break;
	case 3: // Decay
		piValue = &m_iDecay;
		break;
	case 5: // Release
		piValue = &m_iRelease;
		break;
	}

	if (piValue) {
		int iValue = int(*piValue)
			+ ((pos.x() - m_posDrag.x()) << 7) / (width() >> 2);
		if (iValue < 0) iValue = 0;
		else
		if (iValue > 127) iValue = 127;
		if (*piValue != (unsigned short) iValue) {
			m_posDrag = pos;
			switch (m_iDragNode) {
			case 2: // Attack
				setAttack(iValue);
				break;
			case 3: // Decay
				setDecay(iValue);
				break;
			case 5: // Release
				setRelease(iValue);
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
void qxgeditPartEg::mousePressEvent ( QMouseEvent *pMouseEvent )
{
	if (pMouseEvent->button() == Qt::LeftButton) {
		const QPoint& pos = pMouseEvent->pos();
		const int iDragNode = nodeIndex(pos);
		if (iDragNode >= 0) {
			setCursor(Qt::SizeHorCursor);
			m_iDragNode = iDragNode;
			m_posDrag = pos;
		}
	}

	QFrame::mousePressEvent(pMouseEvent);
}


void qxgeditPartEg::mouseMoveEvent ( QMouseEvent *pMouseEvent )
{
	dragNode(pMouseEvent->pos());
}


void qxgeditPartEg::mouseReleaseEvent ( QMouseEvent *pMouseEvent )
{
	QFrame::mouseReleaseEvent(pMouseEvent);

	dragNode(pMouseEvent->pos());

	if (m_iDragNode >= 0) {
		m_iDragNode = -1;
		unsetCursor();
	}
}


// end of qxgeditPartEg.cpp
