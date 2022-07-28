/*
    Sonivox EAS Synthesizer for Qt applications
    Copyright (C) 2016-2022, Pedro Lopez-Cabanillas <plcl@users.sf.net>

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

#ifndef MIDIIOWRAPPER_H
#define MIDIIOWRAPPER_H

#include <QString>
#include <QFile>
#include <eas_types.h>

class FileWrapper
{
public:
    FileWrapper(const QString path);
    FileWrapper(const char *path);
    ~FileWrapper();
    EAS_FILE_LOCATOR getLocator();
    int readAt(void *buffer, int offset, int size);
    int size();

private:
    QFile *m_file;
    off64_t m_Base;
    int64_t  m_Length;
    EAS_FILE m_easFile;
};

#endif // MIDIIOWRAPPER_H
