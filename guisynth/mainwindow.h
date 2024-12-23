/*
    Sonivox EAS Synthesizer for Qt applications
    Copyright (C) 2016-2024, Pedro Lopez-Cabanillas <plcl@users.sf.net>

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileInfo>
#include "synthcontroller.h"

enum PlayerState {
    InitialState,
    EmptyState,
    PlayingState,
    StoppedState
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void updateState(PlayerState newState);
    void initialize();
    void readSongFile(const QFileInfo &file);
    void readSoundfont(const QFileInfo &file);

protected:
    void showEvent(QShowEvent *ev) override;
    void closeEvent(QCloseEvent *ev) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private slots:
    void alsaConnectionChanged(QString name);
    void reverbTypeChanged(int index);
    void chorusTypeChanged(int index);
    void reverbChanged(int value);
    void chorusChanged(int value);
    void songStopped();

    void openMIDIFile();
    void openDLSFile();
    void playSong();
    void stopSong();

private:
    Ui::MainWindow *ui;
    SynthController *m_synth;
    QString m_songFile;
    QString m_dlsFile;
    PlayerState m_state;
    QString m_subscription;
};

#endif // MAINWINDOW_H
