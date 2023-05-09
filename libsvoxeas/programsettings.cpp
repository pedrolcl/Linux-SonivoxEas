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

#include "programsettings.h"

ProgramSettings::ProgramSettings(QObject *parent) : QObject(parent)
{
    ResetDefaults();
}

ProgramSettings* ProgramSettings::instance()
{
    static ProgramSettings inst;
    return &inst;
}

void ProgramSettings::ResetDefaults()
{
    m_bufferTime = 60;
    m_reverbType = 1;
    m_reverbWet = 25800;
    m_chorusType = -1;
    m_chorusLevel = 0;
    emit ValuesChanged();
}

void ProgramSettings::ReadFromNativeStorage()
{
    QSettings::setDefaultFormat(QSettings::NativeFormat);
    QSettings settings;
    internalRead(settings);
}

void ProgramSettings::SaveToNativeStorage()
{
    QSettings::setDefaultFormat(QSettings::NativeFormat);
    QSettings settings;
    internalSave(settings);
}

void ProgramSettings::ReadFromFile(const QString& filepath)
{
    QSettings settings(filepath, QSettings::IniFormat);
    internalRead(settings);
}

void ProgramSettings::SaveToFile(const QString& filepath)
{
    QSettings settings(filepath, QSettings::IniFormat);
    internalSave(settings);
}

void ProgramSettings::internalRead(QSettings &settings)
{
    m_bufferTime = settings.value("BufferTime", 60).toInt();
    m_reverbType = settings.value("ReverbType", 1).toInt();
    m_reverbWet = settings.value("ReverbWet", 25800).toInt();
    m_chorusType = settings.value("ChorusType", -1).toInt();
    m_chorusLevel = settings.value("ChorusLevel", 0).toInt();
    emit ValuesChanged();
}

void ProgramSettings::internalSave(QSettings &settings)
{
    settings.setValue("BufferTime", m_bufferTime);
    settings.setValue("ReverbType", m_reverbType);
    settings.setValue("ReverbWet", m_reverbWet);
    settings.setValue("ChorusType", m_chorusType);
    settings.setValue("ChorusLevel", m_chorusLevel);
    settings.sync();
}

int ProgramSettings::chorusLevel() const
{
    return m_chorusLevel;
}

void ProgramSettings::setChorusLevel(int chorusLevel)
{
    m_chorusLevel = chorusLevel;
}

int ProgramSettings::chorusType() const
{
    return m_chorusType;
}

void ProgramSettings::setChorusType(int chorusType)
{
    m_chorusType = chorusType;
}

int ProgramSettings::reverbWet() const
{
    return m_reverbWet;
}

void ProgramSettings::setReverbWet(int reverbWet)
{
    m_reverbWet = reverbWet;
}

int ProgramSettings::reverbType() const
{
    return m_reverbType;
}

void ProgramSettings::setReverbType(int reverbType)
{
    m_reverbType = reverbType;
}

int ProgramSettings::bufferTime() const
{
    return m_bufferTime;
}

void ProgramSettings::setBufferTime(int bufferTime)
{
    m_bufferTime = bufferTime;
}
