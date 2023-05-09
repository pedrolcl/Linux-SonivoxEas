/*
    Sonivox EAS Synthesizer for Qt applications
    Copyright (C) 2016-2023, Pedro Lopez-Cabanillas <plcl@users.sf.net>

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

#include <signal.h>
#include <QDebug>
#include <QCoreApplication>
#include <QCommandLineParser>
#include "eas_reverb.h"
#include "synthcontroller.h"
#include "programsettings.h"

#if QT_VERSION >= QT_VERSION_CHECK(5,15,0)
    #define endl Qt::endl
#endif

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
    QCoreApplication::setApplicationVersion(QT_STRINGIFY(VERSION));
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    QCommandLineParser parser;
    parser.setApplicationDescription("Command Line MIDI Synthesizer and Player");
    parser.addVersionOption();
    parser.addHelpOption();
    QCommandLineOption bufferOption(QStringList() << "b" << "buffer","Audio buffer time in milliseconds.", "buffer_time", "60");
    QCommandLineOption reverbOption(QStringList() << "r" << "reverb", "Reverb type (none=-1,presets=0,1,2,3).", "reverb_type", "1");
    QCommandLineOption wetOption(QStringList() << "w" << "wet", "Reverb wet (0..32765).", "reverb_wet", "25800");
    QCommandLineOption chorusOption(QStringList() << "c" << "chorus", "Chorus type (none=-1,presets=0,1,2,3).", "chorus_type", "-1");
    QCommandLineOption levelOption(QStringList() << "l" << "level", "Chorus level (0..32765).", "chorus_level", "0");
    parser.addOption(bufferOption);
    parser.addOption(reverbOption);
    parser.addOption(chorusOption);
    parser.addOption(wetOption);
    parser.addOption(levelOption);
    parser.addPositionalArgument("files", "MIDI Files (.mid;.kar)", "[files ...]");
    parser.process(app);
    ProgramSettings::instance()->ReadFromNativeStorage();
    if (parser.isSet(bufferOption)) {
        int n = parser.value(bufferOption).toInt();
        if (n > 0)
            ProgramSettings::instance()->setBufferTime(n);
        else {
            fputs("Wrong buffer time.\n", stderr);
            parser.showHelp(1);
        }
    }
    if (parser.isSet(wetOption)) {
        int n = parser.value(wetOption).toInt();
        if (n >= 0 && n <= 32765)
            ProgramSettings::instance()->setReverbWet(n);
        else {
            fputs("Wrong reverb wet value.\n", stderr);
            parser.showHelp(1);
        }
    }
    if (parser.isSet(reverbOption)) {
        int n = parser.value(reverbOption).toInt();
        if (n >= -1 && n <= 3)
            ProgramSettings::instance()->setReverbType(n);
        else {
            fputs("Wrong reverb type.\n", stderr);
            parser.showHelp(1);
        }
    }
    if (parser.isSet(levelOption)) {
        int n = parser.value(levelOption).toInt();
        if (n >= 0 && n <= 32765)
            ProgramSettings::instance()->setChorusLevel(n);
        else {
            fputs("Wrong chorus level.\n", stderr);
            parser.showHelp(1);
        }
    }
    if (parser.isSet(chorusOption)) {
        int n = parser.value(chorusOption).toInt();
        if (n >= -1 && n <= 3)
            ProgramSettings::instance()->setChorusType(n);
        else {
            fputs("Wrong chorus type.\n", stderr);
            parser.showHelp(1);
        }
    }
    synth = new SynthController(ProgramSettings::instance()->bufferTime());
    synth->renderer()->setReverbWet(ProgramSettings::instance()->reverbWet());
    synth->renderer()->initReverb(ProgramSettings::instance()->reverbType());
    synth->renderer()->setChorusLevel(ProgramSettings::instance()->chorusLevel());
    synth->renderer()->initChorus(ProgramSettings::instance()->chorusType());
    QObject::connect(&app, &QCoreApplication::aboutToQuit, synth, &QObject::deleteLater);
    QObject::connect(&app, &QCoreApplication::aboutToQuit, ProgramSettings::instance(), &ProgramSettings::SaveToNativeStorage);
    QObject::connect(synth->renderer(), &SynthRenderer::playbackStopped, &app, &QCoreApplication::quit);
    QObject::connect(synth->renderer(), &SynthRenderer::finished, &app, &QCoreApplication::quit);
    QStringList args = parser.positionalArguments();
    if (!args.isEmpty()) {
        for(int i = 0; i < args.length();  ++i) {
            QFile argFile(args[i]);
            if (argFile.exists()) {
                synth->renderer()->playFile(argFile.fileName());
            }
        }
    }
    synth->start();
    return app.exec();
}
