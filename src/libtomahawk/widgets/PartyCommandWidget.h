/*
 *  Copyright 2012, 2013 Teo Mrnjavac <teo@kde.org>
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

#ifndef PARTYCOMMANDWIDGET_H
#define PARTYCOMMANDWIDGET_H

#include "Typedefs.h"

#include <QMap>
#include <QPixmap>
#include <QWidget>

class QBoxLayout;
class QLabel;
class QPushButton;

namespace Tomahawk
{

class PartyCommandWidget : public QWidget
{
    Q_OBJECT
public:
    enum ButtonState
    {
        Join,
        Leave,
        Disband
    };

    explicit PartyCommandWidget(QWidget *parent = 0);
    virtual ~PartyCommandWidget() {}

public slots:
    void setListeners( const QStringList& listenerDbids );
    void setButtonState( ButtonState state );

signals:
    void joinLeaveButtonClicked( PartyCommandWidget::ButtonState );

private  slots:
    void onJoinLeaveButtonClicked();

private:
    void fillListeners();

    ButtonState m_buttonState;
    QMap< ButtonState, QIcon >   m_buttonIcons;
    QMap< ButtonState, QString > m_buttonStrings;

    QList< Tomahawk::source_ptr > m_listeners;
    QPixmap m_defaultAvatar;

    QWidget* m_listenersWidget;
    QMap< QString, QLabel* > m_avatarLabels;
    int m_unnamedListeners;
    QBoxLayout* m_avatarsLayout;
    QLabel* m_unnamedListenersLabel;

    QPushButton* m_joinLeaveButton;
};

}

#endif // PARTYCOMMANDWIDGET_H
