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

#include "PartyModel.h"

#include "Album.h"
#include "Artist.h"
#include "DropJob.h"
#include "Party.h"
#include "Pipeline.h"
#include "playlist/PlayableItem.h"
#include "PlaylistInterface.h"
#include "Source.h"
#include "SourceList.h"
#include "utils/TomahawkUtils.h"
#include "utils/Logger.h"

#include <QDateTime>
#include <QMimeData>

using namespace Tomahawk;


PartyModel::PartyModel( QObject* parent )
    : PlayableModel( parent )
    , m_changesOngoing( false )
    , m_isLoading( false )
    , m_savedInsertPos( -1 )
{
    m_dropStorage.parent = QPersistentModelIndex();
    m_dropStorage.row = -10;

    setReadOnly( true );
}


PartyModel::~PartyModel()
{
}


QMimeData*
PartyModel::mimeData( const QModelIndexList& indexes ) const
{
    QMimeData* d = PlayableModel::mimeData( indexes );
    if ( !m_party.isNull() )
        d->setData( "application/tomahawk.party.id", m_party->guid().toLatin1() );

    return d;
}


bool
PartyModel::dropMimeData( const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent )
{
    Q_UNUSED( column );

    if ( action == Qt::IgnoreAction || isReadOnly() )
        return true;

    if ( !DropJob::acceptsMimeData( data ) )
        return false;

    m_dropStorage.row = row;
    m_dropStorage.parent = QPersistentModelIndex( parent );
    m_dropStorage.action = action;

    DropJob* dj = new DropJob();

    if ( !DropJob::acceptsMimeData( data, DropJob::Track | DropJob::Playlist | DropJob::Album | DropJob::Artist ) )
        return false;

    dj->setDropTypes( DropJob::Track | DropJob::Playlist | DropJob::Album | DropJob::Artist );
    dj->setDropAction( DropJob::Append );

#ifdef Q_WS_MAC
    // On mac, drags from outside the app are still Qt::MoveActions instead of Qt::CopyAction by default
    // so check if the drag originated in this party to determine whether or not to copy
    if ( !data->hasFormat( "application/tomahawk.party.id" ) ||
       ( !m_party.isNull() && data->data( "application/tomahawk.party.id" ) != m_party->guid() ) )
    {
        dj->setDropAction( DropJob::Append );
    }
#endif

    connect( dj, SIGNAL( tracks( QList< Tomahawk::query_ptr > ) ),
             SLOT( parsedDroppedTracks( QList< Tomahawk::query_ptr > ) ) );
    dj->tracksFromMimeData( data );

    return true;
}


void
PartyModel::parsedDroppedTracks( QList< query_ptr > tracks )
{
    if ( m_dropStorage.row == -10 ) //null value
        return;

    int beginRow;
    if ( m_dropStorage.row != -1 )
        beginRow = m_dropStorage.row;
    else if ( m_dropStorage.parent.isValid() )
        beginRow = m_dropStorage.parent.row();
    else
        beginRow = rowCount( QModelIndex() ); //append

    if ( tracks.count() )
    {
        bool update = ( m_dropStorage.action & Qt::CopyAction ||
                        m_dropStorage.action & Qt::MoveAction );
        if ( update )
            beginRoomChanges();

        insertQueries( tracks, beginRow );

        if ( update && m_dropStorage.action & Qt::CopyAction )
            endRoomChanges();
    }

    m_dropStorage.parent = QPersistentModelIndex();
    m_dropStorage.row = -10;
}


void
PartyModel::beginRoomChanges()
{
    if ( m_party.isNull() || !m_party->author()->isLocal() )
        return;

    Q_ASSERT( !m_changesOngoing );
    m_changesOngoing = true;
}


void
PartyModel::endRoomChanges()
{
    if ( m_party.isNull() || !m_party->author()->isLocal() )
        return;

    if ( m_changesOngoing )
    {
        m_changesOngoing = false;
    }
    else
    {
        tDebug() << "Called" << Q_FUNC_INFO << "unexpectedly!";
        Q_ASSERT( false );
    }


    if ( m_savedInsertPos >= 0 && !m_savedInsertTracks.isEmpty() &&
         !m_savedRemoveTracks.isEmpty() )
    {
        // If we have *both* an insert and remove, then it's a move action
        // However, since we got the insert before the remove (Qt...), the index we have as the saved
        // insert position is no longer valid. Find the proper one by finding the location of the first inserted
        // track
        for ( int i = 0; i < rowCount( QModelIndex() ); i++ )
        {
            const QModelIndex idx = index( i, 0, QModelIndex() );
            if ( !idx.isValid() )
                continue;
            const PlayableItem* item = itemFromIndex( idx );
            if ( !item || item->lrentry().isNull() )
                continue;

//             qDebug() << "Checking for equality:" << (item->lrentry() == m_savedInsertTracks.first()) << m_savedInsertTracks.first()->query()->track() << m_savedInsertTracks.first()->query()->artist();
            if ( item->lrentry() == m_savedInsertTracks.first() )
            {
                // Found our index
                m_party->moveEntries( m_savedInsertTracks, i );
                break;
            }
        }
        m_savedInsertPos = -1;
        m_savedInsertTracks.clear();
        m_savedRemoveTracks.clear();
    }
    else if ( m_savedInsertPos >= 0 ) //only insertion
    {
        QList< query_ptr > qs;
        foreach ( const lrentry_ptr& e, m_savedInsertTracks )
        {
            qs.append( e->query() );
        }

        m_party->insertEntries( qs, m_savedInsertPos );
        m_savedInsertPos = -1;
        m_savedInsertTracks.clear();
    }
    else if ( !m_savedRemoveTracks.isEmpty() )
    {
        m_party->removeEntries( m_savedRemoveTracks );
        m_savedRemoveTracks.clear();
    }
}


