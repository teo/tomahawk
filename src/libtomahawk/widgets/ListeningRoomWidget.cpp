/*
 *  Copyright 2012, Teo Mrnjavac <teo@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "ListeningRoomWidget.h"

#include "Source.h"

#include <QLabel>

ListeningRoomWidget::ListeningRoomWidget( const Tomahawk::listeningroom_ptr& listeningRoom,
                                          QWidget* parent )
    : QWidget( parent )
    , m_listeningRoom( listeningRoom )
{
    QLabel* label = new QLabel( "lol room", this );
}


QPixmap
ListeningRoomWidget::pixmap() const
{
    if ( m_pixmap.isNull() )
        return ViewPage::pixmap();
    else
        return m_pixmap;
}


QString
ListeningRoomWidget::description() const
{
    //TODO: implement!
    QString name = m_listeningRoom->author()->isLocal() ? "me"
                                                        : m_listeningRoom->author()->friendlyName();
    QString desc = tr( "A room hosted by %1 with %2 listeners." )
                   .arg( name )
                   .arg( "over 9000" );
    return desc;
}
