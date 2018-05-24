/*
    Sonivox EAS Synthesizer for Qt applications
    Copyright (C) 2016, Pedro Lopez-Cabanillas <plcl@users.sf.net>

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <QCloseEvent>
#include <QFileDialog>
#include "mainwindow.h"
#include "programsettings.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_state(InitialState)
{
    m_synth = new SynthController(ProgramSettings::instance()->bufferTime(), this);

    ui->setupUi(this);
    ui->combo_Reverb->addItem(QStringLiteral("Large Hall"), 0);
    ui->combo_Reverb->addItem(QStringLiteral("Hall"), 1);
    ui->combo_Reverb->addItem(QStringLiteral("Chamber"), 2);
    ui->combo_Reverb->addItem(QStringLiteral("Room"), 3);
    ui->combo_Reverb->addItem(QStringLiteral("None"), -1);
    ui->combo_Reverb->setCurrentIndex(4);

    ui->combo_Chorus->addItem(QStringLiteral("Preset 1"), 0);
    ui->combo_Chorus->addItem(QStringLiteral("Preset 2"), 1);
    ui->combo_Chorus->addItem(QStringLiteral("Preset 3"), 2);
    ui->combo_Chorus->addItem(QStringLiteral("Preset 4"), 3);
    ui->combo_Chorus->addItem(QStringLiteral("None"), -1);
    ui->combo_Chorus->setCurrentIndex(4);

    connect(ui->combo_Reverb, SIGNAL(currentIndexChanged(int)), SLOT(reverbTypeChanged(int)));
    connect(ui->combo_Chorus, SIGNAL(currentIndexChanged(int)), SLOT(chorusTypeChanged(int)));
    connect(ui->dial_Reverb, &QDial::valueChanged, this, &MainWindow::reverbChanged);
    connect(ui->dial_Chorus, &QDial::valueChanged, this, &MainWindow::chorusChanged);
    connect(ui->openButton, &QToolButton::clicked, this, &MainWindow::openFile);
    connect(ui->playButton, &QToolButton::clicked, this, &MainWindow::playSong);
    connect(ui->stopButton, &QToolButton::clicked, this, &MainWindow::stopSong);
    connect(m_synth->renderer(), &SynthRenderer::playbackStopped, this, &MainWindow::songStopped);

    ui->combo_Reverb->setCurrentIndex(1);
    ui->dial_Reverb->setValue(25800);

    m_songFile = QString();
    updateState(EmptyState);
    initialize();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void
MainWindow::initialize()
{
    int bufTime = ProgramSettings::instance()->bufferTime();
    ui->bufTime->setText(QString("%1 ms").arg(bufTime));
    m_synth->start();
}

void
MainWindow::showEvent(QShowEvent* ev)
{
    ev->accept();
}

void
MainWindow::closeEvent(QCloseEvent* ev)
{
    m_synth->stop();
    ProgramSettings::instance()->SaveToNativeStorage();
    ev->accept();
}

void
MainWindow::reverbTypeChanged(int index)
{
    int value = ui->combo_Reverb->itemData(index).toInt();
    m_synth->renderer()->initReverb(value);
    if (value < 0) {
        ui->dial_Reverb->setValue(0);
    }
}

void
MainWindow::reverbChanged(int value)
{
    m_synth->renderer()->setReverbWet(value);
}

void
MainWindow::chorusTypeChanged(int index)
{
    int value = ui->combo_Chorus->itemData(index).toInt();
    m_synth->renderer()->initChorus(value);
    if (value < 0) {
        ui->dial_Chorus->setValue(0);
    }
}

void
MainWindow::chorusChanged(int value)
{
    m_synth->renderer()->setChorusLevel(value);
}

void
MainWindow::readFile(const QString &file)
{
    if (!file.isEmpty() && file != m_songFile) {
        m_songFile = file;
        QFileInfo f(file);
        ui->lblSong->setText(f.fileName());
        updateState(StoppedState);
    }
}

void
MainWindow::openFile()
{
    QString songFile = QFileDialog::getOpenFileName(this,
        tr("Open MIDI file"),  QDir::homePath(),
        tr("MIDI Files (*.mid *.midi *.kar)"));
    if (songFile.isEmpty()) {
        ui->lblSong->setText("[empty]");
        updateState(EmptyState);
    } else {
        readFile(songFile);
    }
}

void
MainWindow::playSong()
{
    if (m_state == StoppedState) {
        m_synth->renderer()->startPlayback(m_songFile);
        updateState(PlayingState);
    }
}

void
MainWindow::stopSong()
{
    if (m_state == PlayingState) {
        m_synth->renderer()->stopPlayback();
        updateState(StoppedState);
    }
}

void
MainWindow::songStopped()
{
    if (m_state != StoppedState) {
        updateState(StoppedState);
    }
}

void
MainWindow::updateState(PlayerState newState)
{
    //qDebug() << Q_FUNC_INFO << newState;
    if (m_state != newState) {
        switch (newState) {
        case EmptyState:
            ui->playButton->setEnabled(false);
            ui->stopButton->setEnabled(false);
            ui->openButton->setEnabled(true);
            break;
        case PlayingState:
            ui->playButton->setEnabled(false);
            ui->stopButton->setEnabled(true);
            ui->openButton->setEnabled(false);
            break;
        case StoppedState:
            ui->stopButton->setEnabled(true);
            ui->playButton->setEnabled(true);
            ui->playButton->setChecked(false);
            ui->openButton->setEnabled(true);
            break;
        default:
            break;
        }
        m_state = newState;
    }
}
