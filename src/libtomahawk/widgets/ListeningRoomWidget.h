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
#include "ListeningRoomHeader.h"
#include "ViewPage.h"

#include <QtGui/QIcon>

class ListeningRoomHeader;
class ListeningRoomModel;
class ListeningRoomCurrentTrackWidget;
class TrackView;
class QTimeLine;
class QPushButton;
class QModelIndex;

class DLLEXPORT ListeningRoomWidget : public QWidget, public Tomahawk::ViewPage
{
    Q_OBJECT
public:
    explicit ListeningRoomWidget( QWidget* parent = 0 );
    virtual ~ListeningRoomWidget() {}

    QWidget* widget() { return this; }
    Tomahawk::playlistinterface_ptr playlistInterface() const;

    QString title() const;
    QString description() const;
    QPixmap pixmap() const;

    bool isTemporaryPage() const { return false; }
    bool showInfoBar() const { return false; } //we take care of our own header

    bool jumpToCurrentTrack() { return true; } //FIXME: what does this do?

    /**
     * @brief setModel sets the model for this ListeningRoomWidget
     * @param model a pointer to the model
     * ListeningRoomModel wraps around a ListeningRoom object, for easy use with ListeningRoomWidget
     * and its model-view-ish contents. Ideally, LRW should access LR data through LRM rather than
     * getting a pointer to LR and talking to it directly.
     */
    void setModel( ListeningRoomModel* model );
    
public slots:

private slots:
    void toggleHistoryDrawer();
    void onAnimationStep( int );
    void onAnimationFinished();
    void onListenersChanged();
    void onJoinLeaveButtonClicked( ListeningRoomHeader::ButtonState );

    void onDataChanged( const QModelIndex&, const QModelIndex& );

private:
    ListeningRoomHeader *m_header;
    QWidget* m_historyDrawer;
    QPushButton* m_previousTracksButton;
    TrackView* m_historyView;
    QWidget* m_body;
    ListeningRoomCurrentTrackWidget* m_currentTrackWidget;

    TrackView* m_view;
    ListeningRoomModel* m_model;

    QPixmap m_pixmap;

    //history drawer animation
    QTimeLine* m_timeline;
    int m_drawerH;
    bool m_drawerShown;
    QIcon m_downArrow;
    QIcon m_upArrow;
    QString m_showTracksString;
    QString m_hideTracksString;

    int m_currentRow;
};

#endif // LISTENINGROOMWIDGET_H
