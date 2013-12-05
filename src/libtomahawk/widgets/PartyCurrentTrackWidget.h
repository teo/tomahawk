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

#ifndef PARTYCURRENTTRACKWIDGET_H
#define PARTYCURRENTTRACKWIDGET_H

#include <QWidget>


class QLabel;
class ElidedLabel;
class QPersistentModelIndex;

namespace Tomahawk
{

class PartyCurrentTrackWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PartyCurrentTrackWidget( QWidget* parent = 0 );

    void setItem( const QPersistentModelIndex& idx );

private:
    QLabel* m_albumArtLabel;
    ElidedLabel* m_trackLabel;
    ElidedLabel* m_artistLabel;
    QLabel* m_lovesLabel;
    QLabel* m_durationLabel;
    QLabel* m_avatarLabel;

    QPixmap m_albumArtPlaceholder;
};

}

#endif // PARTYCURRENTTRACKWIDGET_H
