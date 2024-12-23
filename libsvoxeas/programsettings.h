﻿/*
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

#ifndef PROGRAMSETTINGS_H
#define PROGRAMSETTINGS_H

#include <QObject>
#include <QString>
#include <QSettings>

class ProgramSettings : public QObject
{
    Q_OBJECT

public:
    static ProgramSettings* instance();

    int bufferTime() const;
    void setBufferTime(int bufferTime);

    int reverbType() const;
    void setReverbType(int reverbType);

    int reverbWet() const;
    void setReverbWet(int reverbWet);

    int chorusType() const;
    void setChorusType(int chorusType);

    int chorusLevel() const;
    void setChorusLevel(int chorusLevel);

    QString dlsSoundfont() const;
    void setDLSsoundfont(const QString &newDLSsoundfont);

    QString ALSAConnection() const;
    void setALSAConnection(const QString &newALSAConnection);

signals:
    void ValuesChanged();

public slots:
    void ResetDefaults();
    void ReadFromNativeStorage();
    void ReadFromFile(const QString &filepath);
    void SaveToNativeStorage();
    void SaveToFile(const QString &filepath);

private:
    explicit ProgramSettings(QObject *parent = nullptr);
    void internalRead(QSettings& settings);
    void internalSave(QSettings& settings);

    int m_bufferTime;
    int m_reverbType;
    int m_reverbWet;
    int m_chorusType;
    int m_chorusLevel;
    QString m_DLSsoundfont;
    QString m_ALSAConnection;
};

#endif // PROGRAMSETTINGS_H
