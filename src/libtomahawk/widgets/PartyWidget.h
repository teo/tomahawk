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

#ifndef PARTYWIDGET_H
#define PARTYWIDGET_H

#include <QtGui/QWidget>

#include "Typedefs.h"
#include "Party.h"
#include "PartyHeader.h"
#include "ViewPage.h"

#include <QtGui/QIcon>

class QTimeLine;
class QPushButton;
class QModelIndex;
class TrackView;

namespace Tomahawk
{

class PartyCurrentTrackWidget;

class DLLEXPORT PartyWidget : public QWidget, public Tomahawk::ViewPage
{
    Q_OBJECT
public:
    explicit PartyWidget( QWidget* parent = 0 );
    virtual ~PartyWidget() {}

    QWidget* widget() { return this; }
    Tomahawk::playlistinterface_ptr playlistInterface() const;

    QString title() const;
    QString description() const;
    QPixmap pixmap() const;

    bool isTemporaryPage() const { return false; }
    bool showInfoBar() const { return false; } //we take care of our own header

    bool jumpToCurrentTrack() { return true; } //FIXME: what does this do?

    void setParty( const party_ptr& party );
    
protected:
    void resizeEvent( QResizeEvent* e );

private slots:
    void toggleHistoryDrawer();
    void onAnimationStep( int );
    void onAnimationFinished();
    void onListenersChanged();
    void onJoinLeaveButtonClicked( PartyHeader::ButtonState );

    void onDataChanged( const QModelIndex&, const QModelIndex& );

    void onHistoryItemActivated( const QModelIndex& idx );
    void onMainViewItemActivated( const QModelIndex& idx );

private:
    PartyHeader *m_header;              //FIXME: remove
    QWidget* m_historyDrawer;
    QPushButton* m_previousTracksButton;
    TrackView* m_historyView;
    QWidget* m_body;
    PartyCurrentTrackWidget* m_currentTrackWidget;

    TrackView* m_view;
    Tomahawk::party_ptr m_party;

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

}

#endif // PARTYWIDGET_H
