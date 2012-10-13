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

#include <QWidget>

#include "ViewPage.h"

class DLLEXPORT ListeningRoomWidget : public QWidget, public Tomahawk::ViewPage
{
    Q_OBJECT
public:
    explicit ListeningRoomWidget( QWidget* parent = 0 );
    virtual ~ListeningRoomWidget() {}

    QWidget* widget() { return this; }
    Tomahawk::playlistinterface_ptr playlistInterface() const { return Tomahawk::playlistinterface_ptr(); }

    QString title() const { return QString(); }

    QString description() const { return QString(); }

    bool jumpToCurrentTrack() { return true; } //FIXME: what does this do?
    
signals:
    
public slots:
    
};

#endif // LISTENINGROOMWIDGET_H
