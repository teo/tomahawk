/* === This file is part of Tomahawk Player - <http://tomahawk-player.org> ===
 *
 *   Copyright 2012, Teo Mrnjavac <teo@kde.org>
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

#include "ListeningRoomHeader.h"

#include "ListeningRoomWidget.h"

#include <QBoxLayout>
#include <QLabel>

ListeningRoomHeader::ListeningRoomHeader( ListeningRoomWidget* parent ) :
    BasicHeader( parent )
{
    QLabel* placeholder = new QLabel( this );
    placeholder->setText( "Placeholder!\nListeners go here!");
    placeholder->setStyleSheet( "color: white" );
    m_mainLayout->addWidget( placeholder );
}

ListeningRoomHeader::~ListeningRoomHeader()
{
}

void
ListeningRoomHeader::addListener( const Tomahawk::source_ptr& source )
{
}

void
ListeningRoomHeader::removeListener( const Tomahawk::source_ptr& source )
{
}
