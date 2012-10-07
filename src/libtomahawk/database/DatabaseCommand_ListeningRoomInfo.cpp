/* === This file is part of Tomahawk Player - <http://tomahawk-player.org> ===
 *
 *   Copyright 2010-2011, Christian Muehlhaeuser <muesli@tomahawk-player.org>
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

#include "DatabaseCommand_ListeningRoomInfo.h"

#include "network/Servent.h"
#include "Source.h"
#include "ListeningRoom.h"

#ifndef ENABLE_HEADLESS
#include "ViewManager.h"
#endif


using namespace Tomahawk;


DatabaseCommand_ListeningRoomInfo::DatabaseCommand_ListeningRoomInfo( QObject* parent )
    : DatabaseCommandLoggable( parent )
{
    tDebug() << Q_FUNC_INFO << "dbcmd listeningroominfo received";
}


DatabaseCommand_ListeningRoomInfo::DatabaseCommand_ListeningRoomInfo( const source_ptr& author,
                                                                      const listeningroom_ptr& listeningRoom )
    : DatabaseCommandLoggable( author )
    , m_listeningRoom( listeningRoom )
{}


void
DatabaseCommand_ListeningRoomInfo::exec( DatabaseImpl* lib )
{
    Q_UNUSED( lib );
    Q_ASSERT( !( m_listeningRoom.isNull() && m_v.isNull() ) );
    Q_ASSERT( !source().isNull() );

    tDebug() << Q_FUNC_INFO;

    uint now = 0;
    if ( m_listeningRoom.isNull() ) //we don't have the unserialized version, so we're remote
    {
        now = m_v.toMap()[ "createdon" ].toUInt();
    }
    else //we're executing locally
    {
        now = QDateTime::currentDateTime().toTime_t();
        m_listeningRoom->setCreatedOn( now );
    }

}


QVariant
DatabaseCommand_ListeningRoomInfo::listeningRoomV() const
{
    if ( m_v.isNull() ) //this is so that QVariant conversion only happens when serializing the DBcmd
        return QJson::QObjectHelper::qobject2qvariant( (QObject*)m_listeningRoom.data() );
    else
        return m_v;
}


void
DatabaseCommand_ListeningRoomInfo::postCommitHook()
{
    tDebug() << Q_FUNC_INFO << "about to visibly create room.";

    if ( source()->isLocal() )
    {
        Servent::instance()->triggerDBSync();
    }

    if ( m_listeningRoom.isNull() ) //if I'm not local
    {
        source_ptr src = source();
        //if ( )
#ifndef ENABLE_HEADLESS
        // db commands run on separate threads, which is good!
        // But one must be careful what he does there, so to create something on the main thread,
        // you either emit a signal or do a queued invoke.
        QMetaObject::invokeMethod( ViewManager::instance(),
                                   "createListeningRoom",
                                   Qt::BlockingQueuedConnection,
                                   QGenericArgument( "Tomahawk::source_ptr", (const void*)&src ),
                                   Q_ARG( QVariant, m_v ) );
#endif
    }
    else
    {
        m_listeningRoom->reportCreated( m_listeningRoom );
    }
}
