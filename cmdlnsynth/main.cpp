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

#include <signal.h>
#include <QDebug>
#include <QCoreApplication>
#include <QCommandLineParser>
#include <eas_reverb.h>
#include "synthcontroller.h"
#include "programsettings.h"

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
static SynthController* synth = 0;

void signalHandler(int sig)
{
    if (sig == SIGINT)
        qDebug() << "SIGINT received. Exiting" << endl;
    else if (sig == SIGTERM)
        qDebug() << "SIGTERM received. Exiting" << endl;
    if (synth != 0)
        synth->stop();
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setOrganizationName("LinuxEASSynth");
    QCoreApplication::setApplicationName("cmdlnsynth");
    QCoreApplication::setApplicationVersion(TOSTRING(VERSION));
    QCommandLineParser parser;
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    parser.setApplicationDescription("Command Line MIDI Synthesizer and Player");
    parser.addVersionOption();
    parser.addHelpOption();
    QCommandLineOption bufferOption(QStringList() << "b" << "buffer","Audio buffer time in milliseconds", "bufer_time", "60");
    parser.addOption(bufferOption);
    parser.addPositionalArgument("files", "MIDI Files (*.mid; *.kar)", "[files ...]");
    parser.process(app);
    ProgramSettings::instance()->ReadFromNativeStorage();
    if (parser.isSet(bufferOption)) {
        qDebug() << "buffertime:" << parser.value(bufferOption);
        ProgramSettings::instance()->setBufferTime(parser.value(bufferOption).toInt());
        ProgramSettings::instance()->SaveToNativeStorage();
    }
    synth = new SynthController(ProgramSettings::instance()->bufferTime());
    QObject::connect(&app, &QCoreApplication::aboutToQuit, synth, &QObject::deleteLater);
    QObject::connect(synth->renderer(), &SynthRenderer::playbackStopped, &app, &QCoreApplication::quit);
    QObject::connect(synth->renderer(), &SynthRenderer::finished, &app, &QCoreApplication::quit);
    synth->renderer()->initReverb(EAS_PARAM_REVERB_HALL);
    QStringList args = parser.positionalArguments();
    if (!args.isEmpty()) {
        for(int i = 0; i < args.length();  ++i) {
            qDebug() << args[i];
            QFile argFile(args[i]);
            if (argFile.exists()) {
                synth->renderer()->playFile(argFile.fileName());
            }
        }
    }
    synth->start();
    return app.exec();
}
