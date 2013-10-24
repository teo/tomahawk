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

#include "PartiesCategoryItem.h"

#include "Party.h"
#include "PartyItem.h"
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


PartiesCategoryItem::PartiesCategoryItem( SourcesModel* model,
                                                        SourceTreeItem* parent,
                                                        int peerSortValue )
    : GroupCategoryItem( model, parent, SourcesModel::PartiesCategory, true, peerSortValue )
{
//useless because it's all empty at startup
    foreach ( const Tomahawk::source_ptr& src, SourceList::instance()->sources( true ) )
    {
        connect( src.data(), SIGNAL( partyAdded( Tomahawk::party_ptr ) ),
                 SLOT( onPartyAdded( Tomahawk::party_ptr ) ), Qt::QueuedConnection );
    }

    connect( SourceList::instance(), SIGNAL( sourceAdded( Tomahawk::source_ptr ) ),
             SLOT( onSourceAdded( Tomahawk::source_ptr ) ) );
}


void
PartiesCategoryItem::onSourceAdded( const Tomahawk::source_ptr& src )
{
    tDebug() << "Begin" << Q_FUNC_INFO;

    connect( src.data(), SIGNAL( partyAdded( Tomahawk::party_ptr ) ),
             SLOT( onPartyAdded( Tomahawk::party_ptr ) ), Qt::QueuedConnection );

    _detail::Closure* srcRemovedClosure =
        NewClosure( src.data(), SIGNAL( offline() ),
                    this, SLOT( onSourceRemoved( Tomahawk::source_ptr ) ), src );
    srcRemovedClosure->setAutoDelete( false );

    connect( src.data(), SIGNAL( partyRemoved( Tomahawk::party_ptr ) ),
             SLOT( onPartyCountChanged() ) );

    tDebug() << "End" << Q_FUNC_INFO;
}

void
PartiesCategoryItem::onSourceRemoved( const Tomahawk::source_ptr& src )
{
    if ( src->isOnline() )
        return;
    tDebug() << Q_FUNC_INFO;
    tDebug() << "Removed source " << src->friendlyName();
    for ( int i = 0; i < children().count(); )
    {
        PartyItem* lrItem = qobject_cast< PartyItem* >( children().at( i ) );
        if( lrItem && lrItem->party()->author()->id() == src->id() )
        {
            beginRowsRemoved( i, i );
            removeChild( lrItem );
            endRowsRemoved();

            src->removeParty();
        }
        else
            ++i;
    }
}


void
PartiesCategoryItem::onPartyAdded( const Tomahawk::party_ptr& p )
{
    if ( p.isNull() )
        return;


    int count = children().count();
    if ( m_showAdd )
        --count; //if an add item exists, it should always appear at the end of the list

    beginRowsAdded( count, count );
    PartyItem* lrItem = new PartyItem( model(), this, p ); //inserts child too
    endRowsAdded();

    if ( p->author()->isLocal() )
        connect( p.data(), SIGNAL( aboutToBeDeleted( Tomahawk::party_ptr ) ),
                 SLOT( onPartyDeleted( Tomahawk::party_ptr ) ), Qt::QueuedConnection );
    else
        connect( p.data(), SIGNAL( deleted( Tomahawk::party_ptr ) ),
                 SLOT( onPartyDeleted( Tomahawk::party_ptr ) ), Qt::QueuedConnection );

    tDebug() << "party added to model." << p->playlist()->title();
    onPartyCountChanged();
}


void
PartiesCategoryItem::onPartyDeleted( const Tomahawk::party_ptr& p )
{
    Q_ASSERT( p );

    int count = children().count();
    for ( int i = 0; i < count; ++i )
    {
        PartyItem* lrItem = qobject_cast< PartyItem* >( children().at( i ) );
        if( lrItem && lrItem->party() == p )
        {
            beginRowsRemoved( i, i );
            removeChild( lrItem );
            endRowsRemoved();

            delete lrItem;
            break;
        }
    }
    //onPartyCountChanged() is called afterwards through a signal from Source
}


void
PartiesCategoryItem::onPartyCountChanged()
{
    if ( SourceList::instance()->getLocal()->hasParty() ) //if this change affects my own LR
    {
        setAddItemVisible( false );
    }
    else
    {
        setAddItemVisible( true );
    }
}
