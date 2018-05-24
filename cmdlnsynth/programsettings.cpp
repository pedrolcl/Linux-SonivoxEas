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
    emit ValuesChanged();
}

void ProgramSettings::internalSave(QSettings &settings)
{
    settings.setValue("BufferTime", m_bufferTime);
    settings.sync();
}

int ProgramSettings::bufferTime() const
{
    return m_bufferTime;
}

void ProgramSettings::setBufferTime(int bufferTime)
{
    m_bufferTime = bufferTime;
}