QList< lrentry_ptr >
PartyModel::partyEntries() const
{
    QList< lrentry_ptr > l;
    for ( int i = 0; i < rowCount( QModelIndex() ); ++i )
    {
        QModelIndex idx = index( i, 0, QModelIndex() );
        if ( !idx.isValid() )
            continue;

        PlayableItem* item = itemFromIndex( idx );
        if ( item )
            l << item->lrentry();
    }

    return l;
}


void
PartyModel::loadParty( const Tomahawk::party_ptr& party, bool loadEntries )
{
    if ( !m_party.isNull() ) //NOTE: LR already loaded, does this ever happen?
    {
        disconnect( m_party.data(), SIGNAL( deleted( Tomahawk::party_ptr ) ),
                    this, SIGNAL( partyDeleted() ) );
        disconnect( m_party.data(), SIGNAL( changed() ),
                    this, SLOT( reload() ) );
        disconnect( m_party.data(), SIGNAL( listenersChanged() ),
                    this, SLOT( reloadRoomMetadata() ) );
    }

    m_isLoading = true;

    if ( loadEntries )
        clear();

    m_party = party;
    connect( m_party.data(), SIGNAL( deleted( Tomahawk::party_ptr ) ),
             this, SIGNAL( partyDeleted() ) );
    connect( m_party.data(), SIGNAL( changed() ),
             this, SLOT( reload() ) );
    connect( m_party.data(), SIGNAL( listenersChanged() ),
             this, SLOT( reloadRoomMetadata() ) );

    setReadOnly( !m_party->author()->isLocal() );

    reloadRoomMetadata();

    m_isLoading = false;

    if ( !loadEntries )
    {
        return;
    }

    reload();
}


void
PartyModel::reloadRoomMetadata()
{
    setTitle( m_party->title() );
    QString age = TomahawkUtils::ageToString( QDateTime::fromTime_t( m_party->createdOn() ), true );

    //set the description
    QString desc;
    int n = m_party->listenerIds().count();
    if ( m_party->author()->isLocal() )
        desc = tr( "A party you are hosting, with %n listener(s).", "", n );
    else
        desc = tr( "A party hosted by %1, with %n listener(s).", "", n )
               .arg( m_party->author()->friendlyName() );
    setDescription( desc );

    emit listenersChanged();
}

void
PartyModel::reload()
{
    m_isLoading = true;

    reloadRoomMetadata();

    QList< lrentry_ptr > entries = m_party->entries();
    foreach ( const lrentry_ptr& p, entries )
        tDebug() << p->guid() << p->query()->track()->track() << p->query()->track()->artist();

    // Since LR dbcmds are all singletons, they give all listeners the complete LR data every time,
    // even if the actual list of tracks has not changed.
    // To avoid unnecessary model/view reload, we make sure that we only reload if stuff has changed.
    // It is an expensive O(n) entry-by-entry search for changes, but it surely beats clearing and
    // reloading the whole model and view every time.
    bool hasChanged = false;
    if ( entries.count() != rowCount( QModelIndex() ) )
    {
        hasChanged = true;
    }
    else //could be unchanged
    {
        for ( int i = 0; i < entries.count(); ++i )
        {
            QModelIndex idx = index( i, 0, QModelIndex() );
            if ( !idx.isValid() )
            {
                hasChanged = true;
                break;
            }

            PlayableItem* item = itemFromIndex( idx );
            if ( item && !item->lrentry().isNull() )
            {
                if ( item->lrentry()->query() != entries.at( i )->query() )
                {
                    hasChanged = true;
                    break;
                }
            }
            else
            {
                hasChanged = true;
                break;
            }
        }
    }

    if ( hasChanged )
    {
        clear();
        appendEntries( entries ); //emits signals and stuff
    }

    bool hasRowChanged = false;
    if ( currentItem().row() != m_party->currentRow() )
    {
        hasRowChanged = true;
        int row = m_party->currentRow();
        if ( row >= 0 && row < rowCount( QModelIndex() ) )
            setCurrentItem( index( row, 0, QModelIndex() ) );
    }

    if ( hasChanged || hasRowChanged )
        emit currentRowChanged();

    m_isLoading = false;
}


void
PartyModel::clear()
{
    PlayableModel::clear();
    m_waitingForResolved.clear();
}


void
PartyModel::appendEntries( const QList< lrentry_ptr >& entries )
{
    insertEntriesPrivate( entries, rowCount( QModelIndex() ) );
}


