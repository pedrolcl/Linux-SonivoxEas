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

#include <QCloseEvent>
#include <QDebug>
#include <QFileDialog>
#include <QMimeData>

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

    ui->libSonivoxVersion->setText(m_synth->renderer()->libVersion());
    setWindowTitle(windowTitle() + " v" + qApp->applicationVersion());
    ui->qtVersion->setText(qVersion());
    ui->drumstickVersion->setText(drumstick::ALSA::getDrumstickLibraryVersion());

    connect(ui->combo_ALSAConn,
            &QComboBox::currentTextChanged,
            this,
            &MainWindow::alsaConnectionChanged);
    connect(ui->combo_Reverb, SIGNAL(currentIndexChanged(int)), this, SLOT(reverbTypeChanged(int)));
    connect(ui->combo_Chorus, SIGNAL(currentIndexChanged(int)), this, SLOT(chorusTypeChanged(int)));
    connect(ui->dial_Reverb, &QDial::valueChanged, this, &MainWindow::reverbChanged);
    connect(ui->dial_Chorus, &QDial::valueChanged, this, &MainWindow::chorusChanged);
    connect(ui->openMIDIbtn, &QToolButton::clicked, this, &MainWindow::openMIDIFile);
    connect(ui->openDLSbtn, &QToolButton::clicked, this, &MainWindow::openDLSFile);
    connect(ui->playButton, &QToolButton::clicked, this, &MainWindow::playSong);
    connect(ui->stopButton, &QToolButton::clicked, this, &MainWindow::stopSong);
    connect(m_synth->renderer(), &SynthRenderer::playbackStopped, this, &MainWindow::songStopped);

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
    QFileInfo dlsInfo(ProgramSettings::instance()->dlsSoundfont());
    if (dlsInfo.exists() && dlsInfo.isReadable()) {
        readSoundfont(dlsInfo);
    } else {
        ui->lblSFFileName->setText("[empty]");
        m_dlsFile.clear();
        ProgramSettings::instance()->setDLSsoundfont(m_dlsFile);
        m_synth->renderer()->initSoundfont(m_dlsFile);
    }
    int reverb = ui->combo_Reverb->findData(ProgramSettings::instance()->reverbType());
    ui->combo_Reverb->setCurrentIndex(reverb);
    ui->dial_Reverb->setValue(ProgramSettings::instance()->reverbWet()); //0..32767
    int chorus = ui->combo_Chorus->findData(ProgramSettings::instance()->chorusType());
    ui->combo_Chorus->setCurrentIndex(chorus);
    ui->dial_Chorus->setValue(ProgramSettings::instance()->chorusLevel());
    m_synth->start();

    ui->combo_ALSAConn->blockSignals(true);
    ui->combo_ALSAConn->clear();
    ui->combo_ALSAConn->addItems(m_synth->renderer()->alsaConnections());
    ui->combo_ALSAConn->blockSignals(false);
    ui->combo_ALSAConn->setCurrentText(ProgramSettings::instance()->ALSAConnection());
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

void MainWindow::alsaConnectionChanged(QString name)
{
    qDebug() << Q_FUNC_INFO << name;
    if (!m_subscription.isEmpty()) {
        m_synth->renderer()->unsubscribe(m_subscription);
    }
    m_synth->renderer()->subscribe(name);
    ProgramSettings::instance()->setALSAConnection(name);
    m_subscription = name;
}

void
MainWindow::reverbTypeChanged(int index)
{
    int value = ui->combo_Reverb->itemData(index).toInt();
    m_synth->renderer()->initReverb(value);
    ProgramSettings::instance()->setReverbType(value);
    if (value < 0) {
        ui->dial_Reverb->setValue(0);
        ProgramSettings::instance()->setReverbWet(0);
    } else {
        int wet = m_synth->renderer()->reverbWet();
        ui->dial_Reverb->setValue(wet);
        ProgramSettings::instance()->setReverbWet(wet);
    }
}

