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

#include "ListeningRoom.h"
#include "ViewManager.h"
#include "ViewPage.h"


ListeningRoomItem::ListeningRoomItem( SourcesModel* model,
                                      SourceTreeItem* parent,
                                      const Tomahawk::listeningroom_ptr& lr,
                                      int index )
    : SourceTreeItem( model, parent, SourcesModel::ListeningRoom, index )
    , m_listeningroom( lr )
{
    tDebug() << Q_FUNC_INFO << "le wild Listening Room item appears.";
    Q_ASSERT( lr );

    m_icon = QIcon( RESPATH "images/playlist-icon.png" );

    connect( lr.data(), SIGNAL( changed() ), SLOT( onUpdated() ), Qt::QueuedConnection );
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
