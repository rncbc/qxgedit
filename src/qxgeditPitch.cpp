// qxgeditPitch.cpp
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
#include "qxgeditPitch.h"

#include <QPainter>
#include <QMouseEvent>


//----------------------------------------------------------------------------
// qxgeditPitch -- Custom widget

// Constructor.
qxgeditPitch::qxgeditPitch (
	QWidget *pParent, Qt::WindowFlags wflags )
	: QFrame(pParent, wflags),
		m_iAttackTime(0), m_iAttackLevel(0),
		m_iReleaseTime(0), m_iReleaseLevel(0),
		m_poly(4), m_iDragNode(-1)
{
	setMouseTracking(true);
	setMinimumSize(QSize(160, 80));

	QFrame::setFrameShape(QFrame::Panel);
	QFrame::setFrameShadow(QFrame::Sunken);
}


// Destructor.
qxgeditPitch::~qxgeditPitch (void)
{
}


// Parameter accessors.
void qxgeditPitch::setAttackTime ( unsigned short iAttackTime )
{
	if (m_iDragNode >= 0)
		return;

	if (m_iAttackTime != iAttackTime) {
		m_iAttackTime  = iAttackTime;
		update();
		emit attackTimeChanged(attackTime());
	}
}

unsigned short qxgeditPitch::attackTime (void) const
{
	return m_iAttackTime;
}


void qxgeditPitch::setAttackLevel ( unsigned short iAttackLevel )
{
	if (m_iDragNode >= 0)
		return;

	if (m_iAttackLevel != iAttackLevel) {
		m_iAttackLevel  = iAttackLevel;
		update();
		emit attackLevelChanged(attackLevel());
	}
}

unsigned short qxgeditPitch::attackLevel (void) const
{
	return m_iAttackLevel;
}


void qxgeditPitch::setReleaseTime ( unsigned short iReleaseTime )
{
	if (m_iDragNode >= 0)
		return;

	if (m_iReleaseTime != iReleaseTime) {
		m_iReleaseTime  = iReleaseTime;
		update();
		emit releaseTimeChanged(releaseTime());
	}
}

unsigned short qxgeditPitch::releaseTime (void) const
{
	return m_iReleaseTime;
}


void qxgeditPitch::setReleaseLevel ( unsigned short iReleaseLevel )
{
	if (m_iDragNode >= 0)
		return;

	if (m_iReleaseLevel != iReleaseLevel) {
		m_iReleaseLevel  = iReleaseLevel;
		update();
		emit releaseLevelChanged(releaseLevel());
	}
}

unsigned short qxgeditPitch::releaseLevel (void) const
{
	return m_iReleaseLevel;
}


// Draw curve.
void qxgeditPitch::paintEvent ( QPaintEvent *pPaintEvent )
{
	QPainter painter(this);

	int h  = height();
	int w  = width();

	int h2 = h >> 1;
	int w4 = w >> 2;

	int x2 = w / 3;
	int x3 = x2 << 1;

	int x1 = x2 - int((m_iAttackTime * w4) >> 7);
	int y1 = h2 - ((int(m_iAttackLevel) - 64) * (h - 12)) / 128;
	int x4 = x3 + int((m_iReleaseTime * w4) >> 7);
	int y2 = h2 - ((int(m_iReleaseLevel) - 64) * (h - 12)) / 128;

	m_poly.putPoints(0, 4,
		x1, y1,
		x2, h2,
		x3, h2,
		x4, y2);

	const QPalette& pal = palette();
	const bool bDark = (pal.window().color().value() < 0x7f);
	const QColor& rgbLite = (bDark ? Qt::darkYellow : Qt::yellow);
	if (bDark)
		painter.fillRect(0, 0, w, h, pal.dark().color());
	painter.drawPolyline(m_poly);

	painter.setBrush(rgbLite); // pal.midlight().color()
	painter.drawRect(nodeRect(0));
	painter.drawRect(nodeRect(3));
	painter.setBrush(pal.mid().color());
	painter.drawRect(nodeRect(1));
	painter.drawRect(nodeRect(2));

#ifdef CONFIG_DEBUG_0
	painter.drawText(QFrame::rect(),
		Qt::AlignTop|Qt::AlignHCenter,
		tr("Attack (%1,%2) Release (%3,%4)")
		.arg(int(attackTime())   - 64)
		.arg(int(attackLevel())  - 64)
		.arg(int(releaseTime())  - 64)
		.arg(int(releaseLevel()) - 64));
#endif

	painter.end();

	QFrame::paintEvent(pPaintEvent);
}


// Draw rectangular point.
QRect qxgeditPitch::nodeRect ( int iNode ) const
{
	const QPoint& pos = m_poly.at(iNode);
	return QRect(pos.x() - 4, pos.y() - 4, 8, 8); 
}


int qxgeditPitch::nodeIndex ( const QPoint& pos ) const
{
	if (nodeRect(0).contains(pos))
		return 0; // Attack

	if (nodeRect(3).contains(pos))
		return 3; // Release

	return -1;
}


void qxgeditPitch::dragNode ( const QPoint& pos )
{
	unsigned short *piTime  = NULL;
	unsigned short *piLevel = NULL;
	switch (m_iDragNode) {
	case 0: // Attack
		piLevel = &m_iAttackLevel;
		piTime  = &m_iAttackTime;
		break;
	case 3: // Release
		piLevel = &m_iReleaseLevel;
		piTime  = &m_iReleaseTime;
		break;
	}

	if (piTime && piLevel) {
		int iTime = int(*piTime)
			+ (m_iDragNode == 0 ? -1 : +1)
			* ((pos.x() - m_posDrag.x()) << 7) / (width() >> 2);
		int iLevel = int(*piLevel)
			+ ((m_posDrag.y() - pos.y()) << 7) / height();
		if (iTime < 0) iTime = 0;
		else
		if (iTime > 127) iTime = 127;
		if (iLevel < 0) iLevel = 0;
		else
		if (iLevel > 127) iLevel = 127;
		if (*piTime  != (unsigned short) iTime ||
			*piLevel != (unsigned short) iLevel) {
			*piTime   = iTime;
			*piLevel  = iLevel;
			m_posDrag = pos;
			update();
			switch (m_iDragNode) {
			case 0: // Attack
				emit attackTimeChanged(attackTime());
				emit attackLevelChanged(attackLevel());
				break;
			case 3: // Release
				emit releaseTimeChanged(releaseTime());
				emit releaseLevelChanged(releaseLevel());
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
void qxgeditPitch::mousePressEvent ( QMouseEvent *pMouseEvent )
{
	if (pMouseEvent->button() == Qt::LeftButton) {
		const QPoint& pos = pMouseEvent->pos();
		int iDragNode = nodeIndex(pos);
		if (iDragNode >= 0) {
			setCursor(Qt::SizeAllCursor);
			m_iDragNode = iDragNode;
			m_posDrag = pos;
		}
	}

	QFrame::mousePressEvent(pMouseEvent);
}


void qxgeditPitch::mouseMoveEvent ( QMouseEvent *pMouseEvent )
{
	dragNode(pMouseEvent->pos());
}


void qxgeditPitch::mouseReleaseEvent ( QMouseEvent *pMouseEvent )
{
	QFrame::mouseReleaseEvent(pMouseEvent);

	dragNode(pMouseEvent->pos());

	if (m_iDragNode >= 0) {
		m_iDragNode = -1;
		unsetCursor();
	}
}


// end of qxgeditPitch.cpp
