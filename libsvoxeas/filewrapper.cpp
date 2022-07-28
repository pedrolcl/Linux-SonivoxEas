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

#include <QDebug>
#include "filewrapper.h"

static int
readAt(void *handle, void *buffer, int pos, int size) {
    return ((FileWrapper*)handle)->readAt(buffer, pos, size);
}

static int
size(void *handle) {
    return ((FileWrapper*)handle)->size();
}

FileWrapper::FileWrapper(const QString path)
{
    //qDebug() << Q_FUNC_INFO << path;
    m_file = new QFile(path);
    m_file->open(QIODevice::ReadOnly);
    m_Base = 0;
    m_Length = m_file->size();
}

FileWrapper::FileWrapper(const char *path)
{
    //qDebug("FileWrapper(path=%s)", path);
    m_file = new QFile(path);
    m_file->open(QIODevice::ReadOnly);
    m_Base = 0;
    m_Length = m_file->size();
}

FileWrapper::~FileWrapper() {
    //qDebug("~FileWrapper");
    m_file->close();
}

int
FileWrapper::readAt(void *buffer, int offset, int size) {
    //qDebug("readAt(%p, %d, %d)", buffer, offset, size);
    m_file->seek(offset);
    if (offset + size > m_Length) {
        size = m_Length - offset;
    }
    return m_file->read((char *)buffer, size);
}

int
FileWrapper::size() {
    //qDebug("size() = %d", int(mLength));
    return m_Length;
}

EAS_FILE_LOCATOR
FileWrapper::getLocator() {
    m_easFile.handle = this;
    m_easFile.readAt = ::readAt;
    m_easFile.size = ::size;
    return &m_easFile;
}
