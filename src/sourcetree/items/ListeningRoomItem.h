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

#ifndef LISTENINGROOMITEM_H
#define LISTENINGROOMITEM_H

#include "SourceTreeItem.h"

class ListeningRoomItem : public SourceTreeItem
{
    Q_OBJECT
public:
    explicit ListeningRoomItem( SourcesModel* model,
                                SourceTreeItem* parent,
                                const Tomahawk::listeningroom_ptr& lr,
                                int index = -1 );

    virtual QString text() const;
    virtual QString editorText() const;
    virtual Tomahawk::listeningroom_ptr listeningroom() const;
    virtual Qt::ItemFlags flags() const;
//    virtual bool willAcceptDrag( const QMimeData* data ) const;
//    virtual DropTypes supportedDropTypes( const QMimeData* data ) const;
//    virtual bool dropMimeData( const QMimeData* data, Qt::DropAction action );
    virtual QIcon icon() const;
    virtual bool setData(const QVariant& v, bool role);
    virtual int peerSortValue() const;
    virtual int IDValue() const;
//    virtual bool isBeingPlayed() const;

    virtual SourceTreeItem* activateCurrent();

public slots:
    virtual void activate();

private slots:
    void onUpdated();

private:
    QIcon m_icon;
    Tomahawk::listeningroom_ptr m_listeningroom;
};

#endif // LISTENINGROOMITEM_H
