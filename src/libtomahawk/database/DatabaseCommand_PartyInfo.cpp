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

#include "DatabaseCommand_PartyInfo.h"

#include "network/Servent.h"
#include "Source.h"
#include "SourceList.h"
#include "Party.h"
#include "utils/Logger.h"

#include <QDateTime>


namespace Tomahawk
{

DatabaseCommand_PartyInfo::DatabaseCommand_PartyInfo( QObject* parent )
    : DatabaseCommandLoggable( parent )
{
    tDebug() << Q_FUNC_INFO << "dbcmd partyinfo received";
}

/*private*/
DatabaseCommand_PartyInfo::DatabaseCommand_PartyInfo( Action action,
                                                                      const source_ptr& author )
    : DatabaseCommandLoggable( author )
    , m_action( action )
{}


DatabaseCommand_PartyInfo*
DatabaseCommand_PartyInfo::PartyInfo( const source_ptr& author,
                                             const party_ptr& party )
{
    DatabaseCommand_PartyInfo* cmd =
            new DatabaseCommand_PartyInfo( Info, author );
    cmd->m_party = party;
    cmd->m_guid = party->guid();

    return cmd;
}

DatabaseCommand_PartyInfo*
DatabaseCommand_PartyInfo::DisbandParty( const Tomahawk::source_ptr& author,
                                                const QString& partyGuid )
{
    DatabaseCommand_PartyInfo* cmd =
            new DatabaseCommand_PartyInfo( Disband, author );
    cmd->m_guid = partyGuid;

    return cmd;
}


void
DatabaseCommand_PartyInfo::exec( DatabaseImpl* lib )
{
    Q_UNUSED( lib );
    Q_ASSERT( !source().isNull() );
    if ( m_action == Info )
    {
        tDebug() << Q_FUNC_INFO << "with action Info";
        Q_ASSERT( !( m_party.isNull() && m_v.isNull() ) );

        uint now = 0;
        if ( m_party.isNull() ) //we don't have the unserialized version, so we're remote
        {
            now = m_v.toMap()[ "createdon" ].toUInt();
        }
        else //we're executing locally
        {
            if ( m_party->createdOn() == 0 ) //creating it locally now
                now = QDateTime::currentDateTime().toTime_t();
            else
                now = m_party->createdOn();
            m_party->setCreatedOn( now );
        }
    }
    else if ( m_action == Disband )
    {
        tDebug() << Q_FUNC_INFO << "with action Disband";
        Q_ASSERT( !m_guid.isEmpty() );
    }
}


QVariant
DatabaseCommand_PartyInfo::partyV() const
{
    if ( m_action == Info && m_v.isNull() ) //this is so that QVariant conversion only happens when serializing the DBcmd
        return QJson::QObjectHelper::qobject2qvariant( (QObject*)m_party.data() );
    else
        return m_v;
}


void
DatabaseCommand_PartyInfo::postCommitHook()
{
    tDebug() << Q_FUNC_INFO << "about to visibly create party.";

    if ( source()->isLocal() )
    {
        Servent::instance()->triggerDBSync();
    }


    if ( m_action == Info )
    {
        if ( m_party.isNull() ) //if I'm not local
        {
            source_ptr src = source();
    #ifndef ENABLE_HEADLESS
            // db commands run on separate threads, which is good!
            // But one must be careful what he does there, so to create something on the main thread,
            // you either emit a signal or do a queued invoke.
            QMetaObject::invokeMethod( SourceList::instance(),
                                      "createParty",
                                       Qt::BlockingQueuedConnection,
                                       QGenericArgument( "Tomahawk::source_ptr", (const void*)&src ),
                                       Q_ARG( QVariant, m_v ) );

    #endif
        }
        else
        {
            m_party->reportCreated( m_party );
        }
    }
    else if ( m_action == Disband )
    {
        if ( source().isNull() )
            return;

        party_ptr party = source()->party( m_guid );
        if ( party.isNull() )
        {
            tDebug() << "The Party does not exist or has already been disbanded.";
            return;
        }
        source()->removeParty( party );
        party->reportDeleted( party );
    }
}

} //ns