void
PartyModel::insertAlbums( const QList< album_ptr >& albums, int row )
{
    foreach ( const album_ptr& album, albums )
    {
        if ( album.isNull() )
            return;

        connect( album.data(), SIGNAL( tracksAdded( QList< Tomahawk::query_ptr >, Tomahawk::ModelMode, Tomahawk::collection_ptr ) ),
                                 SLOT( appendQueries( QList< Tomahawk::query_ptr > ) ) );

        appendQueries( album->playlistInterface( Mixed )->tracks() );
    }
}


void
PartyModel::insertArtists( const QList< artist_ptr >& artists, int row )
{
    foreach ( const artist_ptr& artist, artists )
    {
        if ( artist.isNull() )
            return;

        connect( artist.data(), SIGNAL( tracksAdded( QList< Tomahawk::query_ptr >, Tomahawk::ModelMode, Tomahawk::collection_ptr ) ),
                                  SLOT( appendQueries( QList< Tomahawk::query_ptr > ) ) );

        appendQueries( artist->playlistInterface( Mixed )->tracks() );
    }
}


void
PartyModel::insertQueries( const QList< query_ptr >& queries, int row )
{
    QList< Tomahawk::lrentry_ptr > entries;
    foreach ( const query_ptr& query, queries )
    {
        lrentry_ptr entry = lrentry_ptr( new PartyEntry() );

        entry->setDuration( query->queryTrack()->duration() );
        entry->setLastmodified( 0 );
        //TODO: annotations? do we need them here?
        entry->setQuery( query );
        entry->setGuid( uuid() );

        entries << entry;
    }

    insertEntriesPrivate( entries, row );
}


void
PartyModel::insertEntriesFromView( const QList< lrentry_ptr >& entries, int row )
{
    if ( entries.isEmpty() || !m_party->author()->isLocal() )
        return;

    beginRoomChanges();
    insertEntriesPrivate( entries, row );
    endRoomChanges();
}


void
PartyModel::insertEntriesPrivate( const QList< lrentry_ptr >& entries, int row )
{
    if ( !entries.count() )
    {
        emit itemCountChanged( rowCount( QModelIndex() ) );
        finishLoading();
        return;
    }

    int c = row;
    QPair< int, int > crows;
    crows.first = c;
    crows.second = c + entries.count() - 1;

    if ( !m_isLoading )
    {
        m_savedInsertPos = row;
        m_savedInsertTracks = entries;
    }

    emit beginInsertRows( QModelIndex(), crows.first, crows.second );

    QList< Tomahawk::query_ptr > queries;
    int i = 0;
    PlayableItem* plitem;
    foreach( const lrentry_ptr& entry, entries )
    {
        plitem = new PlayableItem( entry, rootItem(), row + i );
        plitem->index = createIndex( row + i, 0, plitem );
        i++;

        if ( !entry->query()->resolvingFinished() && !entry->query()->playable() )
        {
            queries << entry->query();
            m_waitingForResolved.append( entry->query().data() );
            connect( entry->query().data(), SIGNAL( resolvingFinished( bool ) ), SLOT( trackResolved( bool ) ) );
        }

        connect( plitem, SIGNAL( dataChanged() ), SLOT( onDataChanged() ) );
    }

    if ( !m_waitingForResolved.isEmpty() )
    {
        Pipeline::instance()->resolve( queries );
        emit loadingStarted();
    }

    emit endInsertRows();
    emit itemCountChanged( rowCount( QModelIndex() ) );
    finishLoading();
}


void
PartyModel::trackResolved( bool )
{
    Query* q = qobject_cast< Query* >( sender() );
    if ( !q )
        return;

    if ( m_waitingForResolved.contains( q ) )
    {
        m_waitingForResolved.removeAll( q );
        disconnect( q, SIGNAL( resolvingFinished( bool ) ),
                    this, SLOT( trackResolved( bool ) ) );
    }

    if ( m_waitingForResolved.isEmpty() )
    {
        emit loadingFinished();
    }
}


void
PartyModel::removeIndex( const QModelIndex& index, bool moreToCome )
{
    PlayableItem* item = itemFromIndex( index );

    //if removing something not resolved, let's terminate the query resolution first
    if ( item && m_waitingForResolved.contains( item->query().data() ) )
    {
        disconnect( item->query().data(), SIGNAL( resolvingFinished( bool ) ),
                    this, SLOT( trackResolved( bool ) ) );

        m_waitingForResolved.removeAll( item->query().data() );
        if ( m_waitingForResolved.isEmpty() )
            emit loadingFinished();
    }

    if ( !m_changesOngoing )
        beginRoomChanges();

    if ( item && !m_isLoading )
        m_savedRemoveTracks << item->lrentry();

    PlayableModel::removeIndex( index, moreToCome );

    if ( !moreToCome )
        endRoomChanges();
}


void
PartyModel::setCurrentItem( const QModelIndex& index )
{
    PlayableModel::setCurrentIndex( index );
    if ( !m_party.isNull() && m_party->author() == SourceList::instance()->getLocal() )
    {
        m_party->setCurrentRow( index.row() );
        m_party->pushUpdate();
    }
}
