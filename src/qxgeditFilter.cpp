// qxgeditFilter.cpp
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

#include "qxgeditFilter.h"

#include <QPainter>
#include <QMouseEvent>


//----------------------------------------------------------------------------
// qxgeditFilter -- Custom widget

// Constructor.
qxgeditFilter::qxgeditFilter (
	QWidget *pParent, Qt::WindowFlags wflags )
	: QFrame(pParent, wflags),
		m_iFreq(0), m_iReso(0),
		m_bDragging(false)
{
	setMinimumSize(QSize(120, 80));
}


// Destructor.
qxgeditFilter::~qxgeditFilter (void)
{
}


// Parameter accessors.
void qxgeditFilter::setFreq ( unsigned short iFreq )
{
	if (m_iFreq != iFreq) {
		m_iFreq  = iFreq;
		update();
		emit freqChanged(freq());
	}
}

unsigned short qxgeditFilter::freq (void) const
{
	return m_iFreq;
}

void qxgeditFilter::setReso ( unsigned short iReso )
{
	if (m_iReso != iReso) {
		m_iReso  = iReso;
		update();
		emit resoChanged(reso());
	}
}

unsigned short qxgeditFilter::reso (void) const
{
	return m_iReso;
}


// Draw curve.
void qxgeditFilter::paintEvent ( QPaintEvent *pPaintEvent )
{
	QPainter painter(this);

	int h  = height();
	int w  = width();

	int h2 = h >> 1;
	int h4 = h >> 2;
	int w4 = w >> 2;
	int w8 = w >> 3;

	int x = w8 + int((m_iFreq * (w - w4)) >> 7);
	int y = h2 - int((m_iReso * (h + h4)) >> 7);

	QPolygon poly(6);
	poly.putPoints(0, 6,
		0,      h2,
		x - w8, h2,
		x,      h2,
		x,      y,
		x + w8, h,
		0,      h);

	QPainterPath path;
	path.moveTo(poly.at(0));
	path.lineTo(poly.at(1));
	path.cubicTo(poly.at(2), poly.at(3), poly.at(4));
	path.lineTo(poly.at(5));

	const QPalette& pal = palette();
	const QColor& rgbLite = (pal.window().color().value() < 0x7f
		? Qt::darkYellow : Qt::yellow);

	QLinearGradient grad(0, 0, w << 1, h << 1);
	grad.setColorAt(0.0f, rgbLite);
	grad.setColorAt(1.0f, Qt::black);

	painter.setBrush(grad);
	painter.drawPath(path);

	painter.end();

	QFrame::paintEvent(pPaintEvent);

#ifdef CONFIG_DEBUG
	painter.drawText(QFrame::rect(),
		Qt::AlignTop|Qt::AlignHCenter,
		tr("Cutoff-Frequency (%1) Resonance (%2)")
		.arg(int(freq()) - 64)
		.arg(int(reso()) - 64));
#endif
}


// Drag/move curve.
void qxgeditFilter::dragCurve ( const QPoint& pos )
{
	int iFreq = int(m_iFreq)
		+ ((pos.x() - m_posDrag.x()) << 7) / width();
	int iReso = int(m_iReso)
		+ ((m_posDrag.y() - pos.y()) << 7) / height();

	if (iFreq < 0) iFreq = 0;
	else
	if (iFreq > 127) iFreq = 127;

	if (iReso < 0) iReso = 0;
	else
	if (iReso > 127) iReso = 127;

	if (m_iFreq == (unsigned short) iFreq &&
		m_iReso == (unsigned short) iReso)
		return;

	m_iFreq = (unsigned short) iFreq;
	m_iReso = (unsigned short) iReso;

	m_posDrag = pos;

	update();

	emit freqChanged(freq());
	emit resoChanged(reso());
}


// Mouse interaction.
void qxgeditFilter::mousePressEvent ( QMouseEvent *pMouseEvent )
{
	if (pMouseEvent->button() == Qt::LeftButton)
		m_posDrag = pMouseEvent->pos();

	QFrame::mousePressEvent(pMouseEvent);

}


void qxgeditFilter::mouseMoveEvent ( QMouseEvent *pMouseEvent )
{
	const QPoint& pos = pMouseEvent->pos();
	if (m_bDragging) {
		dragCurve(pos);
	} else { // if ((pos - m_posDrag).manhattanLength() > 4)
		setCursor(Qt::SizeAllCursor);
		m_bDragging = true;
	}
}


void qxgeditFilter::mouseReleaseEvent ( QMouseEvent *pMouseEvent )
{
	QFrame::mouseReleaseEvent(pMouseEvent);

	if (m_bDragging) {
		dragCurve(pMouseEvent->pos());
		m_bDragging = false;
		unsetCursor();
	}
}


// end of qxgeditFilter.cpp
