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

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_synth(new SynthController)
{
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

    ui->combo_Reverb->setCurrentIndex(1);
    ui->dial_Reverb->setValue(25800);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void
MainWindow::showEvent(QShowEvent*)
{
    m_synth->start();
}

void
MainWindow::closeEvent(QCloseEvent*)
{
    m_synth->stop();
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
