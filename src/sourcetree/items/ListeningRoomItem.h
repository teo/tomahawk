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
                                const QString& text,
                                const QIcon& icon );

    virtual Qt::ItemFlags flags() const;
    virtual QString text() const;
    virtual QIcon icon() const;
    virtual int peerSortValue() const;

    virtual bool willAcceptDrag(const QMimeData* data) const;
    virtual DropTypes supportedDropTypes(const QMimeData* data) const;
    virtual bool dropMimeData(const QMimeData* data, Qt::DropAction action);

private:
    
};

#endif // LISTENINGROOMITEM_H
