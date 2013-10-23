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

#include "ListeningRoomsCategoryItem.h"

#include "ListeningRoom.h"
#include "ListeningRoomItem.h"
#include "SourceList.h"
#include "utils/Closure.h"
#include "utils/Logger.h"


void
GroupCategoryItem::checkExpandedState()
{
    if ( m_defaultExpanded )
    {
        m_defaultExpanded = false;
        requestExpanding();
    }
}


ListeningRoomsCategoryItem::ListeningRoomsCategoryItem( SourcesModel* model,
                                                        SourceTreeItem* parent,
                                                        int peerSortValue )
    : GroupCategoryItem( model, parent, SourcesModel::ListeningRoomsCategory, true, peerSortValue )
{
//useless because it's all empty at startup
    foreach ( const Tomahawk::source_ptr& src, SourceList::instance()->sources( true ) )
    {
        connect( src.data(), SIGNAL( listeningRoomAdded( Tomahawk::listeningroom_ptr ) ),
                 SLOT( onListeningRoomAdded( Tomahawk::listeningroom_ptr ) ), Qt::QueuedConnection );
    }

    connect( SourceList::instance(), SIGNAL( sourceAdded( Tomahawk::source_ptr ) ),
             SLOT( onSourceAdded( Tomahawk::source_ptr ) ) );
}


void
ListeningRoomsCategoryItem::onSourceAdded( const Tomahawk::source_ptr& src )
{
    tDebug() << "Begin" << Q_FUNC_INFO;

    connect( src.data(), SIGNAL( listeningRoomAdded( Tomahawk::listeningroom_ptr ) ),
             SLOT( onListeningRoomAdded( Tomahawk::listeningroom_ptr ) ), Qt::QueuedConnection );

    _detail::Closure* srcRemovedClosure =
        NewClosure( src.data(), SIGNAL( offline() ),
                    this, SLOT( onSourceRemoved( Tomahawk::source_ptr ) ), src );
    srcRemovedClosure->setAutoDelete( false );

    connect( src.data(), SIGNAL( listeningRoomRemoved( Tomahawk::listeningroom_ptr ) ),
             SLOT( onListeningRoomCountChanged() ) );

    tDebug() << "End" << Q_FUNC_INFO;
}

void
ListeningRoomsCategoryItem::onSourceRemoved( const Tomahawk::source_ptr& src )
{
    if ( src->isOnline() )
        return;
    tDebug() << Q_FUNC_INFO;
    tDebug() << "Removed source " << src->friendlyName();
    for ( int i = 0; i < children().count(); )
    {
        ListeningRoomItem* lrItem = qobject_cast< ListeningRoomItem* >( children().at( i ) );
        if( lrItem && lrItem->listeningroom()->author()->id() == src->id() )
        {
            beginRowsRemoved( i, i );
            removeChild( lrItem );
            endRowsRemoved();

            src->removeListeningRoom( lrItem->listeningroom() );
            lrItem->listeningroom()->deleteLater();

            delete lrItem;
        }
        else
            ++i;
    }
}


void
ListeningRoomsCategoryItem::onListeningRoomAdded( const Tomahawk::listeningroom_ptr& p )
{
    if ( p.isNull() )
        return;


    int count = children().count();
    if ( m_showAdd )
        --count; //if an add item exists, it should always appear at the end of the list

    beginRowsAdded( count, count );
    ListeningRoomItem* lrItem = new ListeningRoomItem( model(), this, p ); //inserts child too
    endRowsAdded();

    if ( p->author()->isLocal() )
        connect( p.data(), SIGNAL( aboutToBeDeleted( Tomahawk::listeningroom_ptr ) ),
                 SLOT( onListeningRoomDeleted( Tomahawk::listeningroom_ptr ) ), Qt::QueuedConnection );
    else
        connect( p.data(), SIGNAL( deleted( Tomahawk::listeningroom_ptr ) ),
                 SLOT( onListeningRoomDeleted( Tomahawk::listeningroom_ptr ) ), Qt::QueuedConnection );

    tDebug() << "listening room added to model." << p->title();
    onListeningRoomCountChanged();
}


void
ListeningRoomsCategoryItem::onListeningRoomDeleted( const Tomahawk::listeningroom_ptr& p )
{
    Q_ASSERT( p );

    int count = children().count();
    for ( int i = 0; i < count; ++i )
    {
        ListeningRoomItem* lrItem = qobject_cast< ListeningRoomItem* >( children().at( i ) );
        if( lrItem && lrItem->listeningroom() == p )
        {
            beginRowsRemoved( i, i );
            removeChild( lrItem );
            endRowsRemoved();

            delete lrItem;
            break;
        }
    }
    //onListeningRoomCountChanged() is called afterwards through a signal from Source
}


void
ListeningRoomsCategoryItem::onListeningRoomCountChanged()
{
    if ( SourceList::instance()->getLocal()->hasListeningRooms() ) //if this change affects my own LR
    {
        setAddItemVisible( false );
    }
    else
    {
        setAddItemVisible( true );
    }
}
