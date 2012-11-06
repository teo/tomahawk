/* === This file is part of Tomahawk Player - <http://tomahawk-player.org> ===
 *
 *   Copyright 2010-2011, Christian Muehlhaeuser <muesli@tomahawk-player.org>
 *   Copyright 2010-2011, Leo Franchi <lfranchi@kde.org>
 *   Copyright 2010-2012, Jeff Mitchell <jeff@tomahawk-player.org>
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

#include "ListeningRoom.h"

#include "database/Database.h"
#include "database/DatabaseCommand_ListeningRoomInfo.h"
#include "database/DatabaseCommand_RenameListeningRoom.h"

#include "Pipeline.h"
#include "Source.h"
#include "SourceList.h"
#include "SourcePlaylistInterface.h"
#include "widgets/SourceTreePopupDialog.h"


using namespace Tomahawk;


// ListeningRoomEntry


void
ListeningRoomEntry::setQueryVariant( const QVariant& v )
{
    QVariantMap m = v.toMap();

    QString artist = m.value( "artist" ).toString();
    QString album = m.value( "album" ).toString();
    QString track = m.value( "track" ).toString();

    m_query = Tomahawk::Query::get( artist, track, album );
}


QVariant
ListeningRoomEntry::queryVariant() const
{
    return m_query->toVariant();
}


// ListeningRoom


ListeningRoom::ListeningRoom( const source_ptr& author )
    : m_source( author )
    , m_lastmodified( 0 )
{}


ListeningRoom::ListeningRoom( const source_ptr& author,
                              const QString& guid,
                              const QString& title,
                              const QString& creator,
                              const QList< Tomahawk::lrentry_ptr >& entries )
    : QObject()
    , m_source( author )
    , m_guid( guid )
    , m_title( title )
    , m_creator( creator )
    , m_lastmodified( 0 )
    , m_createdOn( 0 ) //will be set later
    , m_initEntries( entries )
{
    init();
}


ListeningRoom::~ListeningRoom()
{}


void
ListeningRoom::init()
{
    m_locallyChanged = false;
    m_deleted = false;
    connect( Pipeline::instance(), SIGNAL( idle() ), SLOT( onResolvingFinished() ) );
}


listeningroom_ptr
ListeningRoom::create( const source_ptr& author,
                       const QString& guid,
                       const QString& title,
                       const QString& creator,
                       const QList< Tomahawk::query_ptr >& queries )
{
    QList< lrentry_ptr > entries;
    foreach( const Tomahawk::query_ptr& query, queries )
    {
        lrentry_ptr p( new ListeningRoomEntry );
        p->setGuid( uuid() );
        p->setDuration( query->duration() );
        p->setLastmodified( 0 );
        p->setQuery( query );

        entries << p;
    }

    listeningroom_ptr listeningRoom( new ListeningRoom( author, guid, title, creator, entries ), &QObject::deleteLater );
    listeningRoom->setWeakSelf( listeningRoom.toWeakRef() );

    DatabaseCommand_ListeningRoomInfo* cmd =
            DatabaseCommand_ListeningRoomInfo::RoomInfo( author, listeningRoom );
    connect( cmd, SIGNAL( finished() ), listeningRoom.data(), SIGNAL( created() ) );
    Database::instance()->enqueue( QSharedPointer< DatabaseCommand >( cmd ) );
    listeningRoom->reportCreated( listeningRoom );

    //This DBcmd has a postCommitHook in all peers which calls ViewManager::createListeningRoom and
    //deserializes the variant to it.

    return listeningRoom;
}


listeningroom_ptr
ListeningRoom::load( const QString& guid )
{
    listeningroom_ptr p;

    foreach( const Tomahawk::source_ptr& source, SourceList::instance()->sources() )
    {
        p = source->listeningRoom( guid );
        if ( !p.isNull() )
            return p;
    }

    return p;
}


void
ListeningRoom::remove( const listeningroom_ptr& room )
{
    room->aboutToBeDeleted( room );

    DatabaseCommand_ListeningRoomInfo* cmd =
            DatabaseCommand_ListeningRoomInfo::DisbandRoom( room->author(), room->guid() );
    Database::instance()->enqueue( QSharedPointer< DatabaseCommand >( cmd ) );
}


void
ListeningRoom::rename( const QString& title )
{
    if ( title != m_title )
    {
        DatabaseCommand_RenameListeningRoom* cmd =
                new DatabaseCommand_RenameListeningRoom( author(), guid(), title );
        Database::instance()->enqueue( QSharedPointer< DatabaseCommand >( cmd ) );
    }
}

void
ListeningRoom::updateFrom( const listeningroom_ptr& other )
{
    Q_ASSERT( !other.isNull() );
    Q_ASSERT( other->guid() == guid() );

    bool isChanged = false;

    if ( m_title != other->m_title )
    {
        QString oldTitle = m_title;
        m_title = other->m_title;
        emit renamed( m_title, oldTitle );
        isChanged = true;
    }

    if ( m_creator != other->m_creator )
    {
        setCreator( other->creator() );
        isChanged = true;
    }

    if ( m_createdOn != other->m_createdOn )
    {
        setCreatedOn( other->m_createdOn );
        isChanged = true;
    }

    //TODO: don't reload if not needed
    m_entries.clear();
    m_entries.append( other->m_entries );
    isChanged = true;

    if ( isChanged )
        emit changed();
}


void
ListeningRoom::setTitle( const QString& title )
{
    if ( title == m_title )
        return;

    const QString oldTitle = m_title;
    m_title = title;
    emit changed();
    emit renamed( m_title, oldTitle );
}

void
ListeningRoom::setEntriesV( const QVariantList& l )
{
    m_entries.clear();
    foreach ( const QVariant& e, l )
    {
        ListeningRoomEntry* lre = new ListeningRoomEntry;
        QJson::QObjectHelper::qvariant2qobject( e.toMap(), lre );

        if ( lre->isValid() )
            m_entries << lrentry_ptr( lre );
    }
}


void
ListeningRoom::reportCreated( const listeningroom_ptr& self )
{
    Q_ASSERT( self.data() == this );
    m_source->addListeningRoom( self ); //or not add if the guid is the same!
}


void
ListeningRoom::reportDeleted( const listeningroom_ptr& self )
{
    Q_ASSERT( self.data() == this );
    m_source->removeListeningRoom( self );
    emit deleted( self );
}


void
ListeningRoom::onDeleteResult( SourceTreePopupDialog* dialog )
{
    dialog->deleteLater();

    //TODO: implement!
}


void
ListeningRoom::resolve()
{
    QList< query_ptr > qlist;
    foreach( const lrentry_ptr& p, m_entries )
    {
        qlist << p->query();
    }

    Pipeline::instance()->resolve( qlist );
}


void
ListeningRoom::onResultsChanged()
{
    m_locallyChanged = true;
}


void
ListeningRoom::onResolvingFinished()
{
    if ( m_locallyChanged && !m_deleted )
    {
        m_locallyChanged = false;
        pushUpdate();
    }
}


void
ListeningRoom::addEntry( const query_ptr& query )
{
    QList< query_ptr > queries;
    queries << query;

    addEntries( queries );
}


void
ListeningRoom::addEntries( const QList< query_ptr >& queries )
{
    Q_ASSERT( m_source->isLocal() );
    QList< lrentry_ptr > el = entriesFromQueries( queries, true /*only return entries for these new queries*/ );

    const int prevSize = m_entries.size();

    m_entries.append( el );

    pushUpdate();

    //We are appending at end, so notify listeners.
    qDebug() << "ListeningRoom got" << queries.size() << "tracks added, emitting tracksInserted with:" << el.size() << "at pos:" << prevSize - 1;

    emit tracksInserted( el, prevSize );
}


