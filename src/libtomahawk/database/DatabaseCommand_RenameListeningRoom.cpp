/* === This file is part of Tomahawk Player - <http://tomahawk-player.org> ===
 *
 *   Copyright 2010-2011, Christian Muehlhaeuser <muesli@tomahawk-player.org>
 *   Copyright 2012,      Teo Mrnjavac <teo@kde.org>
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

#include "DatabaseCommand_RenameListeningRoom.h"

#include "DatabaseImpl.h"
#include "Source.h"
#include "network/Servent.h"
#include "utils/Logger.h"
#include "ListeningRoom.h"

using namespace Tomahawk;


DatabaseCommand_RenameListeningRoom::DatabaseCommand_RenameListeningRoom( const source_ptr& source, const QString& listeningRoomGuid, const QString& listeningRoomTitle )
    : DatabaseCommandLoggable( source )
{
    setListeningRoomguid( listeningRoomGuid );
    setListeningRoomTitle( listeningRoomTitle );
}


void
DatabaseCommand_RenameListeningRoom::exec( DatabaseImpl* lib )
{
    Q_UNUSED( lib );
}


void
DatabaseCommand_RenameListeningRoom::postCommitHook()
{
    if( source()->isLocal() )
        Servent::instance()->triggerDBSync();

    listeningroom_ptr listeningRoom = source()->listeningRoom( m_listeningRoomGuid );

    if ( listeningRoom.isNull() )
         return;

    qDebug() << "Renaming old listening room " << listeningRoom->title() << " to " << m_listeningRoomTitle << m_listeningRoomGuid << ".";
    listeningRoom->setTitle( m_listeningRoomTitle );
}
