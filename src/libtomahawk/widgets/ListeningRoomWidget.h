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

#ifndef LISTENINGROOMWIDGET_H
#define LISTENINGROOMWIDGET_H

#include <QtGui/QWidget>

#include "libtomahawk/Typedefs.h"
#include "libtomahawk/ListeningRoom.h"
#include "ViewPage.h"

class ListeningRoomHeader;

class DLLEXPORT ListeningRoomWidget : public QWidget, public Tomahawk::ViewPage
{
    Q_OBJECT
public:
    explicit ListeningRoomWidget( const Tomahawk::listeningroom_ptr& listeningRoom,
                                  QWidget* parent = 0 );
    virtual ~ListeningRoomWidget() {}

    void load( const Tomahawk::listeningroom_ptr& listeningRoom );

    QWidget* widget() { return this; }
    Tomahawk::playlistinterface_ptr playlistInterface() const { return Tomahawk::playlistinterface_ptr(); }

    QString title() const { return m_listeningRoom->title(); }

    QString description() const;

    bool isTemporaryPage() const { return false; }
    bool showInfoBar() const { return false; } //we take care of our own header

    bool jumpToCurrentTrack() { return true; } //FIXME: what does this do?
    
signals:
    
public slots:

private:
    Tomahawk::listeningroom_ptr m_listeningRoom;

    ListeningRoomHeader *m_header;
    QWidget* m_body;
};

#endif // LISTENINGROOMWIDGET_H
