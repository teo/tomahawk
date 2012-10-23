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
#include "utils/TomahawkUtilsGui.h"
#include "ListeningRoomHeader.h"
#include "playlist/TrackView.h"

#include <QtGui/QIcon>
#include <QtGui/QLabel>
#include <QtGui/QBoxLayout>
#include <QtGui/QListView>

ListeningRoomWidget::ListeningRoomWidget( const Tomahawk::listeningroom_ptr& listeningRoom,
                                          QWidget* parent )
    : QWidget( parent )
    , m_listeningRoom( listeningRoom )
{
    Q_ASSERT( !m_listeningRoom.isNull() );
    setLayout( new QVBoxLayout );

    m_header = new ListeningRoomHeader( this );
    m_body = new QWidget( this );

    layout()->addWidget( m_header );
    layout()->addWidget( m_body );
    TomahawkUtils::unmarginLayout( layout() );

    m_header->setPixmap( QIcon( RESPATH "images/playlist-icon.png" )
                         .pixmap( 64 ) );
    m_header->setCaption( title() );
    m_header->setDescription( description() );

    //TODO: add listeners to header

    QVBoxLayout* bodyLayout = new QVBoxLayout;
    m_body->setLayout( bodyLayout );
    TomahawkUtils::unmarginLayout( bodyLayout );

    QLabel* upcomingTracksLabel = new QLabel( m_body );
    upcomingTracksLabel->setText( "Upcoming tracks:" );
    bodyLayout->addWidget( upcomingTracksLabel );

    m_view = new TrackView( m_body );
    bodyLayout->addWidget( m_view );

    load( m_listeningRoom );
}


QString
ListeningRoomWidget::description() const
{
    //TODO: implement!
    QString desc;
    if ( m_listeningRoom->author()->isLocal() )
        desc = tr( "A room you are hosting, with %1 listeners." )
               .arg( "over 9000" /*placeholder*/);
    else
        desc = tr( "A room hosted by %1, with %2 listeners." )
               .arg( m_listeningRoom->author()->friendlyName() )
               .arg( "over 9000" /*placeholder*/);
    return desc;
}

void
ListeningRoomWidget::load( const Tomahawk::listeningroom_ptr& listeningRoom )
{
    if ( m_listeningRoom != listeningRoom )
        m_listeningRoom = listeningRoom;

    //TODO: load m_listeningRoom contents
}
