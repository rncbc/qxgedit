// qxgeditFilter.cpp
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
#include "qxgeditFilter.h"

#include <QPainter>
#include <QMouseEvent>


//----------------------------------------------------------------------------
// qxgeditFilter -- Custom widget

// Constructor.
qxgeditFilter::qxgeditFilter (
	QWidget *pParent, Qt::WindowFlags wflags )
	: QFrame(pParent, wflags),
		m_iCutoff(0), m_iResonance(0),
		m_iMaxCutoff(127), m_iMaxResonance(127),
		m_bDragging(false)
{
	setMinimumSize(QSize(160, 80));

	QFrame::setFrameShape(QFrame::Panel);
	QFrame::setFrameShadow(QFrame::Sunken);
}


// Destructor.
qxgeditFilter::~qxgeditFilter (void)
{
}


// Parameter accessors.
void qxgeditFilter::setCutoff ( unsigned short iCutoff )
{
	if (iCutoff > m_iMaxCutoff)
		iCutoff = m_iMaxCutoff;

	if (m_iCutoff != iCutoff) {
		m_iCutoff  = iCutoff;
		update();
		emit cutoffChanged(cutoff());
	}
}

unsigned short qxgeditFilter::cutoff (void) const
{
	return m_iCutoff;
}

void qxgeditFilter::setResonance ( unsigned short iResonance )
{
	if (iResonance > m_iMaxResonance)
		iResonance = m_iMaxResonance;

	if (m_iResonance != iResonance) {
		m_iResonance  = iResonance;
		update();
		emit resonanceChanged(resonance());
	}
}

unsigned short qxgeditFilter::resonance (void) const
{
	return m_iResonance;
}


// Parameter range modifiers.
void qxgeditFilter::setMaxCutoff ( unsigned short iMaxCutoff )
{
	m_iMaxCutoff = iMaxCutoff;
}

unsigned short qxgeditFilter::maxCutoff (void) const
{
	return m_iMaxCutoff;
}

void qxgeditFilter::setMaxResonance ( unsigned short iMaxResonance )
{
	m_iMaxResonance = iMaxResonance;
}

unsigned short qxgeditFilter::maxResonance (void) const
{
	return m_iMaxResonance;
}


// Draw curve.
void qxgeditFilter::paintEvent ( QPaintEvent *pPaintEvent )
{
	QPainter painter(this);

	const int h  = height();
	const int w  = width();

	const int h2 = h >> 1;
	const int h4 = h >> 2;
	const int w4 = w >> 2;
	const int w8 = w >> 3;

	const int x = w8 + int((m_iCutoff * (w - w4)) / (m_iMaxCutoff + 1));
	const int y = h2 - int((m_iResonance * (h + h4)) / (m_iMaxResonance + 1));

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

#ifdef CONFIG_DEBUG_0
	painter.drawText(QFrame::rect(),
		Qt::AlignTop|Qt::AlignHCenter,
		tr("Cutoff (%1) Resonance (%2)")
		.arg(int(cutoff()))
		.arg(int(resonance())));
#endif

	painter.setRenderHint(QPainter::Antialiasing, false);
	painter.end();

	QFrame::paintEvent(pPaintEvent);
}


// Drag/move curve.
void qxgeditFilter::dragCurve ( const QPoint& pos )
{
	int iCutoff = int(m_iCutoff)
		+ ((pos.x() - m_posDrag.x()) * (m_iMaxCutoff + 1) * 3) / (width() << 1);
	int iResonance = int(m_iResonance)
		+ ((m_posDrag.y() - pos.y()) * (m_iMaxResonance + 1)) / (height() >> 1);

	if (iCutoff < 0) iCutoff = 0;
	else
	if (iCutoff > m_iMaxCutoff) iCutoff = m_iMaxCutoff;

	if (iResonance < 0) iResonance = 0;
	else
	if (iResonance > m_iMaxResonance) iResonance = m_iMaxResonance;

	if (m_iCutoff    != (unsigned short) iCutoff ||
		m_iResonance != (unsigned short) iResonance) {
		m_posDrag = pos;
		setCutoff(iCutoff);
		setResonance(iResonance);
	}
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
