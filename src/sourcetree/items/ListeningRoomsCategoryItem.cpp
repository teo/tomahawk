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

    //Why do I have to do this here? Shouldn't Q_DECLARE_METATYPE in ListeningRoom.h be enough?
    qRegisterMetaType< Tomahawk::listeningroom_ptr >( "Tomahawk::listeningroom_ptr" );

    connect( src.data(), SIGNAL( listeningRoomAdded( Tomahawk::listeningroom_ptr ) ),
             SLOT( onListeningRoomAdded( Tomahawk::listeningroom_ptr ) ), Qt::QueuedConnection );
    tDebug() << "End" << Q_FUNC_INFO;
}


void
ListeningRoomsCategoryItem::onListeningRoomAdded( const Tomahawk::listeningroom_ptr& p )
{
    if ( p.isNull() )
        return;

//    beginRowsAdded( children().count() - 1, children().count() );
    ListeningRoomItem* lrItem = new ListeningRoomItem( model(), this, p );
    insertItem( lrItem );

    if ( p->author()->isLocal() )
        connect( p.data(), SIGNAL( aboutToBeDeleted( Tomahawk::listeningroom_ptr ) ),
                 SLOT( onListeningRoomDeleted( Tomahawk::listeningroom_ptr ) ), Qt::QueuedConnection );
    else
        connect( p.data(), SIGNAL( deleted( Tomahawk::listeningroom_ptr ) ),
                 SLOT( onListeningRoomDeleted( Tomahawk::listeningroom_ptr ) ), Qt::QueuedConnection );

//    endRowsAdded();

    tDebug() << "listening room added to model.";
}


void
ListeningRoomsCategoryItem::onListeningRoomDeleted( const Tomahawk::listeningroom_ptr& p )
{
    //TODO: implement
}
