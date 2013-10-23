/* === This file is part of Tomahawk Player - <http://tomahawk-player.org> ===
 *
 *   Copyright 2012 Teo Mrnjavac <teo@kde.org>
 *
 *   Tomahawk is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Tomahawk is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Tomahawk. If not, see <http://www.gnu.org/licenses/>.
 */

#include "UnstyledFrame.h"

#include <QPainter>

UnstyledFrame::UnstyledFrame( QWidget* parent )
    : QWidget( parent )
{
    m_frameColor = Qt::black;
}

void
UnstyledFrame::paintEvent( QPaintEvent* event )
{
    QWidget::paintEvent( event );
    QPainter p;
    p.begin( this );
    p.setPen( m_frameColor );
    p.drawRect( contentsRect() );
    p.end();
}


void UnstyledFrame::setFrameColor(const QColor& color)
{
    m_frameColor = color;
    repaint();
}