void
ListeningRoom::insertEntries( const QList< query_ptr >& queries,
                              const int position )
{
    QList< lrentry_ptr > toInsert = entriesFromQueries( queries, true );

    Q_ASSERT( position <= m_entries.size() );
    if ( position > m_entries.size() )
    {
        qWarning() << "ERROR trying to insert tracks past end of playlist! Appending!!";
        addEntries( queries );
        return;
    }

    for ( int i = toInsert.size()-1; i >= 0; --i )
        m_entries.insert( position, toInsert.at( i ) );

    pushUpdate();

    //Notify listeners.
    qDebug() << "ListeningRoom got" << toInsert.size() << "tracks inserted, emitting tracksInserted at pos:" << position;
    emit tracksInserted( toInsert, position );
}


void
ListeningRoom::pushUpdate()
{
    Tomahawk::listeningroom_ptr me =
            SourceList::instance()->getLocal()->listeningRoom( guid() );

    if ( !me.isNull() )
    {
        DatabaseCommand_ListeningRoomInfo* cmd =
                DatabaseCommand_ListeningRoomInfo::RoomInfo( author(), me );
        Database::instance()->enqueue( QSharedPointer< DatabaseCommand >( cmd ) );
    }
}


QList< lrentry_ptr >
ListeningRoom::entriesFromQueries( const QList< Tomahawk::query_ptr >& queries, bool clearFirst )
{
    QList< lrentry_ptr > el;
    if ( !clearFirst )
        el = entries();

    foreach( const query_ptr& query, queries )
    {
        lrentry_ptr e( new ListeningRoomEntry() );
        e->setGuid( uuid() );
        e->setDuration( query->displayQuery()->duration() );
        e->setLastmodified( 0 );
        e->setQuery( query );
        el << e;
    }

    return el;
}


Tomahawk::playlistinterface_ptr
ListeningRoom::playlistInterface()
{
    if ( m_playlistInterface.isNull() )
    {
        m_playlistInterface = m_source->playlistInterface();
    }

    return m_playlistInterface;
}