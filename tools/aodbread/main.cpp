/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtAddOn.JsonDb module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore>

#include "btree.h"

#include "qbtree.h"
#include "qbtreetxn.h"
#include "qbtreecursor.h"

#include <stdint.h>

QString printable(const QByteArray &ba)
{
    QByteArray array = ba;

    QJsonDocument doc = QJsonDocument::fromBinaryData(ba);
    if (!doc.isNull())
        return QString("QSN:") + doc.toJson();

    // check if it is a number
    bool isNumber = true;
    for (int i = 0; i < array.size(); ++i) {
        if (array.at(i) < 0 || array.at(i) > 9) {
            isNumber = false;
            break;
        }
    }
    if (isNumber) {
        for (int i = 0; i < array.size(); ++i)
            array[i] = array.at(i)+'0';
        return QString("N:") +QString::fromLatin1(array);
    }

    // check if utf-16 latin string
    bool utf16 = true;
    for (int i = 0; i < array.size(); ++i) {
        if (i % 2 == 0) {
            if (!isprint(array.at(i))) {
                utf16 = false;
                break;
            }
        } else {
            if (array.at(i) != 0) {
                utf16 = false;
                break;
            }
        }
    }
    if (utf16)
        return QString("U:") + QString::fromUtf16((const ushort *)array.constData(), array.size()/2);

    for (int i = 0; i < array.size(); ++i) {
        if (array.at(i) == 0)
            array[i] = '_';
    }

    bool isprintable = true;
    for (int i = 0; i < array.size(); ++i) {
        if (!isprint(array.at(i))) {
            isprintable = false;
            break;
        }
    }
    if (isprintable)
        return QString("S:") + QString::fromLatin1(array);

    if (ba.size() == 5 && ba.at(4) == 'S') {
        // state change
        quint32 state = qFromBigEndian<quint32>((const uchar *)ba.mid(0, 4).constData());
        return QString("State:") + QString::number(state);
    }
    array = ba.toHex();
    return QString("x:") + QString::fromLatin1(array);
}

void makePrintable(const QByteArray &key, QString &keyString, const QByteArray &value, QString &valueString)
{
    if (key.size() == 5 && key.at(4) == 'S') {
        // state change
        quint32 state = qFromBigEndian<quint32>((const uchar *)key.mid(0, 4).constData());
        keyString = QString("State:") + QString::number(state);

        QStringList changes;
        const uchar *data = (const uchar *)value.constData();
        for (int i = 0; i < value.size() / 16; ++i) {
            quint32 startKey = qFromBigEndian<quint32>(&data[16 * i]);
            quint32 startTypeNumber = qFromBigEndian<quint32>(&data[16 * i + 4]);
            quint32 endKey = qFromBigEndian<quint32>(&data[16 * i + 8]);
            quint32 endTypeNumber = qFromBigEndian<quint32>(&data[16 * i + 12]);
            changes << QString("objectTypeKey %2->%4, objectKey %1->%3").arg(startKey).arg(startTypeNumber).arg(endKey).arg(endTypeNumber);
        }
        valueString = changes.join(",");
        return;
    }

    keyString = printable(key);
    valueString = printable(value);
}

bool gStat = false;
bool gDump = false;
bool gWantCompact = false;
bool gShowAll = false;
bool gShowAllReversed = false;
qint64 gDumpPage = 0;

void usage()
{
    qDebug() << QCoreApplication::arguments().at(0)
             << "[--dump-page num] [--stat] [--dump] [--compact] [--all|--all-reversed] database_file";
}

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    QStringList args = app.arguments();
    args.pop_front();

    if (args.isEmpty()) {
        usage();
        return 0;
    }

    for (; args.size() > 1; ) {
        QString arg = args.takeFirst();
        if (arg == QLatin1String("--stat")) {
            gStat = true;
        } else if (arg == QLatin1String("--dump")) {
            gDump = true;
        } else if (arg == QLatin1String("--compact")) {
            gWantCompact = true;
        } else if (arg == QLatin1String("--all")) {
            gShowAll = true;
        } else if (arg == QLatin1String("--all-reversed")) {
            gShowAllReversed = true;
        } else if (arg == QLatin1String("--dump-page")) {
            bool ok = false;
            gDumpPage = args.takeFirst().toInt(&ok);
            if (!ok) {
                usage();
                return 0;
            }
        } else {
            usage();
            return 0;
        }
    }

    if (args.isEmpty())
        return 0;

    QString filename = args.at(0);

    if (gDumpPage > 0) {
        return btree_dump_page_from_file(filename.toLocal8Bit().constData(), gDumpPage);
    } else if (gDumpPage < 0) {
        uint32_t page = 1;
        while (btree_dump_page_from_file(filename.toLocal8Bit().constData(), page))
            ++page;
        return 1;
    }

    QBtree db;
    db.setFileName(filename);
    db.setFlags(QBtree::ReadOnly);
    if (!db.open()) {
        qDebug() << "cannot open" << filename;
        usage();
        return 0;
    }

    if (gStat) {
        const struct btree_stat *bs = btree_stat(db.handle());
        qDebug() << "stat:";
        qDebug() << "\thits:" << bs->hits;
        qDebug() << "\treads:" << bs->reads;
        qDebug() << "\tmax_cache:" << bs->max_cache;
        qDebug() << "\tbranch_pages:" << bs->branch_pages;
        qDebug() << "\tleaf_pages:" << bs->leaf_pages;
        qDebug() << "\toverflow_pages:" << bs->overflow_pages;
        qDebug() << "\trevisions:" << bs->revisions;
        qDebug() << "\tdepth:" << bs->depth;
        qDebug() << "\tentries:" << bs->entries;
        qDebug() << "\tpsize:" << bs->psize;
        qDebug() << "\tcreated_at:" << bs->created_at << "(" << QDateTime::fromTime_t(bs->created_at) << ")";
        qDebug() << "\ttag:" << bs->tag;
        qDebug() << "";
    }

    if (gDump) {
        db.dump();
    }

    if (gShowAllReversed) {
        QBtreeCursor cursor(&db);
        QByteArray key, value;
        int i = 0;
        for (bool ok = cursor.last(); ok && cursor.current(&key, &value); ok = cursor.prev(), ++i) {
            QString keyString, valueString;
            makePrintable(key, keyString, value, valueString);
            qDebug() << i << ":" << keyString << ":" << valueString;
        }
    } else if (gShowAll) {
        QBtreeCursor cursor(&db);
        QByteArray key, value;
        int i = 0;
        for (bool ok = cursor.first(); ok && cursor.current(&key, &value); ok = cursor.next(), ++i) {
            QString keyString, valueString;
            makePrintable(key, keyString, value, valueString);
            qDebug() << i << ":" << keyString << ":" << valueString;
        }
    }

    if (gWantCompact) {
        db.close();
        db.setFileName(filename);
        db.setFlags(QBtree::Default);
        db.open();
        if (db.compact()) {
            qDebug() << "compacted.";
        } else {
            qDebug() << "failed to compact" << filename;
        }
        db.close();
    }
    return 1;
}