void
MainWindow::reverbChanged(int value)
{
    ui->reverbWet->setNum(value);
    m_synth->renderer()->setReverbWet(value);
    ProgramSettings::instance()->setReverbWet(value);
}

void
MainWindow::chorusTypeChanged(int index)
{
    int value = ui->combo_Chorus->itemData(index).toInt();
    m_synth->renderer()->initChorus(value);
    ProgramSettings::instance()->setChorusType(value);
    if (value < 0) {
        ui->dial_Chorus->setValue(0);
        ProgramSettings::instance()->setChorusLevel(0);
    } else {
        int level = m_synth->renderer()->chorusLevel();
        ui->dial_Chorus->setValue(level);
        ProgramSettings::instance()->setChorusLevel(level);
    }
}

void
MainWindow::chorusChanged(int value)
{
    ui->chorusLevel->setNum(value);
    m_synth->renderer()->setChorusLevel(value);
    ProgramSettings::instance()->setChorusLevel(value);
}

void
MainWindow::readSongFile(const QFileInfo &file)
{
    if (file.exists() && file.isReadable()) {
        m_songFile = file.absoluteFilePath();
        ui->lblSong->setText(file.fileName());
        updateState(StoppedState);
    }
}

void MainWindow::readSoundfont(const QFileInfo &file)
{
    if (file.exists() && file.isReadable()) {
        m_dlsFile = file.absoluteFilePath();
        ui->lblSFFileName->setText(file.fileName());
        m_synth->renderer()->initSoundfont(m_dlsFile);
        ProgramSettings::instance()->setDLSsoundfont(m_dlsFile);
    }
}

void
MainWindow::openMIDIFile()
{
    QString fileName
        = QFileDialog::getOpenFileName(this,
                                       tr("Open MIDI file"),
                                       QString(),
                                       tr("MIDI Files (*.mid *.midi *.kar *.rmi *.xmf *.mxmf)"));
    if (fileName.isEmpty()) {
        ui->lblSong->setText("[empty]");
        updateState(EmptyState);
    } else {
        QFileInfo fInfo(fileName);
        readSongFile(fInfo);
    }
}

void
MainWindow::openDLSFile()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open SoundFont file"),
                                                    QString(),
                                                    tr("SoundFont Files (*.dls *.sf2)"));
    if (fileName.isEmpty()) {
        m_dlsFile.clear();
        ui->lblSFFileName->setText("[empty]");
        ProgramSettings::instance()->setDLSsoundfont(m_dlsFile);
        m_synth->renderer()->initSoundfont(m_dlsFile);
    } else {
        QFileInfo fInfo(fileName);
        readSoundfont(fInfo);
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
            ui->openMIDIbtn->setEnabled(true);
            ui->openDLSbtn->setEnabled(true);
            break;
        case PlayingState:
            ui->playButton->setEnabled(false);
            ui->stopButton->setEnabled(true);
            ui->openMIDIbtn->setEnabled(false);
            ui->openDLSbtn->setEnabled(false);
            break;
        case StoppedState:
            ui->stopButton->setEnabled(true);
            ui->playButton->setEnabled(true);
            ui->playButton->setChecked(false);
            ui->openMIDIbtn->setEnabled(true);
            ui->openDLSbtn->setEnabled(true);
            break;
        default:
            break;
        }
        m_state = newState;
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        QString path;
        const QList<QUrl> urlList = mimeData->urls();
        if (urlList.count() > 0) {
            path = urlList.first().toLocalFile();
        }

        if (!path.isEmpty()) {
            QFileInfo fInfo(path);
            if (fInfo.exists()) {
                const auto ext = fInfo.suffix().toLower();
                if (ext == "mid" || ext == "midi" || ext == "kar" || ext == "rmi" || ext == "xmf"
                    || ext == "mxmf") {
                    readSongFile(fInfo);
                } else if (ext == "dls" || ext == "sf2") {
                    readSoundfont(fInfo);
                }
            }
        }
    }
}
