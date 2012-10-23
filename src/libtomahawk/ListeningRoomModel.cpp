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

#include "ListeningRoomModel.h"

#include "ListeningRoom.h"
#include "Source.h"
#include "utils/TomahawkUtils.h"

using namespace Tomahawk;

ListeningRoomModel::ListeningRoomModel( QObject* parent )
    : PlayableModel( parent )
{
}

ListeningRoomModel::~ListeningRoomModel()
{
}

void
ListeningRoomModel::loadListeningRoom(const Tomahawk::listeningroom_ptr &room, bool loadEntries)
{
    if ( !m_listeningRoom.isNull() ) //NOTE: LR already loaded, does this ever happen?
    {
        disconnect( m_listeningRoom.data(), SIGNAL( deleted( Tomahawk::listeningroom_ptr ) ),
                    this, SIGNAL( listeningRoomDeleted() ) );
        disconnect( m_listeningRoom.data(), SIGNAL( changed() ),
                    this, SIGNAL( listeningRoomChanged() ) );

    }

    m_isLoading = true;

    if ( loadEntries )
        clear();

    m_listeningRoom = room;
    connect( m_listeningRoom.data(), SIGNAL( deleted( Tomahawk::listeningroom_ptr ) ),
             this, SIGNAL( listeningRoomDeleted() ) );
    connect( m_listeningRoom.data(), SIGNAL( changed() ),
             this, SIGNAL( listeningRoomChanged() ) );

    setReadOnly( !m_listeningRoom->author()->isLocal() );
    setTitle( m_listeningRoom->title() );

    QString age = TomahawkUtils::ageToString( QDateTime::fromTime_t( m_listeningRoom->createdOn() ), true );

    //set the description
    QString desc;
    if ( m_listeningRoom->author()->isLocal() )
        desc = tr( "A room you are hosting, with %1 listeners." )
               .arg( "over 9000" /*placeholder*/);
    else
        desc = tr( "A room hosted by %1, with %2 listeners." )
               .arg( m_listeningRoom->author()->friendlyName() )
               .arg( "over 9000" /*placeholder*/);
    setDescription( desc );

    emit listeningRoomChanged();

    if ( !loadEntries )
    {
        m_isLoading = false;
        return;
    }

    QList< lrentry_ptr > entries = m_listeningRoom->entries();
    foreach ( const lrentry_ptr& p, entries )
        tDebug() << p->guid() << p->query()->track() << p->query()->artist();

    appendEntries( entries );

    m_isLoading = false;
}

