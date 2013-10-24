/* === This file is part of Tomahawk Player - <http://tomahawk-player.org> ===
 *
 *   Copyright 2010-2011, Christian Muehlhaeuser <muesli@tomahawk-player.org>
 *   Copyright 2010-2011, Leo Franchi <lfranchi@kde.org>
 *   Copyright 2010-2012, Jeff Mitchell <jeff@tomahawk-player.org>
 *   Copyright 2012-2013, Teo Mrnjavac <teo@kde.org>
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

#include "Party.h"

#include "database/Database.h"
#include "database/DatabaseCommand_PartyInfo.h"

#include "Pipeline.h"
#include "Source.h"
#include "SourceList.h"
#include "SourcePlaylistInterface.h"
#include "widgets/SourceTreePopupDialog.h"
#include "utils/Logger.h"


using namespace Tomahawk;


Party::Party( const source_ptr& author )
    : m_source( author )
{}


Party::Party( const source_ptr& author,
              const QString& guid,
              const playlist_ptr& basePlaylist )
    : QObject()
    , m_source( author )
    , m_guid( guid )
    , m_playlist( basePlaylist )
    , m_createdOn( 0 ) //will be set later
    , m_currentRow( -1 )
{
    init();
}


Party::~Party()
{}


party_ptr
Party::createFromPlaylist( const playlist_ptr& basePlaylist )
{
    Q_ASSERT( basePlaylist->author()->isLocal() );
    if ( !basePlaylist->author()->isLocal() )
    {
        tDebug() << "Error: cannot create Party from someone else's playlist.";
        return party_ptr();
    }

    party_ptr party( new Party( SourceList::instance()->getLocal(),
                                uuid(),
                                basePlaylist ), &QObject::deleteLater );
    party->setWeakSelf( party.toWeakRef() );

    // Since the listening party isn't added to Source and hooked up to a model yet, we must prepare
    // the dbcmd manually rather than calling pushUpdate().
    DatabaseCommand_PartyInfo* cmd =
            DatabaseCommand_PartyInfo::broadcastParty( basePlaylist->author(), party );
    connect( cmd, SIGNAL( finished() ), party.data(), SIGNAL( created() ) );
    Database::instance()->enqueue( QSharedPointer< DatabaseCommand >( cmd ) );

    party->reportCreated( party );

    //This DBcmd has a postCommitHook in all peers which calls SourceList::createParty and
    //deserializes the variant to it.

    return party;
}


party_ptr Party::createNew( const QString& title, const QList<query_ptr>& queries )
{
    playlist_ptr basePlaylist = Playlist::create( SourceList::instance()->getLocal(),
                                                  uuid(),
                                                  title,
                                                  QString(),
                                                  SourceList::instance()->getLocal()->friendlyName(),
                                                  true,
                                                  queries );

    return createFromPlaylist( basePlaylist );
}


void
Party::init()
{
    m_locallyChanged = false;
    m_deleted = false;
}


party_ptr
Party::load( const QString& guid )
{
    party_ptr p;

    foreach( const Tomahawk::source_ptr& source, SourceList::instance()->sources() )
    {
        if ( !source->hasParty() )
            continue;

        p = source->party();
        if ( p->guid() == guid )
            return p;
    }

    return p;
}


void
Party::remove( const party_ptr& party )
{
    party->aboutToBeDeleted( party );

    DatabaseCommand_PartyInfo* cmd =
            DatabaseCommand_PartyInfo::disbandParty( party->author(), party->guid() );
    Database::instance()->enqueue( QSharedPointer< DatabaseCommand >( cmd ) );
}


void
Party::updateFrom( const party_ptr& other )
{
    Q_ASSERT( !other.isNull() );
    Q_ASSERT( other->guid() == guid() );

    bool isChanged = false;

    if ( m_createdOn != other->m_createdOn )
    {
        setCreatedOn( other->m_createdOn );
        isChanged = true;
    }

    if ( m_currentRow != other->m_currentRow )
    {
        setCurrentRow( other->m_currentRow );
        isChanged = true;
    }

    if ( m_playlist->guid() != other->m_playlist->guid() )
    {
        setPlaylistByGuid( other->m_playlist->guid() );
        isChanged = true;
    }

    m_listenerIds.clear();
    m_listenerIds.append( other->m_listenerIds );
    isChanged = true;

    if ( isChanged )
        emit changed();
}


source_ptr
Party::author() const
{
    return m_source;
}


QString
Party::guid() const
{
    return m_guid;
}


uint
Party::createdOn() const
{
    return m_createdOn;
}


int
Party::currentRow() const
{
    return m_currentRow;
}


playlist_ptr
Party::playlist() const
{
    return m_playlist;
}


void
Party::setGuid( const QString& s )
{
    m_guid = s;
}


void
Party::setCreatedOn( uint createdOn )
{
    m_createdOn = createdOn;
}


void
Party::setListenerIdsV( const QVariantList& v )
{
    m_listenerIds.clear();
    foreach ( const QVariant &e, v )
    {
        QString listenerId = e.toString();

        if ( !listenerId.isEmpty() )
            m_listenerIds << listenerId;
    }

    emit listenersChanged();
}


QVariantList
Party::listenerIdsV() const
{
    QVariantList v;
    foreach ( const QString &e, m_listenerIds )
    {
        v.append( QVariant::fromValue( e ) );
    }
    return v;
}


void
Party::setCurrentRow( int row )
{
    m_currentRow = row;
}


void
Party::setPlaylistByGuid( const QString& guid )
{
    m_playlist = Playlist::get( guid );
}


QString
Party::playlistGuid() const
{
    if ( !m_playlist.isNull() )
        return m_playlist->guid();
    return QString();
}


QStringList
Party::listenerIds() const
{
    return m_listenerIds;
}


void
Party::reportCreated( const party_ptr& self )
{
    Q_ASSERT( self.data() == this );
    setWeakSelf( self.toWeakRef() );
    m_source->setParty( self ); //or not if the guid is the same!
}


void
Party::reportDeleted( const party_ptr& self )
{
    Q_ASSERT( self.data() == this );
    m_source->removeParty();
    emit deleted( self );
}


void
Party::onDeleteResult( SourceTreePopupDialog* dialog )
{
    dialog->deleteLater();

    //TODO: implement!
}


void
Party::addListener( const Tomahawk::source_ptr& listener )
{
    QString listenerId = listener->nodeId();
    if ( m_listenerIds.contains( listenerId ) )
        return;

    m_listenerIds.append( listenerId );
    m_listenerIds.sort();

    pushUpdate();

    connect( listener.data(), SIGNAL( offline() ), this, SLOT( onListenerOffline() ) );

    emit listenersChanged();
}


void
Party::removeListener( const Tomahawk::source_ptr& listener )
{
    QString listenerId = listener->nodeId();
    m_listenerIds.removeAll( listenerId );

    pushUpdate();

    disconnect( listener.data(), SIGNAL( offline() ), this, SLOT( onListenerOffline() ) );

    emit listenersChanged();
}


void
Party::onListenerOffline()
{
    Source* s = qobject_cast< Source* >( sender() );
    if ( s )
    {
        source_ptr sp = SourceList::instance()->get( s->id() );
        Q_ASSERT( !sp.isNull() );
        removeListener( sp );
    }
}


void
Party::setWeakSelf( QWeakPointer< Party > self )
{
    m_weakSelf = self;
}


void
Party::pushUpdate()
{
    Tomahawk::party_ptr thisParty = m_weakSelf.toStrongRef();

    Q_ASSERT( author()->isLocal() );
    if ( !thisParty.isNull() && author()->isLocal() ) //only the DJ can push updates!
    {
        DatabaseCommand_PartyInfo* cmd =
                DatabaseCommand_PartyInfo::broadcastParty( author(), thisParty );
        Database::instance()->enqueue( QSharedPointer< DatabaseCommand >( cmd ) );
    }
}


Tomahawk::playlistinterface_ptr
Party::playlistInterface()
{
    if ( m_playlistInterface.isNull() )
    {
        m_playlistInterface = m_source->playlistInterface();
    }

    return m_playlistInterface;
}
