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

#include "ListeningRoomItem.h"

#include <QMimeData>

#include "DropJob.h"
#include "ListeningRoom.h"
#include "SourcesModel.h"
#include "Query.h"
#include "ViewManager.h"
#include "ViewPage.h"
#include "utils/Logger.h"


ListeningRoomItem::ListeningRoomItem( SourcesModel* mdl,
                                      SourceTreeItem* parent,
                                      const Tomahawk::listeningroom_ptr& lr,
                                      int index )
    : SourceTreeItem( mdl, parent, SourcesModel::ListeningRoom, index )
    , m_listeningroom( lr )
{
    tDebug() << Q_FUNC_INFO << "le wild Listening Room item appears.";
    Q_ASSERT( lr );

    m_icon = QIcon( RESPATH "images/listeningroom.png" );

    connect( lr.data(), SIGNAL( changed() ), SLOT( onUpdated() ), Qt::QueuedConnection );

    if ( ViewManager::instance()->pageForListeningRoom( lr ) )
        model()->linkSourceItemToPage( this, ViewManager::instance()->pageForListeningRoom( lr ) );
}


QString
ListeningRoomItem::text() const
{
    return tr( "%1 (%2)", "the name of a listening room, followed by the current host inside ()" )
            .arg( m_listeningroom->title() )
            .arg( m_listeningroom->author()->friendlyName() );
}

QString
ListeningRoomItem::editorText() const
{
    return m_listeningroom->title();
}


Tomahawk::listeningroom_ptr
ListeningRoomItem::listeningroom() const
{
    return m_listeningroom;
}


Qt::ItemFlags
ListeningRoomItem::flags() const
{
    Qt::ItemFlags flags = SourceTreeItem::flags();
    flags |= Qt::ItemIsEditable;
    return flags;
}


bool
ListeningRoomItem::willAcceptDrag( const QMimeData* data ) const
{
    return !m_listeningroom.isNull() &&
           m_listeningroom->author()->isLocal() &&
           DropJob::acceptsMimeData( data, DropJob::Track ) /*&&
           !m_listeningroom->busy()*/;
    //FIXME: use busy or not!
}


ListeningRoomItem::DropTypes
ListeningRoomItem::supportedDropTypes( const QMimeData* data ) const
{
    //same as PlaylistItem::supportedDropTypes
    if ( data->hasFormat( "application/tomahawk.mixed" ) )
    {
        // If this is mixed but only queries/results, we can still handle them
        bool mixedQueries = true;

        QByteArray itemData = data->data( "application/tomahawk.mixed" );
        QDataStream stream( &itemData, QIODevice::ReadOnly );
        QString mimeType;
        qlonglong val;

        while ( !stream.atEnd() )
        {
            stream >> mimeType;
            if ( mimeType != "application/tomahawk.query.list" &&
                 mimeType != "application/tomahawk.result.list" )
            {
                mixedQueries = false;
                break;
            }
            stream >> val;
        }

        if ( mixedQueries )
            return DropTypeThisTrack | DropTypeThisAlbum | DropTypeAllFromArtist | DropTypeLocalItems | DropTypeTop50;
        else
            return DropTypesNone;
    }

    if ( data->hasFormat( "application/tomahawk.query.list" ) )
        return DropTypeThisTrack | DropTypeThisAlbum | DropTypeAllFromArtist | DropTypeLocalItems | DropTypeTop50;
    else if ( data->hasFormat( "application/tomahawk.result.list" ) )
        return DropTypeThisTrack | DropTypeThisAlbum | DropTypeAllFromArtist | DropTypeLocalItems | DropTypeTop50;
    else if ( data->hasFormat( "application/tomahawk.metadata.album" ) )
        return DropTypeThisAlbum | DropTypeAllFromArtist | DropTypeLocalItems | DropTypeTop50;
    else if ( data->hasFormat( "application/tomahawk.metadata.artist" ) )
        return DropTypeAllFromArtist | DropTypeLocalItems | DropTypeTop50;
    else if ( data->hasFormat( "text/plain" ) )
    {
        return DropTypesNone;
    }
    return DropTypesNone;
}


bool
ListeningRoomItem::dropMimeData( const QMimeData* data, Qt::DropAction action )
{
    tDebug() << Q_FUNC_INFO;
    Q_UNUSED( action );

//    if ( m_listeningroom->busy() )
//        return false;

    QList< Tomahawk::query_ptr > queries;

    if ( data->hasFormat( "application/tomahawk.listeningroom.id" ) &&
        data->data( "application/tomahawk.listeningroom.id" ) == m_listeningroom->guid() )
        return false; // don't allow dropping on ourselves

    if ( !DropJob::acceptsMimeData( data, DropJob::Track ) )
        return false;

    DropJob *dj = new DropJob();
    dj->setDropTypes( DropJob::Track );
    dj->setDropAction( DropJob::Append );

    connect( dj, SIGNAL( tracks( QList< Tomahawk::query_ptr > ) ), this, SLOT( parsedDroppedTracks( QList< Tomahawk::query_ptr > ) ) );

    if ( dropType() == DropTypeAllFromArtist )
        dj->setGetWholeArtists( true );
    if ( dropType() == DropTypeThisAlbum )
        dj->setGetWholeAlbums( true );

    if ( dropType() == DropTypeLocalItems )
    {
        dj->setGetWholeArtists( true );
        dj->tracksFromMimeData( data, false, true );
    }
    else if ( dropType() == DropTypeTop50 )
    {
        dj->setGetWholeArtists( true );
        dj->tracksFromMimeData( data, false, false, true );
    }
    else
        dj->tracksFromMimeData( data, false, false );

    // TODO can't know if it works or not yet...
    return true;
}


QIcon
ListeningRoomItem::icon() const
{
    return m_icon;
}


bool
ListeningRoomItem::setData(const QVariant &v, bool role)
{
    Q_UNUSED( role );

    if ( m_listeningroom->author()->isLocal() ) //if this is MY listening room
    {
        m_listeningroom->rename( v.toString() );
        return true;
    }
    return false;
}


int
ListeningRoomItem::peerSortValue() const
{
    return 0;
}


int
ListeningRoomItem::IDValue() const
{
    return m_listeningroom->createdOn();
}


SourceTreeItem*
ListeningRoomItem::activateCurrent()
{
    if ( ViewManager::instance()->pageForListeningRoom( m_listeningroom ) ==
         ViewManager::instance()->currentPage() )
    {
        model()->linkSourceItemToPage( this, ViewManager::instance()->currentPage() );
        emit selectRequest( this );

        return this;
    }

    return 0;
}


void
ListeningRoomItem::activate()
{
    Tomahawk::ViewPage* p = ViewManager::instance()->show( m_listeningroom );
    model()->linkSourceItemToPage( this, p );
}

void
ListeningRoomItem::onUpdated()
{
    emit updated();
}

void
ListeningRoomItem::parsedDroppedTracks( const QList< Tomahawk::query_ptr >& tracks )
{
    qDebug() << "adding" << tracks.count() << "tracks";
    if ( tracks.count() && !m_listeningroom.isNull() && m_listeningroom->author()->isLocal() )
    {
        qDebug() << "on listening room:" << m_listeningroom->title() << m_listeningroom->guid();

        m_listeningroom->addEntries( tracks );
    }
}

