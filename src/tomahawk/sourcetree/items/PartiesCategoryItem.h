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

#ifndef PARTIESCATEGORYITEM_H
#define PARTIESCATEGORYITEM_H

#include "CategoryItems.h"
#include "SourceTreeItem.h"

/**
 * @brief The GroupCategoryItem class creates something like a GroupItem, i.e.
 * a source view item that's usually top-level and acts as a grouping item to
 * show/hide its contents; which also happens to handle its contents dynamically
 * like a CategoryItem.
 */
class GroupCategoryItem : public CategoryItem
{
    Q_OBJECT
public:
    GroupCategoryItem( SourcesModel* model,
                       SourceTreeItem* parent,
                       SourcesModel::CategoryType category,
                       bool showAddItem,
                       int peerSortValue = 0 )
        : CategoryItem( model, parent, category, showAddItem, peerSortValue )
        , m_defaultExpanded( true )
    {
        setRowType( SourcesModel::Group );
    }

    virtual bool willAcceptDrag( const QMimeData* ) const { return false; }
    virtual QIcon icon() const { return QIcon(); }
    virtual bool isBeingPlayed() const { return false; }
    virtual int peerSortValue() const { return SourceTreeItem::peerSortValue(); }

    void checkExpandedState();
    void setDefaultExpanded( bool b ) { m_defaultExpanded = b; }

public slots:
    virtual void activate() { emit toggleExpandRequest( this ); }

signals:
    void activated();

private slots:
    void requestExpanding() { emit expandRequest( this ); }

private:
    bool m_defaultExpanded;
};


class PartiesCategoryItem : public GroupCategoryItem
{
    Q_OBJECT
public:
    PartiesCategoryItem( SourcesModel* model,
                                SourceTreeItem* parent,
                                int peerSortValue = 0 );

public slots:
    void onSourceRemoved( const Tomahawk::source_ptr& src );

private slots:
    void onSourceAdded( const Tomahawk::source_ptr& src );
    void onPartyAdded( const Tomahawk::party_ptr& p );
    void onPartyDeleted( const Tomahawk::party_ptr& p );
    void onPartyCountChanged();
};



#endif // PARTIESCATEGORYITEM_H
